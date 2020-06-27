using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Grpc.Net.Client;
using GrpcTestService;
using GrpcPiVisService;

namespace TestClientWpf
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private GrpcChannel channel;
        private TestService.TestServiceClient testclient;
        private PiVis.PiVisClient pivisclient;


        public MainWindow()
        {
            AppContext.SetSwitch("System.Net.Http.SocketsHttpHandler.Http2UnencryptedSupport", true);
            channel = GrpcChannel.ForAddress("http://127.0.0.1:50051");
            testclient = new TestService.TestServiceClient(channel);
            pivisclient = new PiVis.PiVisClient(channel);
            InitializeComponent();
            CurrentTab.Content = "Home";
            TabSelector.ItemsSource = pivisclient.GetServices(new Empty()).Services;
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            testclient.SetMessage(new StringRequest() { Text = Message.Text });
        }

        private void TabSelector_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            foreach (var item in e.AddedItems)
            {
                var newTab = item as GrpcTestService.TabIndex?;
                if (item != null)
                    testclient.SetTab(new ChangeTab() { Tab = newTab ?? GrpcTestService.TabIndex.Home });

                CurrentTab.Content = testclient.GetTab(new EmptyResult()).Tab.ToString();
            }
        }

        private async void Slider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            System.Diagnostics.Debug.WriteLine(e.NewValue);
            await testclient.SetHueAsync(new HueRequest() { Hue = e.NewValue });
        }
    }
}

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

namespace TestClientWpf
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private GrpcChannel channel;
        private TestService.TestServiceClient client;


        public MainWindow()
        {
            AppContext.SetSwitch("System.Net.Http.SocketsHttpHandler.Http2UnencryptedSupport", true);
            channel = GrpcChannel.ForAddress("http://192.168.0.236:50051");
            client = new TestService.TestServiceClient(channel);
            InitializeComponent();
            CurrentTab.Content = "Home";
            TabSelector.ItemsSource = Enum.GetValues(typeof(GrpcTestService.TabIndex));
            TabSelector.SelectedItem = GrpcTestService.TabIndex.Home;
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            client.SetMessage(new StringRequest() { Text = Message.Text });
        }

        private void TabSelector_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            foreach (var item in e.AddedItems)
            {
                var newTab = item as GrpcTestService.TabIndex?;
                if (item != null)
                    client.SetTab(new ChangeTab() { Tab = newTab ?? GrpcTestService.TabIndex.Home });

                CurrentTab.Content = client.GetTab(new EmptyResult()).Tab.ToString();
            }
        }

        private async void Slider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            System.Diagnostics.Debug.WriteLine(e.NewValue);
            await client.SetHueAsync(new HueRequest() { Hue = e.NewValue });
        }
    }
}

using Grpc.Net.Client;
using Grpc.Core.Utils;
using GrpcTestService;
using System;
using System.Net.Http;
using System.Threading.Tasks;

namespace TestClientConsole
{
    class Program
    {
        static async Task Main(string[] args)
        {
            
            AppContext.SetSwitch("System.Net.Http.SocketsHttpHandler.Http2UnencryptedSupport", true);
            using GrpcChannel channel = GrpcChannel.ForAddress("http://127.0.0.1:50051");
            TestService.TestServiceClient client = new TestService.TestServiceClient(channel);

            TabIndex opt = TabIndex.Home;
            try
            {
            while(Enum.TryParse(Console.ReadLine(), true, out opt))
                await client.SetTabAsync(new ChangeTab() { Tab = opt});
            }
            catch (Exception e)
            {
                Console.WriteLine(e);    
            }
        }
    }
}

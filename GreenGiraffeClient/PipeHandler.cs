using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.Pipes;
using System.Diagnostics;
using System.Windows.Forms;

namespace GreenGiraffeClient
{
    class PipeHandler
    {
        //names of the pipe we're gonna be communicating with
        string outPipeName, inPipeName;
        PipeSecurity pipeSecurity;

        public PipeHandler(string outPipeName, string inPipeName)
        {
            this.outPipeName = outPipeName;
            this.inPipeName = inPipeName;

            //make sure everyone can access the pipe
            pipeSecurity = new PipeSecurity();
            PipeAccessRule psRule = new PipeAccessRule(@"Everyone", PipeAccessRights.ReadWrite, System.Security.AccessControl.AccessControlType.Allow);
            pipeSecurity.AddAccessRule(psRule);

        }

        //as a client, send a message to the "server", in this case would be dll hack
        public async void sendMessageAsync(string msg)
        {
            try
            {
                //open the pipe
                using (NamedPipeClientStream pipe = new NamedPipeClientStream(".", outPipeName, PipeDirection.InOut))
                {
                    using(StreamWriter writer = new StreamWriter(pipe, Encoding.UTF8))
                    {
                        pipe.Connect();

                        //write message to pipe stream
                        await writer.WriteAsync(msg);
                    }
                }
            }
            catch (Exception exception)
            {
                //op somethin went wrong
                MessageBox.Show(exception.Message, "sendMessageAsync error");
            }
        }

        //set up background listener to client (dll hack)
        private async void createListenPipe(Action<string> messageReceived)
        { 
            try
            {
                while(true)
                {
                    using (var pipe = new NamedPipeServerStream(inPipeName, PipeDirection.In, maxNumberOfServerInstances: 1, PipeTransmissionMode.Byte, PipeOptions.Asynchronous, 4096, 4096, pipeSecurity))
                    {
                        //wait for connection
                        await pipe.WaitForConnectionAsync();

                        using (var reader = new StreamReader(pipe))
                        {
                            //read from the stream
                            var message = await reader.ReadToEndAsync();

                            //invoke message received function
                            messageReceived(message);
                        }
                    }
                }
            }
            catch (Exception exception)
            {
                //FUTURE KYLE: cannot access file or smthn, file is the same name as pipe?
                MessageBox.Show(exception.Message, "createListenPipe error");
            }
        }

        public void listen()
        {
            Task.Run(() => createListenPipe((string msg) =>
            {
                //dll sent some data, update stuff or smthn
                //i dont fkin know lmao
                MessageBox.Show(msg, "yeah");
            }));
        }
    }
}

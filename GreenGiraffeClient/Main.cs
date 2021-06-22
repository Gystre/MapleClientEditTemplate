using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.IO.Pipes;

namespace GreenGiraffeClient
{
    public partial class Main : Form
    {
        public Main()
        {
            InitializeComponent();

            //connect the pipe
            NamedPipeClientStream pipe = new NamedPipeClientStream(".", "GGClientPipe", PipeDirection.InOut);
            pipe.Connect();

            //once connected, read in the alive message to open the form
            using (StreamReader rdr = new StreamReader(pipe, Encoding.UTF8))
            {
                rdr.ReadToEnd();
            }
        }
    }
}

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
        PipeHandler pipeHandler;

        public Main()
        {
            InitializeComponent();

            //start listening for events
            pipeHandler = new PipeHandler("GGClientPipeOUT", "GGClientPipeIN");
            pipeHandler.listen();

        }
    }
}

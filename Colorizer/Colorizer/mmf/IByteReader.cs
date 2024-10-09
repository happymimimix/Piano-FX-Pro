using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework
{
    interface IByteReader : IDisposable
    {
        byte Read();
    }
}

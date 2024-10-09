using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework.MIDIEvents
{
    public class TempoEvent : MIDIEvent
    {
        public int Tempo { get; set; }

        public TempoEvent(double delta, int tempo) : base(delta)
        {
            Tempo = tempo;
        }

        public override MIDIEvent Clone()
        {
            return new TempoEvent(DeltaTime, Tempo);
        }

        public override byte[] GetData()
        {
            byte[] data = new byte[6];
            data[0] = 0xFF;
            data[1] = 0x51;
            data[2] = 0x03;
            data[3] = (byte)((Tempo >> 16) & 0xFF);
            data[4] = (byte)((Tempo >> 8) & 0xFF);
            data[5] = (byte)(Tempo & 0xFF);
            return data;
        }
    }
}

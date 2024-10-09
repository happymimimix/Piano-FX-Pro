using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework.MIDIEvents
{
    public class ProgramChangeEvent : ChannelEvent
    {
        public byte Program { get; set; }

        public ProgramChangeEvent(double delta, byte channel, byte program) : base(delta, channel)
        {
            Program = program;
        }

        public override MIDIEvent Clone()
        {
            return new ProgramChangeEvent(DeltaTime, Channel, Program);
        }

        public override byte[] GetData()
        {
            return new byte[]
            {
                (byte)(0b11000000 | Channel),
                Program
            };
        }
    }
}

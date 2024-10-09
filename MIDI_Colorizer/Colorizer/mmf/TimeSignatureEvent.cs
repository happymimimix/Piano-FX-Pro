using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework.MIDIEvents
{
    public class TimeSignatureEvent : MIDIEvent
    {
        public byte Numerator { get; set; }
        public byte Denominator { get; set; }
        public byte TicksPerClick { get; set; }
        public byte BB { get; set; }

        public TimeSignatureEvent(double delta, byte nn, byte dd, byte cc, byte bb) : base(delta)
        {
            Numerator = nn;
            Denominator = dd;
            TicksPerClick = cc;
            BB = bb;
        }

        public override MIDIEvent Clone()
        {
            return new TimeSignatureEvent(DeltaTime, Numerator, Denominator, TicksPerClick, BB);
        }

        public override byte[] GetData()
        {
            return new byte[]
            {
                0xFF, 0x58, 0x04, Numerator, Denominator, TicksPerClick, BB
            };
        }
    }
}

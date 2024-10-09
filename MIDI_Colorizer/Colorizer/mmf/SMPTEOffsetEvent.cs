using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework.MIDIEvents
{
    public class SMPTEOffsetEvent : MIDIEvent
    {
        public byte Hours { get; set; }
        public byte Minutes { get; set; }
        public byte Seconds { get; set; }
        public byte Frames { get; set; }
        public byte FractionalFrames { get; set; }

        public SMPTEOffsetEvent(double delta, byte hr, byte mn, byte se, byte fr, byte ff) : base(delta)
        {
            Hours = hr;
            Minutes = mn;
            Seconds = se;
            Frames = fr;
            FractionalFrames = ff;
        }

        public override MIDIEvent Clone()
        {
            return new SMPTEOffsetEvent(DeltaTime, Hours, Minutes, Seconds, Frames, FractionalFrames);
        }

        public override byte[] GetData()
        {
            return new byte[]
            {
                0xFF, 0x54, 0x05, Hours, Minutes, Seconds, Frames, FractionalFrames
            };
        }
    }
}

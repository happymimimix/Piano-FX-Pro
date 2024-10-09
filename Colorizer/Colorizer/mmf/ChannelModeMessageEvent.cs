using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework.MIDIEvents
{
    public class ChannelModeMessageEvent : ChannelEvent
    {
        public byte C { get; set; }
        public byte V { get; set; }

        public ChannelModeMessageEvent(double delta, byte channel, byte cc, byte vv) : base(delta, channel)
        {
            C = cc;
            V = vv;
        }

        public override MIDIEvent Clone()
        {
            return new ChannelModeMessageEvent(DeltaTime, Channel, C, V);
        }

        public override byte[] GetData()
        {
            return new byte[]
            {
                (byte)(0b10110000 | Channel),
                C,
                V
            };
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework.MIDIEvents
{
    public class ChannelPrefixEvent : ChannelEvent
    {
        public ChannelPrefixEvent(double delta, byte channel) : base(delta, channel)
        {
        }

        public override MIDIEvent Clone()
        {
            return new ChannelPrefixEvent(DeltaTime, Channel);
        }

        public override byte[] GetData()
        {
            return new byte[] { 0xFF, 0x20, 0x01, Channel };
        }
    }
}

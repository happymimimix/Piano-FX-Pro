using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework.MIDIEvents
{
    public abstract class NoteEvent : ChannelEvent
    {
        public byte Key { get; set; }

        public NoteEvent(double delta, byte key, byte channel) : base(delta, channel)
        {
            Key = key;
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework.MIDIEvents
{
    public class SystemExclusiveMessageEvent : MIDIEvent
    {
        byte[] data;

        public SystemExclusiveMessageEvent(double delta, byte[] data) : base(delta)
        {
            this.data = data;
        }

        public override MIDIEvent Clone()
        {
            return new SystemExclusiveMessageEvent(DeltaTime, (byte[])data.Clone());
        }

        public override byte[] GetData()
        {
            return data;
        }
    }
}

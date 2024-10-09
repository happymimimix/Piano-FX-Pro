using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework.MIDIEvents
{
    public class EndOfExclusiveEvent : MIDIEvent
    {
        public EndOfExclusiveEvent(double delta) : base(delta) { }

        public override MIDIEvent Clone()
        {
            return new EndOfExclusiveEvent(DeltaTime);
        }

        public override byte[] GetData()
        {
            return new byte[] { 0b11110111 };
        }
    }
}

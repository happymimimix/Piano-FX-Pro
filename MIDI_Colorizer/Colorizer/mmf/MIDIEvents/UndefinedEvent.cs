using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework.MIDIEvents
{
    public class UndefinedEvent : MIDIEvent
    {
        public byte Command { get; set; }

        public UndefinedEvent(double delta, byte command) : base(delta)
        {
            Command = command;
        }

        public override MIDIEvent Clone()
        {
            return new UndefinedEvent(DeltaTime, Command);
        }

        public override byte[] GetData()
        {
            return new byte[] { Command };
        }

    }
}

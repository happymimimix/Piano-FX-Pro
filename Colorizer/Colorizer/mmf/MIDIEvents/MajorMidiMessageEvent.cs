using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework.MIDIEvents
{
    public class MajorMidiMessageEvent : MIDIEvent
    {
        byte[] data;

        public MajorMidiMessageEvent(double delta, byte command) : base(delta)
        {
            data = new byte[] { command };
        }

        public MajorMidiMessageEvent(double delta, byte command, byte var1) : base(delta)
        {
            data = new byte[] { command, var1 };
        }

        public MajorMidiMessageEvent(double delta, byte command, byte var1, byte var2) : base(delta)
        {
            data = new byte[] { command, var1, var2 };
        }

        public override MIDIEvent Clone()
        {
            if (data.Length == 1)
                return new MajorMidiMessageEvent(DeltaTime, data[0]);
            if (data.Length == 2)
                return new MajorMidiMessageEvent(DeltaTime, data[0], data[1]);
            if (data.Length == 3)
                return new MajorMidiMessageEvent(DeltaTime, data[0], data[1], data[2]);
            else throw new Exception("Bad Things™ happened");
        }

        public override byte[] GetData()
        {
            return data;
        }
    }
}

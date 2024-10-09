using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework.MIDIEvents
{
    public class PitchWheelChangeEvent : ChannelEvent
    {
        short value;
        public short Value
        {
            get => value;
            set
            {
                if (value > 8191) this.value = 8191;
                else if (value < -8192) this.value = -8192;
                else this.value = value;
            }
        }

        public PitchWheelChangeEvent(double delta, byte channel, short value) : base(delta, channel)
        {
            Value = value;
        }

        public override MIDIEvent Clone()
        {
            return new PitchWheelChangeEvent(DeltaTime, Channel, Value);
        }

        public override byte[] GetData()
        {
            int val = value + 8192;
            return new byte[]
            {
                (byte)(0b11100000 | Channel),
                (byte)(val & 0x7F),
                (byte)((val >> 7) & 0x7F)
            };
        }
    }
}

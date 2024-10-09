using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework.MIDIEvents
{
    public class ChannelPressureEvent : ChannelEvent
    {
        public byte Pressure { get; set; }

        public ChannelPressureEvent(double delta, byte channel, byte pressure) : base(delta, channel)
        {
            Pressure = pressure;
        }

        public override MIDIEvent Clone()
        {
            return new ChannelPressureEvent(DeltaTime, Channel, Pressure);
        }

        public override byte[] GetData()
        {
            return new byte[]
            {
                (byte)(0b11010000 | Channel),
                Pressure
            };
        }
    }
}

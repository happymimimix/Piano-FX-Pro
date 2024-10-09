using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework.MIDIEvents
{
    public class ControlChangeEvent : ChannelEvent
    {
        public byte Controller { get; set; }
        public byte Value { get; set; }

        public ControlChangeEvent(double delta, byte channel, byte controller, byte value) : base(delta, channel)
        {
            Controller = controller;
            Value = value;
        }

        public override MIDIEvent Clone()
        {
            return new ControlChangeEvent(DeltaTime, Channel, Controller, Value);
        }

        public override byte[] GetData()
        {
            return new byte[]
            {
                (byte)(0b10110000 | Channel),
                Controller,
                Value
            };
        }
    }
}

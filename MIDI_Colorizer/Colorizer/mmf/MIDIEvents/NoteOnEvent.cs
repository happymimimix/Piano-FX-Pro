using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework.MIDIEvents
{
    public class NoteOnEvent : NoteEvent
    {
        public byte Velocity { get; set; }

        public NoteOnEvent(double delta, byte channel, byte key, byte velocity) : base(delta, key, channel)
        {
            Key = key;
            Velocity = velocity;
        }

        public override MIDIEvent Clone()
        {
            return new NoteOnEvent(DeltaTime, Channel, Key, Velocity);
        }

        public override byte[] GetData()
        {
            return new byte[]
            {
                (byte)(0b10010000 | Channel),
                Key,
                Velocity
            };
        }
    }
}

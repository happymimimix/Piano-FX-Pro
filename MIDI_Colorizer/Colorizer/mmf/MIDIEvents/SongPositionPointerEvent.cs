using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework.MIDIEvents
{
    public class SongPositionPointerEvent : MIDIEvent
    {
        public ushort Location { get; set; }

        public SongPositionPointerEvent(double delta, ushort location) : base(delta)
        {
            Location = location;
        }

        public override MIDIEvent Clone()
        {
            return new SongPositionPointerEvent(DeltaTime, Location);
        }

        public override byte[] GetData()
        {
            return new byte[]
            {
                0b11110010,
                (byte)(Location & 0x7F),
                (byte)((Location >> 7) & 0x7F)
            };
        }
    }
}

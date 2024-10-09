using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework.MIDIEvents
{
    public class SongSelectEvent : MIDIEvent
    {
        public byte Song { get; set; }

        public SongSelectEvent(double delta, byte song) : base(delta)
        {
            Song = song;
        }

        public override MIDIEvent Clone()
        {
            return new SongSelectEvent(DeltaTime, Song);
        }

        public override byte[] GetData()
        {
            return new byte[]
            {
                0b11110011,
                Song
            };
        }
    }
}

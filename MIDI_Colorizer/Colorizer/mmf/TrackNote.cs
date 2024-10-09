using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework
{
    public class TrackNote : Note
    {
        public int Track { get; set; }

        public TrackNote(int track, byte channel, byte key, byte vel, double start, double end) : base(channel, key, vel, start, end)
        { 
            Track = track; 
        }

        public TrackNote(int track, Note n) : this(track, n.Channel, n.Key, n.Velocity, n.Start, n.End)
        { }

        public override Note Clone()
        {
            return new TrackNote(Track, Channel, Key, Velocity, Start, End);
        }
    }
}

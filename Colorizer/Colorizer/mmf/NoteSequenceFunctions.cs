using MIDIModificationFramework.MIDIEvents;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework
{
    public static class NoteSequenceFunctions
    {
        public static IEnumerable<MIDIEvent> ExtractEvents(this IEnumerable<Note> seq)
        {
            return NoteConversion.EncodeNotes(seq);
        }

        public static IEnumerable<MIDIEvent> ExtractEvents(this IEnumerable<Note> seq, FastList<MIDIEvent> buffer)
        {
            return Mergers.MergeWithBuffer(NoteConversion.EncodeNotes(seq), buffer);
        }

        public static IEnumerable<TrackNote> ToTrackNotes(this IEnumerable<Note> seq, int track)
        {
            return NoteConversion.ToTrackNotes(seq, track);
        }

        public static IEnumerable<T> MergeAll<T>(this IEnumerable<IEnumerable<T>> seq)
            where T : Note
        {
            return Mergers.MergeSequences(seq);
        }

        public static IEnumerable<T> MergeAllMany<T>(this IEnumerable<IEnumerable<T>> seq)
            where T : Note
        {
            return Mergers.MergeManySequences(seq);
        }

        public static IEnumerable<T> MergeWith<T>(this IEnumerable<T> seq, IEnumerable<T> seq2)
            where T : Note
        {
            return Mergers.MergeSequences(new[] { seq, seq2 });
        }

        public static IEnumerable<T> TrimStart<T>(this IEnumerable<T> seq)
            where T : Note => TrimStart(seq, 0);
        public static IEnumerable<T> TrimStart<T>(this IEnumerable<T> seq, double time)
            where T : Note
        {
            foreach (var n in seq)
            {
                if (n.End < time) continue;
                if (n.Start < time)
                {
                    var nc = n.Clone() as T;
                    nc.SetStartOnly(time);
                    if(nc.Length < 0.00000001) continue;
                    yield return nc;
                }
                else
                {
                    yield return n;
                }
            }
        }

        public static IEnumerable<T> TrimEnd<T>(this IEnumerable<T> seq, double time)
            where T : Note
        {
            foreach (var n in seq)
            {
                if (n.Start > time) continue;
                if (n.End > time)
                {
                    var nc = n.Clone() as T;
                    nc.End = time;
                    if(nc.Length < 0.00000001) continue;
                    yield return nc;
                }
                else
                {
                    yield return n;
                }
            }
        }
    }
}

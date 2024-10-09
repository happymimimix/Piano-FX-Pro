using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MIDIModificationFramework.MIDIEvents;

namespace MIDIModificationFramework.Generator
{
    public static class TransformExtensions
    {
        public static IEnumerable<T> SetChannel<T>(this IEnumerable<T> seq, int channel)
            where T : Note
        {
            foreach (var n in seq.CloneNotes())
            {
                n.Channel = (byte)channel;
                yield return n;
            }
        }

        public static IEnumerable<IEnumerable<T>> SetChannel<T>(this IEnumerable<IEnumerable<T>> seq, int channel)
            where T : Note => seq.Select(s => s.SetChannel(channel));

        public static IEnumerable<T> SetEventsChannel<T>(this IEnumerable<T> seq, int channel)
            where T : MIDIEvent
        {
            foreach (var e in seq)
            {
                if (e is ChannelEvent)
                {
                    var ce = e.Clone() as ChannelEvent;
                    ce.Channel = (byte)channel;
                    yield return ce as T;
                }
                else
                {
                    yield return e;
                }
            }
        }

        public static IEnumerable<IEnumerable<T>> SetEventsChannel<T>(this IEnumerable<IEnumerable<T>> seq, int channel)
            where T : MIDIEvent => seq.Select(s => s.SetEventsChannel(channel));

        public static IEnumerable<T> OffsetKeys<T>(this IEnumerable<T> seq, int keys)
            where T : Note
        {
            foreach (var n in seq.CloneNotes())
            {
                int k = n.Key + keys;
                if (k < 0 || k > 127) continue;
                n.Key = (byte)k;
                yield return n;
            }
        }

        public static IEnumerable<IEnumerable<T>> OffsetKeys<T>(this IEnumerable<IEnumerable<T>> seq, int keys)
            where T : Note => seq.Select(s => s.OffsetKeys(keys));

        public static IEnumerable<T> OffsetEventKeys<T>(this IEnumerable<T> seq, int keys)
            where T : MIDIEvent
        {
            foreach (var e in seq)
            {
                if (e is NoteEvent)
                {
                    var ke = e.Clone() as NoteEvent;
                    int k = ke.Key + keys;
                    if (k < 0 || k > 127) continue;
                    ke.Key = (byte)k;
                    yield return ke as T;
                }
                else
                {
                    yield return e;
                }
            }
        }

        public static IEnumerable<IEnumerable<T>> OffsetEventKeys<T>(this IEnumerable<IEnumerable<T>> seq, int keys)
            where T : MIDIEvent => seq.Select(s => s.OffsetEventKeys(keys));
    }
}

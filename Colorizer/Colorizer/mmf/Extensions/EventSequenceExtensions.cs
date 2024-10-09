using MIDIModificationFramework.MIDIEvents;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework
{
    public static class EventSequenceExtensions
    {
        public static IEnumerable<MIDIEvent> CancelTempoEvents(this IEnumerable<MIDIEvent> seq, double newTempo)
        {
            return SequenceFunctions.CancelTempoEvents(seq, newTempo);
        }

        public static IEnumerable<MIDIEvent> CancelTempoEvents(this IEnumerable<MIDIEvent> seq, double newTempo, bool returnTempos)
        {
            return SequenceFunctions.CancelTempoEvents(seq, newTempo, returnTempos);
        }

        public static IEnumerable<MIDIEvent> MakeTimeBased(this IEnumerable<MIDIEvent> seq, double originalPPQ)
        {
            return SequenceFunctions.CancelTempoEvents(seq, 250000 / originalPPQ);
        }

        public static IEnumerable<MIDIEvent> RoundDeltas(this IEnumerable<MIDIEvent> seq)
        {
            return SequenceFunctions.RoundDeltas(seq);
        }

        public static IEnumerable<MIDIEvent> InjectEvents(this IEnumerable<MIDIEvent> seq, Func<MIDIEvent> generator)
        {
            return SequenceFunctions.EventInjector(seq, generator);
        }

        public static IEnumerable<T> FilterEvents<T>(this IEnumerable<MIDIEvent> seq)
            where T : MIDIEvent
        {
            double delta = 0;
            foreach (var e in seq)
            {
                if (e is T)
                {
                    var ev = e.Clone() as T;
                    ev.DeltaTime += delta;
                    delta = 0;
                    yield return ev;
                }
                else
                {
                    delta += e.DeltaTime;
                }
            }
        }

        public static IEnumerable<MIDIEvent> FilterEvents(this IEnumerable<MIDIEvent> seq, IEnumerable<Type> types)
        {
            double delta = 0;
            foreach (var e in seq)
            {
                bool extends = false;
                foreach (var t in types)
                {
                    if (t.IsInstanceOfType(e))
                    {
                        extends = true;
                        break;
                    }
                }
                if (extends)
                {
                    var ev = e.Clone();
                    ev.DeltaTime += delta;
                    delta = 0;
                    yield return ev;
                }
                else
                {
                    delta += e.DeltaTime;
                }
            }
        }

        public static IEnumerable<T> RemoveEvents<T>(this IEnumerable<T> seq, IEnumerable<Type> types)
            where T : MIDIEvent
        {
            double delta = 0;
            foreach (var e in seq)
            {
                bool extends = false;
                foreach (var t in types)
                {
                    if (t.IsInstanceOfType(e))
                    {
                        extends = true;
                        break;
                    }
                }
                if (!extends)
                {
                    var ev = e.Clone() as T;
                    ev.DeltaTime += delta;
                    delta = 0;
                    yield return ev;
                }
                else
                {
                    delta += e.DeltaTime;
                }
            }
        }

        public static IEnumerable<T> ChangePPQ<T>(this IEnumerable<T> seq, double startPPQ, double endPPQ)
            where T : MIDIEvent
        {
            return SequenceFunctions.PPQChange(seq, startPPQ, endPPQ);
        }

        public static IEnumerable<T> ChangePPQ<T>(this IEnumerable<T> seq, double ppqMultiplier)
            where T : MIDIEvent
        {
            return SequenceFunctions.PPQChange(seq, 1, ppqMultiplier);
        }

        public static IEnumerable<T> MergeWith<T>(this IEnumerable<T> seq, IEnumerable<T> seq2)
            where T : MIDIEvent
        {
            return Mergers.MergeSequences(seq, seq2);
        }

        public static IEnumerable<T> MergeAllTracks<T>(this IEnumerable<IEnumerable<T>> seqs)
            where T : MIDIEvent
        {
            return Mergers.MergeSequences(seqs);
        }

        public static IEnumerable<MIDIEvent> MergeBuffer(this IEnumerable<MIDIEvent> seq, FastList<MIDIEvent> buffer)
        {
            return Mergers.MergeWithBuffer(seq, buffer);
        }

        public static IEnumerable<Note> ExtractNotes(this IEnumerable<MIDIEvent> seq)
        {
            return NoteConversion.ExtractNotes(seq);
        }

        public static IEnumerable<Note> ExtractNotes(this IEnumerable<MIDIEvent> seq, FastList<MIDIEvent> otherEvents)
        {
            return NoteConversion.ExtractNotes(seq, otherEvents);
        }

        public static IEnumerable<MIDIEvent> FilterEvents(this IEnumerable<MIDIEvent> seq, Func<MIDIEvent, bool> select)
        {
            return SequenceFunctions.FilterEvents(seq, select);
        }
    }
}

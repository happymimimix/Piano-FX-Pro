using MIDIModificationFramework.MIDIEvents;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework
{
    public static class SequenceFunctions
    {
        public static IEnumerable<MIDIEvent> CancelTempoEvents(IEnumerable<MIDIEvent> sequence, double newTempo, bool returnTempos = false)
        {
            double extraTicks = 0;
            int tempo = 500000;
            double lastDiff = 0;

            newTempo = tempo * (tempo / newTempo);

            foreach (var _e in sequence)
            {
                var e = _e.Clone();
                e.DeltaTime = e.DeltaTime / newTempo * tempo + extraTicks;
                extraTicks = 0;
                if (e is TempoEvent)
                {
                    var ev = e as TempoEvent;
                    tempo = ev.Tempo;
                    extraTicks = e.DeltaTime + lastDiff;
                    lastDiff = 0;
                    if (returnTempos)
                    {
                        ev.Tempo = (int)newTempo;
                        yield return ev;
                    }
                    continue;
                }
                yield return e;
            }
        }

        public static IEnumerable<MIDIEvent> EventInjector(IEnumerable<MIDIEvent> sequence, Func<MIDIEvent> generator)
        {
            MIDIEvent nextGenerated = generator().Clone();
            foreach (var _e in sequence)
            {
                var e = _e.Clone();
                while (nextGenerated.DeltaTime < e.DeltaTime)
                {
                    e.DeltaTime -= nextGenerated.DeltaTime;
                    yield return nextGenerated;
                    nextGenerated = generator().Clone();
                }
                nextGenerated.DeltaTime -= e.DeltaTime;
                yield return e;
            }
        }

        public static IEnumerable<T> ExtractEvent<T>(IEnumerable<MIDIEvent> sequence)
            where T : MIDIEvent
        {
            double delta = 0;
            foreach (var e in sequence)
            {
                delta += e.DeltaTime;
                if (e is T)
                {
                    var _e = e.Clone() as T;
                    _e.DeltaTime = delta;
                    delta = 0;
                    yield return _e;
                }
            }
        }

        public static IEnumerable<T> PPQChange<T>(IEnumerable<T> sequence, double startPPQ, double endPPQ)
            where T : MIDIEvent
        {
            var ppqRatio = endPPQ / startPPQ;
            foreach (var _e in sequence)
            {
                var e = _e.Clone() as T;
                e.DeltaTime *= ppqRatio;
                yield return e;
            }
        }

        public static IEnumerable<T> RoundDeltas<T>(IEnumerable<T> sequence)
            where T : MIDIEvent
        {
            double time = 0;
            double roundedtime = 0;
            foreach (var _e in sequence)
            {
                var e = _e.Clone() as T;
                time += e.DeltaTime;
                var round = Math.Round(time);
                e.DeltaTime = round - roundedtime;
                roundedtime = round;
                yield return e;
            }
        }

        public static IEnumerable<T> FilterEvents<T>(IEnumerable<T> sequence, Func<T, bool> func)
            where T : MIDIEvent
        {
            double extraDelta = 0;
            foreach (var _e in sequence)
            {
                if (func(_e))
                {
                    var e = _e.Clone() as T;
                    e.DeltaTime += extraDelta;
                    extraDelta = 0;
                    yield return e;
                }
                else
                {
                    extraDelta += _e.DeltaTime;
                }
            }
        }
    }
}

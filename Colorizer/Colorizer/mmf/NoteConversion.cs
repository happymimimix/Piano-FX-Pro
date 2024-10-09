using MIDIModificationFramework.MIDIEvents;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework
{
    public static class NoteConversion
    {
        class DecodedNote
        {
            public Note note;
            public bool ended = false;
        }

        class UnplacedNoteOff
        {
            public NoteOffEvent e;
            public double time;
        }

        public static IEnumerable<Note> ExtractNotes(IEnumerable<MIDIEvent> sequence, FastList<MIDIEvent> otherEvents = null)
        {
            double time = 0;
            FastList<DecodedNote>[] unendedNotes = new FastList<DecodedNote>[256 * 16];
            FastList<DecodedNote> notesQueue = new FastList<DecodedNote>();
            for (int i = 0; i < unendedNotes.Length; i++) unendedNotes[i] = new FastList<DecodedNote>();
            double delta = 0;
            foreach (var e in sequence)
            {
                time += e.DeltaTime;
                if (e is NoteOnEvent)
                {
                    var n = e as NoteOnEvent;
                    var note = new DecodedNote()
                    {
                        note = new Note(n.Channel, n.Key, n.Velocity, time, double.PositiveInfinity)
                    };
                    unendedNotes[n.Key * 16 + n.Channel].Add(note);
                    notesQueue.Add(note);
                    delta += e.DeltaTime;
                }
                else if (e is NoteOffEvent)
                {
                    var n = e as NoteOffEvent;
                    var arr = unendedNotes[n.Key * 16 + n.Channel];
                    if (arr.ZeroLen) continue;
                    var note = arr.Pop();
                    note.ended = true;
                    note.note.End = time;
                    delta += e.DeltaTime;
                }
                else
                {
                    if (otherEvents != null)
                    {
                        var ev = e.Clone();
                        ev.DeltaTime += delta;
                        delta = 0;
                        otherEvents.Add(ev);
                    }
                }
                if (notesQueue.First != null && notesQueue.First.ended)
                {
                    yield return notesQueue.Pop().note;
                }
            }
            foreach (DecodedNote un in notesQueue)
            {
                if (!un.ended)
                {
                    un.note.End = time;
                }
                yield return un.note;
            }
            notesQueue.Unlink();
            foreach (var s in unendedNotes) s.Unlink();
        }

        public static IEnumerable<MIDIEvent> EncodeNotes(IEnumerable<Note> sequence)
        {
            List<UnplacedNoteOff> noteOffs = new List<UnplacedNoteOff>();
            double prevTime = 0;

            foreach (var n in sequence)
            {
                while (noteOffs.Count != 0 && noteOffs[0].time <= n.Start)
                {
                    var e = noteOffs[0];
                    noteOffs.RemoveAt(0);
                    e.e.DeltaTime = e.time - prevTime;
                    yield return e.e;
                    prevTime = e.time;
                }

                yield return new NoteOnEvent(n.Start - prevTime, n.Channel, n.Key, n.Velocity);
                prevTime = n.Start;
                var time = n.End;
                var off = new UnplacedNoteOff() { e = new NoteOffEvent(0, n.Channel, n.Key), time = time };
                var pos = noteOffs.Count / 2;
                if (noteOffs.Count == 0) noteOffs.Add(off);
                else
                {
                    // binary search
                    for (int jump = noteOffs.Count / 4; ; jump /= 2)
                    {
                        if (jump <= 0) jump = 1;
                        if (pos < 0) pos = 0;
                        if (pos >= noteOffs.Count) pos = noteOffs.Count - 1;
                        var u = noteOffs[pos];
                        if (u.time >= time)
                        {
                            if (pos == 0 || noteOffs[pos - 1].time < time)
                            {
                                noteOffs.Insert(pos, off);
                                break;
                            }
                            else pos -= jump;
                        }
                        else
                        {
                            if (pos == noteOffs.Count - 1)
                            {
                                noteOffs.Add(off);
                                break;
                            }
                            else pos += jump;
                        }
                    }
                }
            }

            foreach (var e in noteOffs)
            {
                e.e.DeltaTime = e.time - prevTime;
                yield return e.e;
                prevTime = e.time;
            }
        }

        public static IEnumerable<TrackNote> ToTrackNotes(IEnumerable<Note> notes, int track)
        {
            foreach (var n in notes) yield return new TrackNote(track, n);
        }
    }
}

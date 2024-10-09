using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework.Generator
{
    public static class NoteSequenceExtensions
    {
        public static IEnumerable<T> CloneNotes<T>(this IEnumerable<T> seq)
            where T : Note
        {
            foreach (var n in seq) yield return n.Clone() as T;
        }

        public static IEnumerable<T> OffsetTime<T>(this IEnumerable<T> seq, double ticks)
            where T : Note
        {
            foreach (var n in seq.CloneNotes())
            {
                n.Start += ticks;
                yield return n;
            }
        }

        public static IEnumerable<IEnumerable<T>> OffsetTime<T>(this IEnumerable<IEnumerable<T>> seq, double ticks)
            where T : Note => seq.Select(s => s.OffsetTime(ticks));

        public static IEnumerable<IEnumerable<MIDIEvent>> ExtractEvents(this IEnumerable<IEnumerable<Note>> seq) =>
            seq.Select(s => s.ExtractEvents());

        public static IEnumerable<IEnumerable<T>> TrimStart<T>(this IEnumerable<IEnumerable<T>> seq)
            where T : Note => TrimStart(seq, 0);
        public static IEnumerable<IEnumerable<T>> TrimStart<T>(this IEnumerable<IEnumerable<T>> seq, double start)
            where T : Note => seq.Select(s => s.TrimStart(start));

        public static IEnumerable<IEnumerable<T>> TrimEnd<T>(this IEnumerable<IEnumerable<T>> seq, double end)
            where T : Note => seq.Select(s => s.TrimEnd(end));

        public static IEnumerable<IEnumerable<T>> Pack<T>(this IEnumerable<IEnumerable<T>> seq, int tracks)
            where T : Note
        {
            var all = seq.ToArray();

            IEnumerable<IEnumerable<T>> select(int offset)
            {
                for (int i = offset; i < all.Length; i += tracks)
                {
                    yield return all[i];
                }
            }

            for (int i = 0; i < tracks; i++)
            {
                yield return select(i).MergeAll();
            }
        }

        public static IEnumerable<Note> Silhouette(this IEnumerable<Note> seq)
        {
            Queue<Note>[] unendedNotes = new Queue<Note>[256 * 16];
            FastList<Note> notesQueue = new FastList<Note>();
            for (int i = 0; i < unendedNotes.Length; i++) unendedNotes[i] = new Queue<Note>();

            Queue<Note> queueFromNote(Note n) => unendedNotes[n.Key * 16 + n.Channel];

            foreach (var n in seq)
            {
                var q = queueFromNote(n);
                if (q.Count == 0 || q.Last().End + 0.00000001 < n.Start)
                {
                    var newNote = new Note(n.Channel, n.Key, 1, n.Start, n.End);
                    q.Enqueue(newNote);
                    notesQueue.Add(newNote);
                }
                else
                {
                    q.Last().End = n.End;
                }
                if (!notesQueue.ZeroLen && queueFromNote(notesQueue.First).Count > 1)
                {
                    queueFromNote(notesQueue.First).Dequeue();
                    yield return notesQueue.Pop();
                }
            }
            foreach (Note n in notesQueue)
            {
                yield return n;
            }
            notesQueue.Unlink();
        }

        public static IEnumerable<IEnumerable<Note>> Silhouette(this IEnumerable<IEnumerable<Note>> seq) =>
            seq.Select(s => s.Silhouette());
    }
}

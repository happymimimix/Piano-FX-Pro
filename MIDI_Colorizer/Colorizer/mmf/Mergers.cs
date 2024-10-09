using MIDIModificationFramework.MIDIEvents;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework
{
    public static class Mergers
    {
        public static IEnumerable<T> MergeSequences<T>(IEnumerable<T> sequence1, IEnumerable<T> sequence2, bool noClone = false)
            where T : MIDIEvent
        {
            var enum1 = sequence1.GetEnumerator();
            var enum2 = sequence2.GetEnumerator();
            T e1 = null;
            T e2 = null;
            if (noClone)
            {
                if (enum1.MoveNext()) e1 = enum1.Current;
                if (enum2.MoveNext()) e2 = enum2.Current;
            }
            else
            {
                if (enum1.MoveNext()) e1 = enum1.Current.Clone() as T;
                if (enum2.MoveNext()) e2 = enum2.Current.Clone() as T;
            }

            while (true)
            {
                if (e1 != null)
                {
                    if (e2 != null)
                    {
                        if (e1.DeltaTime <= e2.DeltaTime)
                        {
                            e2.DeltaTime -= e1.DeltaTime;
                            yield return e1;
                            if (enum1.MoveNext())
                            {
                                if (noClone) e1 = enum1.Current;
                                else e1 = enum1.Current.Clone() as T;
                            }
                            else e1 = null;
                        }
                        else
                        {
                            e1.DeltaTime -= e2.DeltaTime;
                            yield return e2;
                            if (enum2.MoveNext())
                            {
                                if (noClone) e2 = enum2.Current;
                                else e2 = enum2.Current.Clone() as T;
                            }
                            else e2 = null;
                        }
                    }
                    else
                    {
                        yield return e1;
                        if (enum1.MoveNext())
                        {
                            if (noClone) e1 = enum1.Current;
                            else e1 = enum1.Current.Clone() as T;
                        }
                        else e1 = null;
                    }
                }
                else
                {
                    if (e2 == null) break;
                    else yield return e2;
                    if (enum2.MoveNext())
                    {
                        if (noClone) e2 = enum2.Current;
                        else e2 = enum2.Current.Clone() as T;
                    }
                    else e2 = null;
                }
            }
        }

        public static IEnumerable<T> MergeSequences<T>(IEnumerable<T> sequence1, IEnumerable<T> sequence2)
            where T : Note
        {
            var enum1 = sequence1.GetEnumerator();
            var enum2 = sequence2.GetEnumerator();
            T n1 = null;
            T n2 = null;
            if (enum1.MoveNext()) n1 = enum1.Current;
            if (enum2.MoveNext()) n2 = enum2.Current;

            while (true)
            {
                if (n1 != null)
                {
                    if (n2 != null)
                    {
                        if (n1.Start < n2.Start)
                        {
                            yield return n1;
                            if (enum1.MoveNext()) n1 = enum1.Current;
                            else n1 = null;
                        }
                        else
                        {
                            yield return n2;
                            if (enum2.MoveNext()) n2 = enum2.Current;
                            else n2 = null;
                        }
                    }
                    else
                    {
                        yield return n1;
                        if (enum1.MoveNext()) n1 = enum1.Current;
                        else n1 = null;
                    }
                }
                else
                {
                    if (n2 == null) break;
                    else yield return n2;
                    if (enum2.MoveNext()) n2 = enum2.Current;
                    else n2 = null;
                }
            }
        }

        public static IEnumerable<T> MergeSequences<T>(IEnumerable<IEnumerable<T>> sequences, bool noClone = false)
            where T : MIDIEvent
        {
            var batch1 = new List<IEnumerable<T>>();
            var batch2 = new List<IEnumerable<T>>();
            foreach (var s in sequences) batch1.Add(s);
            if (batch1.Count == 0) return new T[0];
            while (batch1.Count > 1)
            {
                int pos = 0;
                while (pos < batch1.Count)
                {
                    if (batch1.Count - pos == 1)
                    {
                        batch2.Add(batch1[pos]);
                        pos += 1;
                    }
                    else
                    {
                        batch2.Add(MergeSequences(batch1[pos], batch1[pos + 1], noClone));
                        pos += 2;
                    }
                }
                batch1 = batch2;
                batch2 = new List<IEnumerable<T>>();
            }
            return batch1[0];
        }

        public static IEnumerable<T> MergeSequences<T>(IEnumerable<IEnumerable<T>> sequences)
            where T : Note
        {
            var batch1 = new List<IEnumerable<T>>();
            var batch2 = new List<IEnumerable<T>>();
            foreach (var s in sequences) batch1.Add(s);
            if (batch1.Count == 0) return new T[0];
            while (batch1.Count > 1)
            {
                int pos = 0;
                while (pos < batch1.Count)
                {
                    if (batch1.Count - pos == 1)
                    {
                        batch2.Add(batch1[pos]);
                        pos += 1;
                    }
                    else
                    {
                        batch2.Add(MergeSequences(batch1[pos], batch1[pos + 1]));
                        pos += 2;
                    }
                }
                batch1 = batch2;
                batch2 = new List<IEnumerable<T>>();
            }
            return batch1[0];
        }

        public static IEnumerable<MIDIEvent> MergeWithBuffer(IEnumerable<MIDIEvent> sequence, FastList<MIDIEvent> buffer)
        {
            double bdeltasub = 0;
            foreach (var _e in sequence)
            {
                var e = _e.Clone();
                while (!buffer.ZeroLen && buffer.First.DeltaTime - bdeltasub < e.DeltaTime)
                {
                    var be = buffer.Pop().Clone();
                    be.DeltaTime -= bdeltasub;
                    bdeltasub = 0;
                    e.DeltaTime -= be.DeltaTime;
                    yield return be;
                }
                yield return e;
                bdeltasub += e.DeltaTime;
            }
            while (!buffer.ZeroLen)
            {
                var be = buffer.Pop().Clone();
                be.DeltaTime -= bdeltasub;
                bdeltasub = 0;
                yield return be;
            }
        }

        public static IEnumerable<T> MergeManySequences<T>(IEnumerable<IEnumerable<T>> sequences)
            where T : Note
        {
            bool mainEnded = false;
            var mainIter = sequences.GetEnumerator();
            var lists = new List<IEnumerator<T>>();
            IEnumerator<T> nextList = null;

            double prevstart = -1;

            void makeNextList()
            {
                while (!mainEnded)
                {
                    if (!mainIter.MoveNext())
                    {
                        mainEnded = true;
                        break;
                    }
                    var seq = mainIter.Current;
                    var it = seq.GetEnumerator();
                    if (it.MoveNext())
                    {
                        nextList = it;
                        break;
                    }
                }
                if (mainEnded) nextList = null;
            }

            makeNextList();
            if (nextList != null)
            {
                while (!(mainEnded && lists.Count == 0))
                {
                    double min = -1;
                    int minid = -1;
                    for (int i = 0; i < lists.Count; i++)
                    {
                        if (lists[i].Current.Start < min || i == 0)
                        {
                            min = lists[i].Current.Start;
                            minid = i;
                        }
                    }
                    if (minid == -1 || (nextList != null && nextList.Current.Start < lists[minid].Current.Start))
                    {
                        lists.Add(nextList);
                        minid = lists.Count - 1;
                        makeNextList();
                    }

                    if (prevstart > lists[minid].Current.Start)
                    { }
                    prevstart = lists[minid].Current.Start;

                    yield return lists[minid].Current;
                    if (!lists[minid].MoveNext()) lists.RemoveAt(minid);
                }
            }
        }
    }
}

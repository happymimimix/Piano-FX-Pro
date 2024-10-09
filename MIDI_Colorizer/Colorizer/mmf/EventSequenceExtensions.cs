using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework.Generator
{
    public static class EventSequenceExtensions
    {
        public static IEnumerable<T> CloneEvents<T>(this IEnumerable<T> seq)
            where T : MIDIEvent
        {
            foreach (var n in seq) yield return n.Clone() as T;
        }
    }
}

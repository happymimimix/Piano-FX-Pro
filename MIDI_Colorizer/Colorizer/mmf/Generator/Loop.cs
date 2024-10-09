using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework.Generator
{
    public static class Loop
    {
        public static IEnumerable<T> For<T>(int start, int count, Func<int, T> action)
        {
            for(int i = start; i < start + count; i++)
            {
                yield return action(i);
            }
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework.Generator
{
    public static class Fields
    {
        public static IEnumerable<Note> Block(double length, double noteDensity) => Basic(length, noteDensity, (a, b) => true);

        public static IEnumerable<Note> Basic(double length, double noteDensity, Func<double, double, bool> place)
        {
            double size = 1 / noteDensity;
            for(double i = 0; i < length; i += size)
            {
                var end = i + size;
                if (end > length) end = length;
                for (byte k = 0; k < 128; k++)
                {
                    if (place(k, i)) 
                        yield return new Note(0, k, 1, i, end);
                }
            }
        }

        public static IEnumerable<Note> Basic<T>(double length, double noteDensity, T val, Func<double, double, T, bool> place) =>
            Basic(length, noteDensity, (a, b) => place(a, b, val));
        public static IEnumerable<Note> Basic<T, T2>(double length, double noteDensity, T val, T2 val2, Func<double, double, T, T2, bool> place) =>
            Basic(length, noteDensity, (a, b) => place(a, b, val, val2));
        public static IEnumerable<Note> Basic<T, T2, T3>(double length, double noteDensity, T val, T2 val2, T3 val3, Func<double, double, T, T2, T3, bool> place) =>
            Basic(length, noteDensity, (a, b) => place(a, b, val, val2, val3));

        public static IEnumerable<IEnumerable<Note>> RangeSplit(IEnumerable<Note> notes, int tracks, Func<Note, double> func)
        {
            return Loop.For(0, tracks, i =>
                notes.Where(n => (func(n) % tracks) - i < 1)
            );
        }

        public static IEnumerable<IEnumerable<Note>> RangeSplit(IEnumerable<Note> notes, int tracks, Func<double, double, double> func) =>
            RangeSplit(notes, tracks, (n) => func(n.Key, n.Start));
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework.Generator
{
    public static class Keep
    {
        public static T Value<T, V1>(V1 value, Func<V1, T> func) => func(value);
        public static T Value<T, V1, V2>(V1 value, V2 value2, Func<V1, V2, T> func) => func(value, value2);
        public static T Value<T, V1, V2, V3>(V1 value, V2 value2, V3 value3, Func<V1, V2, V3, T> func) => func(value, value2, value3);
    }
}

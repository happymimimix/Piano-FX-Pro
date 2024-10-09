using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework.Generator
{
    public static class MathHelper
    {
        public static void Rotate(ref double x, ref double y, double angle)
        {
            var newy = x * Math.Sin(angle) + y * Math.Cos(angle);
            x = x * Math.Cos(angle) - y * Math.Sin(angle);
            y = newy;
        }
    }
}

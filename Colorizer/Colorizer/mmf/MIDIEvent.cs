using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework
{
    public abstract class MIDIEvent
    {
        double deltatime;
        public double DeltaTime
        {
            get => deltatime;
            set
            {
                if (value < -0.0000001) throw new ArgumentException("Negative delta time not allowed", "delta");
                deltatime = value;
            }
        }

        public MIDIEvent(double delta)
        {
            DeltaTime = delta;
        }
        public abstract byte[] GetData();

        IEnumerable<byte> MakeVariableLenFast(int i)
        {
            var b = new byte[5];
            int len = 4;
            byte added = 0x00;
            while (true)
            {
                byte v = (byte)(i & 0x7F);
                i = i >> 7;
                v = (byte)(v | added);
                b[len--] = v;
                added = 0x80;
                if (i == 0)
                {
                    break;
                }
            }
            return b.Skip(len + 1);
        }

        public byte[] MakeVariableLen(int i)
        {
            return MakeVariableLenFast(i).ToArray();
        }

        public byte[] GetDataWithDelta()
        {
            if (DeltaTime < 0 || double.IsNaN(DeltaTime)) throw new Exception("Invalid delta time detected: " + DeltaTime);
            return MakeVariableLenFast((int)Math.Round(DeltaTime)).Concat(GetData()).ToArray();
        }

        public abstract MIDIEvent Clone();
    }
}

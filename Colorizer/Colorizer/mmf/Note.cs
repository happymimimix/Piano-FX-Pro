using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework
{
    public class Note
    {
        double length;

        public double Start { get; set; }
        public byte Channel { get; set; }
        public byte Key { get; set; }
        public byte Velocity { get; set; }
        
        public double End
        {
            get => Start + length;
            set
            {
                Length = value - Start;
            }
        }

        public double Length
        {
            get => length;
            set
            {
                if (value < -0.00000001) 
                    throw new ArgumentException("Note can not have a negative length");
                length = value;
            }
        }

        public void SetStartOnly(double newStart)
        {
            double newLength = End - newStart;
            Start = newStart;
            Length = newLength;
        }

        public Note(byte channel, byte key, byte vel, double start, double end)
        {
            Channel = channel;
            this.Start = start;
            this.Length = end - start;
            this.Key = key;
            this.Velocity = vel;
        }

        public virtual Note Clone()
        {
            return new Note(Channel, Key, Velocity, Start, End);
        }
    }
}

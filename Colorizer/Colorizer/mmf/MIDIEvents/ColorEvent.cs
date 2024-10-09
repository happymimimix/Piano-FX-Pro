using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework.MIDIEvents
{
    public class ColorEvent : MIDIEvent
    {
        byte channel = 0x7f;
        public byte Channel {
            get => channel;
            set
            {
                if (value > 15 && value != 0x7f) throw new ArgumentException("Channel can only be between 0 and 16, or 7F for all channels");
                channel = value;
            }
        }
        bool gradients;

        public byte R { get; set; }
        public byte G { get; set; }
        public byte B { get; set; }
        public byte A { get; set; }
        public byte R2 { get; set; }
        public byte G2 { get; set; }
        public byte B2 { get; set; }
        public byte A2 { get; set; }

        public ColorEvent(double delta, byte r, byte g, byte b, byte a) : base(delta)
        {
            R = r;
            G = g;
            B = b;
            A = a;

            gradients = false;
        }

        public ColorEvent(double delta, byte channel, byte r, byte g, byte b, byte a) : base(delta)
        {
            if (channel == 0x7f) Channel = 0x7f;
            else Channel = (byte)(channel % 16);
            R = r;
            G = g;
            B = b;
            A = a;

            gradients = false;
        }

        public ColorEvent(double delta, byte r, byte g, byte b, byte a, byte r2, byte g2, byte b2, byte a2) : base(delta)
        {
            R = r;
            G = g;
            B = b;
            A = a;
            R2 = r2;
            G2 = g2;
            B2 = b2;
            A2 = a2;

            gradients = true;
        }

        public ColorEvent(double delta, byte channel, byte r, byte g, byte b, byte a, byte r2, byte g2, byte b2, byte a2) : base(delta)
        {
            if (channel == 0x7f) Channel = 0x7f;
            else Channel = (byte)(channel % 16);

            R = r;
            G = g;
            B = b;
            A = a;
            R2 = r2;
            G2 = g2;
            B2 = b2;
            A2 = a2;

            gradients = true;
        }

        private ColorEvent(double delta, byte channel, byte r, byte g, byte b, byte a, byte r2, byte g2, byte b2, byte a2, bool gradients) : base(delta)
        {
            if (channel == 0x7f) Channel = 0x7f;
            else Channel = (byte)(channel % 16);

            R = r;
            G = g;
            B = b;
            A = a;
            R2 = r2;
            G2 = g2;
            B2 = b2;
            A2 = a2;

            this.gradients = gradients;
        }

        public override MIDIEvent Clone()
        {
            return new ColorEvent(DeltaTime, Channel, R, G, B, A, R2, G2, B2, A2, gradients);
        }

        public override byte[] GetData()
        {
            byte[] data;
            if (gradients)
                data = new byte[15];
            else
                data = new byte[11];
            data[0] = 0xFF;
            data[1] = 0x0A;
            if (gradients)
                data[2] = 0x0B;
            else
                data[2] = 0x08;
            data[3] = 0x00;
            data[4] = 0x0F;
            data[5] = Channel;
            data[6] = 0x00;

            data[7] = R;
            data[8] = G;
            data[9] = B;
            data[10] = A;

            if (gradients)
            {
                data[11] = R2;
                data[12] = G2;
                data[13] = B2;
                data[14] = A2;
            }

            return data;
        }
    }
}

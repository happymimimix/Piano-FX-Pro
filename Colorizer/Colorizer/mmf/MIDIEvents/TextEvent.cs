using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework.MIDIEvents
{
    public enum TextEventType
    {
        TextEvent = 1,
        CopyrightNotice = 2,
        TrackName = 3,
        InstrumentName = 4,
        Lyric = 5,
        Marker = 6,
        CuePoint = 7,
        ProgramName = 8,
        DeviceName = 9,
        Undefined = 10,
        MetaEvent = 0x7F
    }

    public class TextEvent : MIDIEvent
    {
        byte[] data;

        public string Text
        {
            get => new string(data.Select(b => (char)b).ToArray());
            set
            {
                data = value.ToArray().Select(c => (byte)c).ToArray();
            }
        }

        public byte[] Bytes
        {
            get => data;
            set
            {
                data = value;
            }
        }

        public int Length => data.Length;

        public byte Type { get; set; }

        public TextEvent(double delta, byte type, byte[] data) : base(delta)
        {
            Type = type;
            Bytes = data;
        }

        public TextEvent(double delta, byte type, string text) : base(delta)
        {
            Type = type;
            Bytes = data;
        }

        public override MIDIEvent Clone()
        {
            return new TextEvent(DeltaTime, Type, (byte[])data.Clone());
        }

        public override byte[] GetData()
        {
            byte[] len = MakeVariableLen(Length);
            return new byte[] { 0xFF, (byte)Type }
                .Concat(
                    len
                ).Concat(
                    data
                ).ToArray();
        }
    }
}

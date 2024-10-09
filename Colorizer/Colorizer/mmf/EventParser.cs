using MIDIModificationFramework.MIDIEvents;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework
{
    public class EventParser : IDisposable
    {
        class StreamByteReader : IByteReader
        {
            Stream stream;

            public StreamByteReader(Stream s)
            {
                stream = s;
            }

            public void Dispose() => stream.Dispose();
            public byte Read()
            {
                int b = stream.ReadByte();
                if (b == -1) throw new EndOfStreamException();
                return (byte)b;
            }
        }

        IByteReader reader;
        long TrackTime { get; set; } = 0;

        public bool Ended { get; private set; } = false;

        internal EventParser(IByteReader reader)
        {
            this.reader = reader;
        }

        public EventParser(Stream reader)
        {
            this.reader = new StreamByteReader(reader);
        }

        uint ReadVariableLen()
        {
            long n = 0;
            while (true)
            {
                byte curByte = Read();
                n = (n << 7) | (byte)(curByte & 0x7F);
                if ((curByte & 0x80) == 0)
                {
                    break;
                }
            }
            return (uint)n;
        }

        int pushback = -1;
        byte Read()
        {
            if (pushback != -1)
            {
                byte p = (byte)pushback;
                pushback = -1;
                return p;
            }
            return reader.Read();
        }

        byte prevCommand;
        public MIDIEvent ParseNextEvent()
        {
            if (Ended) return null;
            uint delta = ReadVariableLen();
            TrackTime += delta;
            byte command = Read();
            if (command < 0x80)
            {
                pushback = command;
                command = prevCommand;
            }
            prevCommand = command;
            byte comm = (byte)(command & 0b11110000);
            if (comm == 0b10010000)
            {
                byte channel = (byte)(command & 0b00001111);
                byte note = Read();
                byte vel = Read();
                if (vel == 0) return new NoteOffEvent(delta, channel, note);
                return new NoteOnEvent(delta, channel, note, vel);
            }
            else if (comm == 0b10000000)
            {
                byte channel = (byte)(command & 0b00001111);
                byte note = Read();
                byte vel = Read();
                return new NoteOffEvent(delta, channel, note);
            }
            else if (comm == 0b10100000)
            {
                byte channel = (byte)(command & 0b00001111);
                byte note = Read();
                byte vel = Read();
                return new PolyphonicKeyPressureEvent(delta, channel, note, vel);
            }
            else if (comm == 0b10110000)
            {
                byte channel = (byte)(command & 0b00001111);
                byte controller = Read();
                byte value = Read();
                return new ControlChangeEvent(delta, command, controller, value);
            }
            else if (comm == 0b11000000)
            {
                byte program = Read();
                return new ProgramChangeEvent(delta, command, program);
            }
            else if (comm == 0b11010000)
            {
                byte pressure = Read();
                return new ChannelPressureEvent(delta, command, pressure);
            }
            else if (comm == 0b11100000)
            {
                byte var1 = Read();
                byte var2 = Read();
                return new PitchWheelChangeEvent(delta, command, (short)(((var2 << 7) | var1) - 8192));
            }
            else if (comm == 0b10110000)
            {
                byte cc = Read();
                byte vv = Read();
                return new ChannelModeMessageEvent(delta, command, cc, vv);
            }
            else if (command == 0b11110000)
            {
                List<byte> data = new List<byte>() { command };
                byte b = 0;
                while (b != 0b11110111)
                {
                    b = Read();
                    data.Add(b);
                }
                return new SystemExclusiveMessageEvent(delta, data.ToArray());
            }
            else if (command == 0b11110100 || command == 0b11110001 || command == 0b11110101 || command == 0b11111001 || command == 0b11111101)
            {
                return new UndefinedEvent(delta, command);
            }
            else if (command == 0b11110010)
            {
                byte var1 = Read();
                byte var2 = Read();
                return new SongPositionPointerEvent(delta, (ushort)((var2 << 7) | var1));
            }
            else if (command == 0b11110011)
            {
                byte pos = Read();
                return new SongSelectEvent(delta, pos);
            }
            else if (command == 0b11110110)
            {
                return new TuneRequestEvent(delta);
            }
            else if (command == 0b11110111)
            {
                return new EndOfExclusiveEvent(delta);
            }
            else if (command == 0b11111000)
            {
                return new MajorMidiMessageEvent(delta, command);
            }
            else if (command == 0b11111010)
            {
                return new MajorMidiMessageEvent(delta, command);
            }
            else if (command == 0b11111100)
            {
                return new MajorMidiMessageEvent(delta, command);
            }
            else if (command == 0b11111110)
            {
                return new MajorMidiMessageEvent(delta, command);
            }
            else if (command == 0xFF)
            {
                command = Read();
                if (command == 0x00)
                {
                    if (Read() != 2)
                    {
                        throw new Exception("Corrupt Track");
                    }
                    return new TrackStartEvent();
                }
                else if ((command >= 0x01 && command <= 0x0A) || command == 0x7F)
                {
                    int size = (int)ReadVariableLen();
                    var data = new byte[size];
                    for (int i = 0; i < size; i++) data[i] = Read();
                    if (command == 0x0A &&
                        (size == 8 || size == 12) &&
                        data[0] == 0x00 && data[1] == 0x0F &&
                        (data[2] < 16 || data[2] == 0x7F) &&
                        data[3] == 0)
                    {
                        if (data.Length == 8)
                        {
                            return new ColorEvent(delta, data[2], data[4], data[5], data[6], data[7]);
                        }
                        return new ColorEvent(delta, data[2], data[4], data[5], data[6], data[7], data[8], data[9], data[10], data[11]);
                    }
                    else
                        return new TextEvent(delta, command, data);
                }
                else if (command == 0x20)
                {
                    command = Read();
                    if (command != 1)
                    {
                        throw new Exception("Corrupt Track");
                    }
                    return new ChannelPrefixEvent(delta, Read());
                }
                else if (command == 0x21)
                {
                    command = Read();
                    if (command != 1)
                    {
                        throw new Exception("Corrupt Track");
                    }
                    return new MIDIPortEvent(delta, Read());
                }
                else if (command == 0x2F)
                {
                    command = Read();
                    if (command != 0)
                    {
                        throw new Exception("Corrupt Track");
                    }
                    Ended = true;
                    return null;
                }
                else if (command == 0x51)
                {
                    command = Read();
                    if (command != 3)
                    {
                        throw new Exception("Corrupt Track");
                    }
                    int btempo = 0;
                    for (int i = 0; i != 3; i++)
                        btempo = (int)((btempo << 8) | Read());
                    return new TempoEvent(delta, btempo);
                }
                else if (command == 0x54)
                {
                    command = Read();
                    if (command != 5)
                    {
                        throw new Exception("Corrupt Track");
                    }
                    byte hr = Read();
                    byte mn = Read();
                    byte se = Read();
                    byte fr = Read();
                    byte ff = Read();
                    return new SMPTEOffsetEvent(delta, hr, mn, se, fr, ff);
                }
                else if (command == 0x58)
                {
                    command = Read();
                    if (command != 4)
                    {
                        throw new Exception("Corrupt Track");
                    }
                    byte nn = Read();
                    byte dd = Read();
                    byte cc = Read();
                    byte bb = Read();
                    return new TimeSignatureEvent(delta, nn, dd, cc, bb);
                }
                else if (command == 0x59)
                {
                    command = Read();
                    if (command != 2)
                    {
                        throw new Exception("Corrupt Track");
                    }
                    byte sf = Read();
                    byte mi = Read();
                    return new KeySignatureEvent(delta, sf, mi);
                }
                else
                {
                    throw new Exception("Corrupt Track");
                }
            }
            else
            {
                throw new Exception("Corrupt Track");
            }
        }

        public void Dispose()
        {
            reader.Dispose();
        }
    }
}
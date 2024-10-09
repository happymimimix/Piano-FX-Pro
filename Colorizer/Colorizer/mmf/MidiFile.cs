using MIDIModificationFramework.MIDIEvents;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework
{
    public class MidiFile : IDisposable
    {
        internal class MidiChunkPointer
        {
            public long Start { get; set; }
            public uint Length { get; set; }
        }

        public ushort Format { get; private set; }
        public ushort PPQ { get; private set; }
        public int TrackCount { get; private set; }

        internal MidiChunkPointer[] TrackLocations { get; private set; }

        Stream reader;
        DiskReadProvider readProvider;

        int readBufferSize = 100000;

        public BufferByteReader GetTrackByteReader(int track)
        {
            return new BufferByteReader(readProvider, 100000, TrackLocations[track].Start, TrackLocations[track].Length);
        }

        public IEnumerable<MIDIEvent> GetTrackUnsafe(int track)
        {
            var reader = new EventParser(GetTrackByteReader(track));
            while (!reader.Ended)
            {
                MIDIEvent ev;
                ev = reader.ParseNextEvent();
                if (ev == null) break;
                yield return ev;
            }
            reader.Dispose();
        }

        public IEnumerable<MIDIEvent> GetTrack(int track)
        {
            var reader = new EventParser(GetTrackByteReader(track));
            while (!reader.Ended)
            {
                MIDIEvent ev;
                try
                {
                    ev = reader.ParseNextEvent();
                }
                catch
                {
                    break;
                }
                if (ev == null) break;
                yield return ev;
            }
            reader.Dispose();
        }

        public IEnumerable<IEnumerable<MIDIEvent>> IterateTracks()
        {
            for (int i = 0; i < TrackCount; i++) yield return GetTrack(i);
        }

        string filepath;

        public MidiFile(Stream stream, int readBufferSize)
        {
            reader = stream;
            ParseHeaderChunk();
            List<MidiChunkPointer> tracks = new List<MidiChunkPointer>();
            while (reader.Position < reader.Length)
            {
                ParseTrackChunk(tracks);
            }
            TrackLocations = tracks.ToArray();
            TrackCount = TrackLocations.Length;
            readProvider = new DiskReadProvider(stream);
        }

        public MidiFile(Stream stream) : this(stream, 100000)
        { }

        public MidiFile(string filename, int readBufferSize) : this(File.Open(filename, FileMode.Open), readBufferSize)
        { }

        public MidiFile(string filename) : this(filename, 100000)
        { }

        void AssertText(string text)
        {
            foreach (char c in text)
            {
                if (reader.ReadByte() != c)
                {
                    throw new Exception("Corrupt chunk headers");
                }
            }
        }

        uint ReadInt32()
        {
            uint length = 0;
            for (int i = 0; i != 4; i++)
                length = (uint)((length << 8) | (byte)reader.ReadByte());
            return length;
        }

        ushort ReadInt16()
        {
            ushort length = 0;
            for (int i = 0; i != 2; i++)
                length = (ushort)((length << 8) | (byte)reader.ReadByte());
            return length;
        }

        void ParseHeaderChunk()
        {
            AssertText("MThd");
            uint length = (uint)ReadInt32();
            if (length != 6) throw new Exception("Header chunk size isn't 6");
            Format = ReadInt16();
            ReadInt16();
            PPQ = ReadInt16();
        }

        void ParseTrackChunk(List<MidiChunkPointer> tracks)
        {
            AssertText("MTrk");
            uint length = (uint)ReadInt32();
            tracks.Add(new MidiChunkPointer() { Start = reader.Position, Length = length });
            reader.Position += length;
        }

        public void Dispose()
        {
            reader.Dispose();
        }
    }
}

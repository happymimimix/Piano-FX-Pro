using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace MIDIModificationFramework
{
    internal class ParallelStreamIO : Stream
    {
        public override bool CanRead => true;

        public override bool CanSeek => true;

        public override bool CanWrite => true;

        long length = 0;
        public override long Length => length;

        public bool Closed = false;

        long position = 0;
        public override long Position
        {
            get => position;
            set
            {
                bool locked = false;
                if (!Monitor.IsEntered(stream))
                {
                    Monitor.Enter(stream);
                    locked = true;
                }
                int offset = (int)(value % pstream.ChunkDataSize);
                currentChunk = (value - offset) / pstream.ChunkDataSize;
                if (currentChunk >= locations.Count) throw new ArgumentOutOfRangeException("Position", "Position was out of range");
                currentChunkSize = ReadChunkSize(locations[(int)currentChunk]);
                currentChunkAvailableRead = currentChunkSize - offset;
                if (currentChunkAvailableRead < 0) throw new ArgumentOutOfRangeException("Position", "Position was out of range");
                currentChunkAvailableWrite = pstream.ChunkDataSize - offset;
                position = value;
                streampos = pstream.HeaderChunkSize + pstream.ChunkSize * locations[(int)currentChunk] + 4 + offset;
                if (locked) Monitor.Exit(stream);
            }
        }

        List<long> locations = new List<long>();

        long currentChunk = -1;
        int currentChunkSize = -1;
        int currentChunkAvailableRead = -1;
        int currentChunkAvailableWrite = -1;

        Func<long> GetNewChunk;

        Stream stream;
        long streampos;
        ParallelStream pstream;

        bool locked = false;

        public ParallelStreamIO(long length, List<long> locations, Func<long> getNewChunk, Stream stream, ParallelStream pstream)
        {
            this.length = length;
            this.locations = locations;
            GetNewChunk = getNewChunk;
            this.stream = stream;
            this.pstream = pstream;
            Position = 0;
        }

        public override void Flush()
        {

        }

        void WriteChunkSize(long chunk, int length)
        {
            stream.Position = pstream.HeaderChunkSize + pstream.ChunkSize * chunk;
            byte[] ib = new byte[4];
            int a = 0;
            for (int i = 3; i >= 0; i--) ib[a++] = (byte)((length >> (i * 8)) & 0xFF);
            stream.Write(ib, 0, 4);
        }

        int ReadChunkSize(long chunk)
        {
            stream.Position = pstream.HeaderChunkSize + pstream.ChunkSize * chunk;
            int v = 0;
            for (int j = 0; j < 4; j++) v = (v << 8) + stream.ReadByte();
            return v;
        }

        public override int Read(byte[] buffer, int offset, int count)
        {
            lock (stream)
            {
                int read = 0;
                if (currentChunkAvailableRead == 0) return 0;
                while (read != count)
                {
                    stream.Position = streampos;
                    if (currentChunkAvailableRead <= count - read)
                    {
                        stream.Read(buffer, offset + read, currentChunkAvailableRead);
                        read += currentChunkAvailableRead;
                        Position += currentChunkAvailableRead;
                        if (currentChunkAvailableRead == 0)
                        {
                            break;
                        }
                    }
                    else
                    {
                        int r = stream.Read(buffer, offset + read, count - read);
                        read = count;
                        Position += r;
                    }
                }
                return read;
            }
        }
        
        public override long Seek(long offset, SeekOrigin origin)
        {
            if (origin == SeekOrigin.Begin) Position = offset;
            if (origin == SeekOrigin.Current) Position += offset;
            if (origin == SeekOrigin.End) Position = length - offset;
            return position;
        }

        public override void SetLength(long value)
        {
            throw new NotImplementedException();
        }

        public override void Write(byte[] buffer, int offset, int count)
        {
            lock (stream)
            {
                if (Closed) throw new Exception("Stream closed");
                int written = 0;
                while (written != count)
                {
                    int w = 0;
                    stream.Position = streampos;
                    if (currentChunkAvailableWrite <= count - written)
                    {
                        stream.Write(buffer, offset + written, currentChunkAvailableWrite);
                        w = currentChunkAvailableWrite;
                    }
                    else
                    {
                        stream.Write(buffer, offset + written, count - written);
                        w = count - written;
                    }
                    if (w > currentChunkAvailableRead)
                    {
                        currentChunkSize += w - currentChunkAvailableRead;
                        if(Position + w < currentChunkSize)
                        { }
                        length += w - currentChunkAvailableRead;
                        WriteChunkSize(locations[(int)currentChunk], currentChunkSize);
                        if (w == currentChunkAvailableWrite && currentChunk == locations.Count - 1)
                        {
                            locations.Add(GetNewChunk());
                        }
                        Position += w;
                        written += w;
                    }
                    else
                    {
                        Position += w;
                        written += w;
                    }
                }
            }
        }

        public override void Close()
        {
            lock (stream)
            {
                Closed = true;
            }
        }
    }

    public class ParallelStream : IDisposable
    {
        public int ChunkSize { get; private set; } = 4096 * 4096;
        public int HeaderChunkSize { get; private set; } = 4096 * 4096;
        public int ChunkDataSize => ChunkSize - 4;

        Stream stream;

        long nextOpenChunk = 0;

        int[] headerData;

        List<ParallelStreamIO> streams = new List<ParallelStreamIO>();

        public ParallelStream(Stream stream, int chunkSize = 4096 * 4096, int headerChunkSize = 4096 * 4096)
        {
            lock (stream)
            {
                this.stream = stream;
                ChunkSize = chunkSize;
                HeaderChunkSize = headerChunkSize;
                if (stream.Length < HeaderChunkSize)
                {
                    stream.SetLength(HeaderChunkSize);
                }
                headerData = new int[headerChunkSize / 4];
                stream.Position = 0;
                for (int i = 0; i < headerData.Length; i++)
                {
                    int val = 0;
                    for (int j = 0; j < 4; j++) val = (val << 8) + stream.ReadByte();
                    headerData[i] = val;
                }
            }
        }

        public Stream GetStream(int id, bool readOnly = false)
        {
            lock (stream)
            {
                id += 1;
                if (id == 0) throw new ArgumentOutOfRangeException("id", "id can't be -1");
                List<long> locations = new List<long>();
                long length = 0;
                lock (stream)
                {
                    for (int i = 0; i < headerData.Length; i++)
                    {
                        if (headerData[i] == id)
                        {
                            locations.Add(i);
                            length += GetChunkLen(i);
                        }
                    }
                }
                if (locations.Count == 0) locations.Add(GetEmptyChunk(id));
                else
                {
                    if (!readOnly) throw new Exception("Can't write to a closed write stream. Consider setting readOnly = true");
                }
                var s = new ParallelStreamIO(length, locations, () => GetEmptyChunk(id), stream, this);
                if (readOnly) s.Close();
                else streams.Add(s);
                s.Position = 0;
                return s;
            }
        }

        int GetChunkLen(long chunk)
        {
            stream.Position = HeaderChunkSize + chunk * ChunkSize;
            int l = 0;
            for (int i = 0; i < 4; i++) l = (l << 8) + stream.ReadByte();
            return l;
        }

        void WriteInt(int v)
        {
            byte[] ib = new byte[4];
            int a = 0;
            for (int i = 3; i >= 0; i--) ib[a++] = (byte)((v >> (i * 8)) & 0xFF);
            stream.Write(ib, 0, 4);
        }

        void WriteHeaderEntry(long i)
        {
            stream.Position = i * 4;
            WriteInt(headerData[i]);
        }

        public void CloseAllStreams()
        {
            foreach (var s in streams) if (!s.Closed) s.Close();
            streams.Clear();
        }

        public void DeleteStream(int stream)
        {
            lock (this.stream)
            {
                stream++;
                streams = streams.Where(s => !s.Closed).ToList();
                if (streams.Count > 0) throw new Exception("All streams must be closed before using. Consider using ParallelStream.CloseAllStreams()");
                for (long i = 0; i < headerData.Length; i++)
                {
                    if (headerData[i] == stream) headerData[i] = 0;
                    WriteHeaderEntry(i);
                }
                nextOpenChunk = -1;
            }
        }

        void WriteFullHeaderChunk()
        {
            int l = HeaderChunkSize / 4;
            byte[] raw = new byte[HeaderChunkSize];
            int a = 0;
            for (int i = 0; i < l; i++)
            {
                int v = headerData[l];
                for (int j = 3; j >= 0; j--) raw[a++] = (byte)((v >> (j * 8)) & 0xFF);
            }
            stream.Position = 0;
            stream.Write(raw, 0, HeaderChunkSize);
        }

        int[] ParseHeaderChunk()
        {
            int l = HeaderChunkSize / 4;
            byte[] raw = new byte[HeaderChunkSize];
            int[] data = new int[l];
            stream.Read(raw, 0, HeaderChunkSize);
            unsafe
            {
                for (int i = 0; i < l; i++)
                {
                    int v = 0;
                    for (int j = 0; j < 4; j++) v = (v << 8) + raw[l];
                    data[i] = v;
                }
            }
            return data;
        }

        long GetEmptyChunk(int stream)
        {
            if (nextOpenChunk == -1)
            {
                nextOpenChunk = 0;
                while (headerData[nextOpenChunk] != 0) nextOpenChunk++;
            }
            headerData[nextOpenChunk] = stream;
            WriteHeaderEntry(nextOpenChunk);
            long c = nextOpenChunk;
            if (HeaderChunkSize + (nextOpenChunk + 1) * ChunkSize > this.stream.Length)
                this.stream.SetLength(HeaderChunkSize + (nextOpenChunk + 1) * ChunkSize);
            this.stream.Position = HeaderChunkSize + nextOpenChunk * ChunkSize;
            while (headerData[nextOpenChunk] != 0) nextOpenChunk++;
            this.stream.Write(new byte[] { 0, 0, 0, 0 }, 0, 4);
            return c;
        }

        public void Dispose()
        {
            stream.Close();
            stream.Dispose();
        }
    }
}

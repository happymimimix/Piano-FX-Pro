using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework
{
    public class XZStream : Stream
    {
        Process xz;
        Task copyTask;
        public XZStream(Stream write, bool autoClose = true, int threads = 0)
        {
            xz = new Process();
            xz.StartInfo = new ProcessStartInfo("xz", "-zc --threads=" + threads)
            {
                RedirectStandardOutput = true,
                RedirectStandardInput = true,
                UseShellExecute = false
            };
            xz.Start();
            copyTask = Task.Run(() =>
            {
                xz.StandardOutput.BaseStream.CopyTo(write);
                if (autoClose) write.Close();
            });
            stdin = xz.StandardInput.BaseStream;
            //xz.StandardInput.Close();
        }

        public override bool CanRead => false;

        public override bool CanSeek => false;

        public override bool CanWrite => true;

        public override long Length => throw new NotImplementedException();

        public override long Position { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }

        Stream stdin;

        public override void Flush()
        {
            stdin.Flush();
        }

        public override int Read(byte[] buffer, int offset, int count)
        {
            throw new NotImplementedException();
        }

        public override long Seek(long offset, SeekOrigin origin)
        {
            throw new NotImplementedException();
        }

        public override void SetLength(long value)
        {
            throw new NotImplementedException();
        }

        public override void Write(byte[] buffer, int offset, int count)
        {
            stdin.Write(buffer, offset, count);
        }

        public override void Close()
        {
            xz.StandardInput.Close();
            copyTask.GetAwaiter().GetResult();
            //xz.WaitForExit();
            base.Close();
        }
    }

    public static class XZ
    {
        public static Stream RemoveXZCompressionLayer(Stream input, int threads = 0)
        {
            Process xz = new Process();
            xz.StartInfo = new ProcessStartInfo("xz", "-dc --threads=" + threads)
            {
                RedirectStandardOutput = true,
                RedirectStandardInput = true,
                UseShellExecute = false
            };
            xz.Start();
            Task.Run(() =>
            {
                input.CopyTo(xz.StandardInput.BaseStream);
                xz.StandardInput.Close();
            });
            return xz.StandardOutput.BaseStream;
        }

        public static Stream AddXZCompressionLayer(Stream input, int threads = 0)
        {
            Process xz = new Process();
            xz.StartInfo = new ProcessStartInfo("xz", "-zc --threads=" + threads)
            {
                RedirectStandardOutput = true,
                RedirectStandardInput = true,
                UseShellExecute = false
            };
            xz.Start();
            Task.Run(() =>
            {
                input.CopyTo(xz.StandardInput.BaseStream);
                xz.StandardInput.Close();
            });
            return xz.StandardOutput.BaseStream;
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIDIModificationFramework.MIDIEvents
{
    public class KeySignatureEvent : MIDIEvent
    {
        public byte SF { get; set; }
        public byte MI { get; set; }

        public KeySignatureEvent(double delta, byte sf, byte mi) : base(delta)
        {
            SF = sf;
            MI = mi;
        }

        public override MIDIEvent Clone()
        {
            return new KeySignatureEvent(DeltaTime, SF, MI);
        }

        public override byte[] GetData()
        {
            return new byte[] { 0xFF, 0x59, 0x02, SF, MI };
        }
    }
}

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using MIDIModificationFramework;
using MIDIModificationFramework.MIDIEvents;

namespace Colorizer
{
    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                if (args.Length > 0)
                {
                    if (args[args.Length - 1].ToLower().CompareTo("generate") == 0)
                    {
                        if (args.Length <= 1)
                        {
                            Console.WriteLine("Usage: \nCreate lyric events in your midi formatted as \"COLOR <channel> <r> <g> <b> <a>\". \nChannel can be any integer value between 0~15 or 127 for all channels on the current track. \nThen execute this program with parameters \"<path/to/input.mid> generate\". ");
                            return;
                        }
                        MidiFile reader = new MidiFile(args[0]);
                        MidiWriter writer = new MidiWriter(args[0] + ".colored.mid");
                        writer.Init(reader.PPQ);
                        writer.WriteFormat(1);
                        foreach (IEnumerable<MIDIEvent> track in reader.IterateTracks())
                        {
                            List<MIDIEvent> newtrack = new List<MIDIEvent>();
                            foreach (MIDIEvent midievent in track)
                            {
                                if (midievent is TextEvent && ((TextEvent)midievent).Type == 5)
                                {
                                    byte[] bytes = ((TextEvent)midievent).Bytes;
                                    string text = Encoding.ASCII.GetString(bytes, 0, bytes.Length);
                                    if (!text.StartsWith("COLOR"))
                                    {
                                        continue;
                                    }
                                    string[] splitted = text.Split(' ');
                                    if (splitted.Length != 6)
                                    {
                                        Console.WriteLine("The lyric \"" + text + "\" is not correctly formatted. It will be ignored. ");
                                        continue;
                                    }
                                    sbyte channel = sbyte.Parse(splitted[1]);
                                    byte R = byte.Parse(splitted[2]);
                                    byte G = byte.Parse(splitted[3]);
                                    byte B = byte.Parse(splitted[4]);
                                    byte A = byte.Parse(splitted[5]);
                                    if (channel != 127 && (channel < 0 || channel > 15))
                                    {
                                        Console.WriteLine("The lyric \"" + text + "\" specified an invalid channel. It will be ignored. ");
                                        continue;
                                    }
                                    newtrack.Add(new ColorEvent(midievent.DeltaTime, (byte)channel, R, G, B, A));
                                    Console.WriteLine("Created color event from lyric: \"" + text + "\"");
                                }
                                else
                                {
                                    newtrack.Add(midievent);
                                }
                            }
                            writer.WriteTrack(newtrack);
                        }
                        writer.Close();
                        Console.WriteLine("Job done! ");
                        return;
                    }
                    else if (args[args.Length - 1].ToLower().CompareTo("restore") == 0)
                    {
                        if (args.Length <= 1)
                        {
                            Console.WriteLine("Usage: \nExecute this program with parameters \"<path/to/input.mid> restore\". \nThen all color events in your midi will be replaced with lyric events that you can easily edit. ");
                            return;
                        }
                        MidiFile reader = new MidiFile(args[0]);
                        MidiWriter writer = new MidiWriter(args[0] + ".uncolored.mid");
                        writer.Init(reader.PPQ);
                        writer.WriteFormat(1);
                        foreach (IEnumerable<MIDIEvent> track in reader.IterateTracks())
                        {
                            List<MIDIEvent> newtrack = new List<MIDIEvent>();
                            foreach (MIDIEvent midievent in track)
                            {
                                if (midievent is ColorEvent)
                                {
                                    newtrack.Add(new TextEvent(midievent.DeltaTime, 5, Encoding.ASCII.GetBytes("COLOR " + ((ColorEvent)midievent).Channel + " " + ((ColorEvent)midievent).R + " " + ((ColorEvent)midievent).G + " " + ((ColorEvent)midievent).B + " " + ((ColorEvent)midievent).A)));
                                    Console.WriteLine("Created lyric from color event: \"" + midievent.ToString() + "\"");
                                }
                                else
                                {
                                    newtrack.Add(midievent);
                                }
                            }
                            writer.WriteTrack(newtrack);
                        }
                        writer.Close();
                        Console.WriteLine("Job done! ");
                        return;
                    }
                    else if (args[args.Length - 1].ToLower().CompareTo("gradient") == 0)
                    {
                        if (args.Length <= 1)
                        {
                            Console.WriteLine("Usage: \nExecute this program with parameters \"<channel> <r1> <g1> <b1> <a1> <r2> <g2> <b2> <a2> <duration> <spacing> <path/to/output.mid> gradient\". \nThen multiple lyric events formatted as \"COLOR <channel> <r> <g> <b> <a>\" will be generated in the midi. ");
                            return;
                        }
                        MidiWriter writer = new MidiWriter(args[11]);
                        writer.Init(128);
                        writer.WriteFormat(1);
                        writer.WriteTrack(new List<MIDIEvent>()); //Conductor
                        List<MIDIEvent> track = new List<MIDIEvent>();
                        sbyte channel = sbyte.Parse(args[0]);
                        byte[] start = { byte.Parse(args[1]), byte.Parse(args[2]), byte.Parse(args[3]), byte.Parse(args[4]) };
                        byte[] end = { byte.Parse(args[5]), byte.Parse(args[6]), byte.Parse(args[7]), byte.Parse(args[8]) };
                        ulong duration = ulong.Parse(args[9]);
                        ulong spacing = ulong.Parse(args[10]);
                        if (channel != 127 && (channel < 0 || channel > 15))
                        {
                            Console.WriteLine("An invalid channel has been specified. The generation will be aborted. ");
                            return;
                        }
                        if (spacing <= 0 || spacing > duration)
                        {
                            Console.WriteLine("An invalid sapcing has been specified. The generation will be aborted. ");
                            return;
                        }
                        for (ulong tick = 0; tick < duration-spacing; tick++)
                        {
                            float percent = ((float)(tick) / (float)(duration));
                            byte R = (byte)(start[0] + percent * (end[0] - start[0]));
                            byte G = (byte)(start[1] + percent * (end[1] - start[1]));
                            byte B = (byte)(start[2] + percent * (end[2] - start[2]));
                            byte A = (byte)(start[3] + percent * (end[3] - start[3]));
                            track.Add(new TextEvent(spacing, 5, Encoding.ASCII.GetBytes("COLOR " + channel + " " + R + " " + G + " " + B + " " + A)));
                            Console.WriteLine("Created lyric: \"COLOR " + channel + " " + R + " " + G + " " + B + " " + A + "\"");
                        }
                        track.Add(new TextEvent(spacing, 5, Encoding.ASCII.GetBytes("COLOR " + channel + " " + end[0] + " " + end[1] + " " + end[2] + " " + end[3])));
                        Console.WriteLine("Created lyric: \"COLOR " + channel + " " + end[0] + " " + end[1] + " " + end[2] + " " + end[3] + "\"");
                        writer.WriteTrack(track);
                        writer.Close();
                        Console.WriteLine("Job done! ");
                        return;
                    }
                    else if (args[args.Length - 1].ToLower().CompareTo("config") == 0)
                    {
                        if (args.Length <= 1)
                        {
                            Console.WriteLine("Usage: \nExecute this program with parameters \"<path/to/output.mid> track config\". \nThen multiple lyric events formatted as \"COLOR <channel> <r> <g> <b> <a>\" will be generated base on PFA config for each track in the midi. \nAlternatively execute this program with parameters \"<path/to/output.mid> channel config\". \nThen multiple lyric events formatted as \"COLOR <channel> <r> <g> <b> <a>\" will be generated base on PFA config for each channel in the midi. ");
                            return;
                        }
                        if (args[1].ToLower().CompareTo("track") == 0)
                        {
                            XmlDocument config = new XmlDocument();
                            config.Load(System.IO.Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "Piano From Above", "Config.xml"));
                            XmlNodeList colorlist = config.SelectNodes("//Color");
                            MidiWriter writer = new MidiWriter(args[0]);
                            writer.Init(128);
                            writer.WriteFormat(1);
                            writer.WriteTrack(new List<MIDIEvent>()); //Conductor
                            foreach (XmlNode node in colorlist)
                            {
                                List<MIDIEvent> track = new List<MIDIEvent>();
                                byte R = byte.Parse(node.Attributes["R"].Value);
                                byte G = byte.Parse(node.Attributes["G"].Value);
                                byte B = byte.Parse(node.Attributes["B"].Value);
                                byte A = (byte)(byte.MaxValue - byte.Parse(node.Attributes["A"].Value)); // Alpha in PFA is reversed! 
                                track.Add(new TextEvent(0, 5, Encoding.ASCII.GetBytes("COLOR 127 " + R + " " + G + " " + B + " " + A)));
                                Console.WriteLine("Created lyric: \"COLOR 127 " + R + " " + G + " " + B + " " + A + "\"");
                                writer.WriteTrack(track);
                            }
                            writer.Close();
                            Console.WriteLine("Job done! ");
                            return;
                        }
                        else if (args[1].ToLower().CompareTo("channel") == 0)
                        {
                            XmlDocument config = new XmlDocument();
                            config.Load(System.IO.Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "Piano From Above", "Config.xml"));
                            XmlNodeList colorlist = config.SelectNodes("//Color");
                            MidiWriter writer = new MidiWriter(args[0]);
                            writer.Init(128);
                            writer.WriteFormat(1);
                            writer.WriteTrack(new List<MIDIEvent>()); //Conductor
                            List<MIDIEvent> track = new List<MIDIEvent>();
                            byte channel = 0;
                            foreach (XmlNode node in colorlist)
                            {
                                byte R = byte.Parse(node.Attributes["R"].Value);
                                byte G = byte.Parse(node.Attributes["G"].Value);
                                byte B = byte.Parse(node.Attributes["B"].Value);
                                byte A = (byte)(byte.MaxValue - byte.Parse(node.Attributes["A"].Value)); // Alpha in PFA is reversed! 
                                track.Add(new TextEvent(0, 5, Encoding.ASCII.GetBytes("COLOR " + channel + " " + R + " " + G + " " + B + " " + A)));
                                Console.WriteLine("Created lyric: \"COLOR " + channel + " " + R + " " + G + " " + B + " " + A + "\"");
                                channel++;
                                if (channel > 15) channel = 0; //Shouldn't happen in theory but what if it does? 
                            }
                            writer.WriteTrack(track);
                            writer.Close();
                            Console.WriteLine("Job done! ");
                            return;
                        }
                        else
                        {
                            Console.WriteLine("Unacceptable input: " + args[1]);
                        }
                    }
                    else
                    {
                        Console.WriteLine("Unacceptable input: " + args[0]);
                    }
                } else
                {
                    Console.WriteLine("Parameters needed! \n\nSupported options: \ngenerate - Generate color events based on lyric events\nrestore - Restore generated color events back to lyric events\ngradient - Make a gradient for specified amount of ticks\nconfig - Generate color events base on PFA config");
                }
                return;
            }
            catch (Exception e)
            {
                Console.WriteLine("Processing failed! \nReason: \n" + e);
            }
        }
    }
}

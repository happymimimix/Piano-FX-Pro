using System;
using System.Collections.Generic;
using System.Collections.Concurrent;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace MIDIModificationFramework
{
    public static class ThreadedSequenceFunctions
    {
        class DisposeAction : IDisposable
        {
            Action disposeAction;

            public DisposeAction(Action action)
            {
                disposeAction = action;
            }

            public void Dispose()
            {
                disposeAction();
            }
        }

        public static IEnumerable<T> ConstantThreadedBuffer<T>(this IEnumerable<T> seq, int maxSize)
        {
            BlockingCollection<T> buffer = new BlockingCollection<T>(maxSize);

            CancellationTokenSource cancel = new CancellationTokenSource();

            var runner = Task.Run(() =>
            {
                foreach (var t in seq)
                {
                    if (cancel.IsCancellationRequested) break;
                    buffer.Add(t);
                }
                buffer.CompleteAdding();
            });

            void cancelAndWait()
            {
                cancel.Cancel();
                runner.Wait();
            }

            using (new DisposeAction(cancelAndWait))
            {
                foreach (var t in buffer.GetConsumingEnumerable())
                {
                    yield return t;
                }
            }
        }
        public static IEnumerable<T> ConstantThreadedBuffer<T>(this IEnumerable<T> seq, int maxBatches, int batchSize)
        {
            BatchBlockingCollection<T> buffer = new BatchBlockingCollection<T>(maxBatches, batchSize);

            CancellationTokenSource cancel = new CancellationTokenSource();

            var runner = Task.Run(() =>
            {
                foreach (var t in seq)
                {
                    if (cancel.IsCancellationRequested) break;
                    buffer.Add(t, cancel.Token);
                }
                buffer.Complete();
            });

            void cancelAndWait()
            {
                cancel.Cancel();
                runner.Wait();
            }

            using (new DisposeAction(cancelAndWait))
            {
                foreach (var t in buffer)
                {
                    yield return t;
                }
            }
        }

        public static IEnumerable<T> TaskedThreadedBuffer<T>(this IEnumerable<T> seq, int maxSize)
        {
            BlockingCollection<T> buffer = new BlockingCollection<T>();

            Task readerTask = null;

            bool completed = false;

            var en = seq.GetEnumerator();

            CancellationTokenSource cancel = new CancellationTokenSource();
           
            Exception e = null;

            Action runReader = () =>
            {
                if (readerTask != null) readerTask.Wait();
                if (completed) return;
                readerTask = Task.Run(() =>
                {
                    while (buffer.Count < maxSize)
                    {
                        if (cancel.IsCancellationRequested) break;
                        try
                        {
                            if (!en.MoveNext())
                            {
                                completed = true;
                                buffer.CompleteAdding();
                                return;
                            }
                            buffer.Add(en.Current, cancel.Token);
                        }
                        catch (Exception ex)
                        {
                            buffer.CompleteAdding();
                            cancel.Cancel();
                            e = ex;
                        }
                    }
                    readerTask = null;
                });
            };

            runReader();

            void cancelAndWait()
            {
                cancel.Cancel();
                buffer.CompleteAdding();
                readerTask?.Wait();
            }

            using (new DisposeAction(cancelAndWait))
            {
                foreach (var t in buffer.GetConsumingEnumerable())
                {
                    yield return t;
                    if (buffer.Count < maxSize / 2 || buffer.Count == 0)
                    {
                        if (!completed) runReader();
                    }
                }
                if (e != null) throw e;
            }
        }

        public static IEnumerable<T> TaskedThreadedBuffer<T>(this IEnumerable<T> seq, int maxBatches, int batchSize)
        {
            BatchBlockingCollection<T> buffer = new BatchBlockingCollection<T>(batchSize);

            Task readerTask = null;

            bool completed = false;

            var en = seq.GetEnumerator();

            CancellationTokenSource cancel = new CancellationTokenSource();

            Exception e = null;

            Action runReader = () =>
            {
                if (readerTask != null) readerTask.GetAwaiter().GetResult();
                if (completed) return;
                readerTask = Task.Run(() =>
                {
                    while (buffer.BatchCount < maxBatches)
                    {
                        if (cancel.IsCancellationRequested) break;
                        try
                        {
                            if (!en.MoveNext())
                            {
                                completed = true;
                                buffer.Complete();
                                return;
                            }
                            buffer.Add(en.Current, cancel.Token);
                        }
                        catch (Exception ex)
                        {
                            buffer.Complete();
                            cancel.Cancel();
                            e = ex;
                        }
                    }
                    readerTask = null;
                });
            };

            runReader();


            void cancelAndWait()
            {
                cancel.Cancel();
                buffer.Complete();
                readerTask?.Wait();
            }

            using (new DisposeAction(cancelAndWait))
            {
                foreach (var t in buffer)
                {
                    yield return t;
                    if (buffer.BatchCount < maxBatches / 2 || buffer.BatchCount == 0)
                    {
                        if (!completed) runReader();
                    }
                }
                if (e != null) throw e;
            }
        }
    }
}

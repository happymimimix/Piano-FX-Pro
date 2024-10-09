using System;
using System.Collections;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace MIDIModificationFramework
{
    public class BatchBlockingCollection<T> : IEnumerable<T>
    {
        BlockingCollection<FastList<T>> data;

        public int BatchCount => data.Count;

        public bool IsComplete { get; private set; }

        FastList<T> buffer = new FastList<T>();
        int currentBatchSize = 0;

        int batchSize = 1000;

        public BatchBlockingCollection() : this(1000)
        { }

        public BatchBlockingCollection(int batchSize) : this(10, batchSize)
        { }

        public BatchBlockingCollection(int maxBatches, int batchSize)
        {
            this.batchSize = batchSize;
            data = new BlockingCollection<FastList<T>>(maxBatches);
        }

        public void Add(T item, CancellationToken cancel)
        {
            if (IsComplete) 
                throw new ObjectDisposedException("Already completed adding");
            buffer.Add(item);
            currentBatchSize++;
            if (currentBatchSize >= batchSize)
            {
                data.Add(buffer, cancel);
                buffer = new FastList<T>();
                currentBatchSize = 0;
            }
        }

        public void Add(T item)
        {
            if (IsComplete) throw new ObjectDisposedException("Already completed adding");
            buffer.Add(item);
            currentBatchSize++;
            if (currentBatchSize >= batchSize)
            {
                data.Add(buffer);
                buffer = new FastList<T>();
                currentBatchSize = 0;
            }
        }

        public void Complete()
        {
            if(currentBatchSize > 0)
            {
                data.Add(buffer);
                buffer = new FastList<T>();
                currentBatchSize = 0;
            }
            data.CompleteAdding();
            IsComplete = true;
        }

        IEnumerable<T> Enumerate()
        {
            foreach (var b in data.GetConsumingEnumerable())
            {
                foreach (T e in b)
                {
                    yield return e;
                }
            }
        }

        public IEnumerator<T> GetEnumerator()
        {
            return Enumerate().GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return Enumerate().GetEnumerator();
        }
    }
}

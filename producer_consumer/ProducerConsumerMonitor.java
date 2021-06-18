package producer_consumer;

public class ProducerConsumerMonitor {
    static final int N = 100; // constant giving the buffer size
    static Producer producer = new Producer(); // instantiate a new producer thread
    static Consumer consumer = new Consumer(); // instantiate a new consumer thread
    static Monitor monitor = new Monitor(); // instantiate a new monitor

    public static void main(String[] args) {
        producer.start(); // start the producer thread
        consumer.start(); // start the consumer thread
    }

    static class Producer extends Thread {
        private int i = 1;

        /**
         * run method contains the thread code
         */
        @Override
        public void run() {
            int item;
            // producer loop
            while (true) {
                item = produceItem();
                monitor.insert(item);
            }
        }

        /**
         * actually produce
         * 
         * @return item which is an integer
         */
        private int produceItem() {
            int item = i;
            i++;
            System.out.println("produce: " + item);
            return item;
        }
    }

    static class Consumer extends Thread {
        /**
         * run method contains the thread code
         */
        @Override
        public void run() {
            int item;
            // consumer loop
            while (true) {
                item = monitor.remove();
                consumeItem(item);
            }
        }

        /**
         * actually consume
         * 
         * @param item an integer
         */
        private void consumeItem(int item) {
            System.out.println("consume: " + item);
        }
    }

    /**
     * this is a monitor
     */
    static class Monitor {
        private int[] buffer = new int[N];
        private int count = 0; // counter
        private int lo = 0; // index
        private int hi = 0; // index

        public synchronized void insert(int val) {
            // if the buffer is full, go to sleep
            if (count == N) {
                goToSleep();
            }
            buffer[hi] = val; // insert an item into the buffer
            hi = (hi + 1) % N; // slot to place next item in
            count = count + 1; // one more item in the buffer now
            // if consumer was sleeping, wake it up
            if (count == 1) {
                notify();
            }
        }

        public synchronized int remove() {
            int val;
            // if the buffer is empty, go to sleep
            if (count == 0) {
                goToSleep();
            }
            val = buffer[lo]; // fetch an item from the buffer
            lo = (lo + 1) % N; // slot to fetch next item from
            count = count - 1; // one few items in the buffer
            // if producer was sleeping, wake it up
            if (count == N - 1) {
                notify();
            }
            return val;
        }

        private void goToSleep() {
            try {
                wait();
            } catch (InterruptedException exc) {
                System.out.println(exc);
            }
        }
    }

}
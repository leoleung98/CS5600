import java.util.Random;

public class BoundedBufferTest {
    public static void main(String[] args) {
        BoundedBuffer buffer = new BoundedBuffer(10);
        Thread[] threads = new Thread[3];

        for (int i = 0; i < 3; i++) {
            threads[i] = new Thread(new ProducerConsumer(buffer), "Thread " + (i + 1));
            threads[i].start();
        }

        try {
            for (int i = 0; i < 3; i++) {
                threads[i].join();
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        // print any remaining strings in the buffer
        System.out.println("\nStrings left in buffer: ");
        int stringRemain = 0;
        while (buffer.getCount() > 0) {
            try {
                if (buffer.fetch() == null) {
                    stringRemain++;
                }
                System.out.println(buffer.fetch() + " left in buffer");
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        if (stringRemain == 0) {
            System.out.println("String buffer is empty.");
        }

        System.out.println("\nAll threads completed.");
    }
}

class ProducerConsumer implements Runnable {
    private final BoundedBuffer buffer;
    private final Random rand = new Random();

    public ProducerConsumer(BoundedBuffer buffer) {
        this.buffer = buffer;
    }

    public void run() {
        for (int i = 0; i < 30; i++) {

            String data = Thread.currentThread().getId() + "-" + String.format("%05d", rand.nextInt(100000));

            try {
                if (i % 2 == 0) {
                    buffer.deposit(data);
                    System.out.println("Thread " + Thread.currentThread().getId() + " deposited " + data);
                } else {
                    String result = buffer.fetch();
                    System.out.println("Thread " + Thread.currentThread().getId() + " fetched " + result);
                }

                Thread.sleep(500); // simulate data production/consumption
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

public class Main {
    // SINKRONISASI 1: Objek pengunci (lock/mutex) untuk mencegah race condition pada konsol
    private static final Object lock = new Object();

    // Thread 1: Menghitung Faktorial
    static class FactorialTask implements Runnable {
        @Override
        public void run() {
            int n = 5;
            long factorial = 1;
            for (int i = 1; i <= n; i++) {
                factorial *= i;
            }
            // SINKRONISASI (Mutual Exclusion):
            // Memastikan hanya 1 thread yang mencetak ke System.out dalam satu waktu
            // agar output teks di konsol (shared resource) tidak saling bertumpuk.
            synchronized (lock) {
                System.out.println("[Thread 1 - Java] Faktorial dari " + n + " adalah: " + factorial);
            }
        }
    }

    // Thread 2: Menampilkan Deret Fibonacci
    static class FibonacciTask implements Runnable {
        @Override
        public void run() {
            int n = 7;
            long[] fib = new long[n];
            if (n > 0) fib[0] = 0;
            if (n > 1) fib[1] = 1;
            for (int i = 2; i < n; i++) {
                fib[i] = fib[i - 1] + fib[i - 2];
            }
            // SINKRONISASI (Mutual Exclusion): Mengunci akses konsol saat mencetak deret
            synchronized (lock) {
                System.out.print("[Thread 2 - Java] Deret Fibonacci (" + n + " angka): ");
                for (long val : fib) {
                    System.out.print(val + " ");
                }
                System.out.println();
            }
        }
    }

    // Thread 3: Membaca File Teks
    static class ReadFileTask implements Runnable {
        @Override
        public void run() {
            try (BufferedReader reader = new BufferedReader(new FileReader("sample.txt"))) {
                String line;
                // SINKRONISASI (Mutual Exclusion): Mengunci konsol saat membaca & mencetak isi file
                synchronized (lock) {
                    System.out.println("[Thread 3 - Java] Isi file sample.txt:");
                    while ((line = reader.readLine()) != null) {
                        System.out.println("   -> " + line);
                    }
                }
            } catch (IOException e) {
                // SINKRONISASI (Mutual Exclusion): Mengunci konsol saat mencetak pesan error
                synchronized (lock) {
                    System.out.println("[Thread 3 - Java] Gagal membaca file: " + e.getMessage());
                }
            }
        }
    }

    public static void main(String[] args) {
        Thread t1 = new Thread(new FactorialTask());
        Thread t2 = new Thread(new FibonacciTask());
        Thread t3 = new Thread(new ReadFileTask());

        try {
            // SINKRONISASI 2 (Execution Ordering):
            // .start() memicu thread, lalu .join() menahan main thread sampai thread selesai.
            // Memastikan urutan eksekusi berjalan rapi bertahap (Thread 1 -> Thread 2 -> Thread 3).
            t1.start();
            t1.join(); // Menunggu Thread 1 selesai

            t2.start();
            t2.join(); // Menunggu Thread 2 selesai

            t3.start();
            t3.join(); // Menunggu Thread 3 selesai
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// SINKRONISASI 1: Mutex (CRITICAL_SECTION) untuk Windows API
// Berfungsi mengunci shared resource (layar konsol) agar tidak terjadi race condition
CRITICAL_SECTION log_mutex;

// Thread 1: Menghitung Faktorial
DWORD WINAPI task_factorial(LPVOID arg) {
    int n = 5;
    long long factorial = 1;
    for (int i = 1; i <= n; i++) {
        factorial *= i;
    }

    // SINKRONISASI (Mutual Exclusion):
    // Enter & Leave Critical Section memastikan hanya 1 thread yang boleh printf dalam satu waktu
    EnterCriticalSection(&log_mutex); // Mengunci akses konsol
    printf("[Thread 1 - C] Faktorial dari %d adalah: %lld\n", n, factorial);
    LeaveCriticalSection(&log_mutex); // Membuka kunci akses

    return 0;
}

// Thread 2: Menampilkan Deret Fibonacci
DWORD WINAPI task_fibonacci(LPVOID arg) {
    int n = 7;
    long long fib[7];
    if (n > 0) fib[0] = 0;
    if (n > 1) fib[1] = 1;

    for (int i = 2; i < n; i++) {
        fib[i] = fib[i - 1] + fib[i - 2];
    }

    // SINKRONISASI (Mutual Exclusion): Mengunci konsol saat mencetak deret fibonacci
    EnterCriticalSection(&log_mutex);
    printf("[Thread 2 - C] Deret Fibonacci (%d angka): ", n);
    for (int i = 0; i < n; i++) {
        printf("%lld ", fib[i]);
    }
    printf("\n");
    LeaveCriticalSection(&log_mutex);

    return 0;
}

// Thread 3: Membaca File Teks
DWORD WINAPI task_read_file(LPVOID arg) {
    FILE *file = fopen("sample.txt", "r");
    char buffer[255];

    if (file == NULL) {
        // SINKRONISASI (Mutual Exclusion): Mengunci konsol saat mencetak pesan error
        EnterCriticalSection(&log_mutex);
        printf("[Thread 3 - C] Gagal membuka file sample.txt\n");
        LeaveCriticalSection(&log_mutex);
        return 0;
    }

    // SINKRONISASI (Mutual Exclusion): Mengunci konsol saat membaca & mencetak isi file
    EnterCriticalSection(&log_mutex);
    printf("[Thread 3 - C] Isi file sample.txt:\n   -> ");
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s", buffer);
    }
    printf("\n");
    LeaveCriticalSection(&log_mutex);

    fclose(file);
    return 0;
}

int main() {
    // Inisialisasi pengunci Critical Section
    InitializeCriticalSection(&log_mutex);

    HANDLE hThread1, hThread2, hThread3;

    // SINKRONISASI 2 (Execution Ordering):
    // WaitForSingleObject menahan main process sampai thread bersangkutan selesai,
    // sehingga menjamin urutan eksekusi bertahap (Thread 1 -> Thread 2 -> Thread 3).
    
    // Thread 1
    hThread1 = CreateThread(NULL, 0, task_factorial, NULL, 0, NULL);
    WaitForSingleObject(hThread1, INFINITE); // Menunggu Thread 1 selesai
    CloseHandle(hThread1);

    // Thread 2
    hThread2 = CreateThread(NULL, 0, task_fibonacci, NULL, 0, NULL);
    WaitForSingleObject(hThread2, INFINITE); // Menunggu Thread 2 selesai
    CloseHandle(hThread2);

    // Thread 3
    hThread3 = CreateThread(NULL, 0, task_read_file, NULL, 0, NULL);
    WaitForSingleObject(hThread3, INFINITE); // Menunggu Thread 3 selesai
    CloseHandle(hThread3);

    // Menghapus pengunci Critical Section dari memori
    DeleteCriticalSection(&log_mutex);
    return 0;
}
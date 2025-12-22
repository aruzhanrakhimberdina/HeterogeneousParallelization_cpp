#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>

#ifdef _OPENMP
#include <omp.h>
#endif

using namespace std;

int main() {

    // Установка локали для русского языка
    setlocale(LC_ALL, "Russian");

    // Задание размера массива
    // Массив содержит 1 000 000 целых чисел
    const int N = 1'000'000;

    // Динамическое выделение памяти под массив
    int* arr = new int[N];

    // Инициализация генератора псевдослучайных чисел
    srand(static_cast<unsigned>(time(nullptr)));

    // Заполнение массива случайными значениями
    for (int i = 0; i < N; ++i) {
        arr[i] = rand();
    }

    // 1) Последовательный поиск

    auto start_seq = chrono::high_resolution_clock::now();

    int min_seq = arr[0];
    int max_seq = arr[0];

    for (int i = 1; i < N; ++i) {
        if (arr[i] < min_seq) min_seq = arr[i];
        if (arr[i] > max_seq) max_seq = arr[i];
    }

    auto end_seq = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> dur_seq = end_seq - start_seq;
    
    // 2) Параллельный поиск

    int min_par = arr[0];
    int max_par = arr[0];

    auto start_par = chrono::high_resolution_clock::now();

#ifdef _OPENMP
#pragma omp parallel
    {
        int local_min = arr[0];
        int local_max = arr[0];

#pragma omp for
        for (int i = 1; i < N; ++i) {
            if (arr[i] < local_min) local_min = arr[i];
            if (arr[i] > local_max) local_max = arr[i];
        }

#pragma omp critical
        {
            if (local_min < min_par) min_par = local_min;
            if (local_max > max_par) max_par = local_max;
        }
    }
#else
    for (int i = 1; i < N; ++i) {
        if (arr[i] < min_par) min_par = arr[i];
        if (arr[i] > max_par) max_par = arr[i];
    }
#endif

    auto end_par = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> dur_par = end_par - start_par;

    // 3) Сравниваем результаты

    cout << "Последовательно: min = " << min_seq
        << ", max = " << max_seq
        << ", время = " << dur_seq.count() << " мс\n";

    cout << "Параллельно:     min = " << min_par
        << ", max = " << max_par
        << ", время = " << dur_par.count() << " мс\n";

    // Проверка корректности параллельного алгоритма:
    // сравниваются результаты последовательного и параллельного
    // вычислений минимального и максимального элементов массива
    if (min_seq != min_par || max_seq != max_par) {
        cout << "Результаты отличаются!\n";
    }
    else {
        cout << "Результаты совпадают. Параллельный алгоритм работает корректно.\n";
    }

    // Вычисление ускорения и эффективности
    if (dur_par.count() > 0.0) {
        double speedup = dur_seq.count() / dur_par.count();
        cout << "Ускорение (speedup): " << speedup << "x\n";

#ifdef _OPENMP
        int threads = omp_get_max_threads();
        cout << "Максимальное количество потоков OpenMP: " << threads << endl;
        cout << "Эффективность: " << (speedup / threads * 100.0) << "%\n";
#else
        cout << "OpenMP не доступен (программа собрана без поддержки OpenMP)\n";
#endif
    }

    // Освобождение динамически выделенной памяти
    delete[] arr;

    return 0;
}
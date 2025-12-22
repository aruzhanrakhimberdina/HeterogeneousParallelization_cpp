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
    // Массив содержит 5 000 000 целых чисел

    const int N = 5'000'000;

    // Динамическое выделение памяти под массив
    // Память выделяется во время выполнения программы
    int* arr = new int[N];

    // Инициализация генератора псевдослучайных чисел
    srand(static_cast<unsigned>(time(nullptr)));

    // Заполнение массива случайными целыми числами в диапазоне [1; 100]
    for (int i = 0; i < N; ++i) {
        arr[i] = rand() % 100 + 1;
    }

    // 1) Последовательное вычисление среднего

    // Фиксация начального момента времени
    // перед выполнением последовательного алгоритма
    auto start_seq = chrono::high_resolution_clock::now();

    // Последовательный подсчёт суммы элементов массива
    long long sum_seq = 0;
    for (int i = 0; i < N; ++i) {
        sum_seq += arr[i];
    }

    // Вычисление среднего значения
    // Приведение к типу double необходимо для корректного
    // получения дробного результата
    double avg_seq = static_cast<double>(sum_seq) / N;

    // Фиксация конечного момента времени и вычисление длительности
    auto end_seq = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> dur_seq = end_seq - start_seq;

    // 2) Параллельное вычисление среднего

    // Фиксация начального момента времени
    // перед выполнением параллельного алгоритма
    auto start_par = chrono::high_resolution_clock::now();

    // Переменная для суммирования в параллельной области
    // Используется тип long long для предотвращения переполнения
    long long sum_par = 0;

#ifdef _OPENMP
    // Параллельная обработка массива:
    // директива reduction(+:sum_par) создаёт локальные частичные суммы
    // для каждого потока и корректно объединяет их по завершении цикла
#pragma omp parallel for reduction(+: sum_par)
    for (int i = 0; i < N; ++i) {
        sum_par += arr[i];
    }
#else
    // Если поддержка OpenMP отсутствует, выполняем вычисления
    // последовательно (аналог параллельного блока)
    for (int i = 0; i < N; ++i) {
        sum_par += arr[i];
    }
#endif

    // Вычисление среднего значения по сумме, полученной параллельно
    double avg_par = static_cast<double>(sum_par) / N;

    // Фиксация конечного момента времени и вычисление длительности
    auto end_par = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> dur_par = end_par - start_par;

    // 3) Сравнение результатов

    cout << "Последовательно: average = " << avg_seq
        << ", время = " << dur_seq.count() << " мс\n";

    cout << "Параллельно:     average = " << avg_par
        << ", время = " << dur_par.count() << " мс\n";

    // Проверка корректности параллельного алгоритма:
    // сравниваются средние значения, полученные последовательным
    // и параллельным способом

    double diff = avg_seq - avg_par;
    if (diff < 0) diff = -diff;

    if (diff > 1e-9) {
        cout << "Средние значения отличаются!\n";
    }
    else {
        cout << "Результаты совпадают. Параллельный алгоритм работает корректно.\n";
    }

    // Вычисление ускорения и эффективности параллельного алгоритма
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

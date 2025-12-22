#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>

using namespace std;

int main() {

    // Задание размера массива
    // Массив состоит из 1 000 000 целых чисел
    const int N = 1'000'000;

    // Динамическое выделение памяти под массив
    // Память выделяется во время выполнения программы
    // с использованием оператора new[]
    int* arr = new int[N];

    // Инициализация генератора псевдослучайных чисел
    // В качестве начального значения используется текущее время
    srand(static_cast<unsigned>(time(nullptr)));

    // Заполнение массива случайными целыми значениями
    for (int i = 0; i < N; ++i) {
        arr[i] = rand();
    }

    // Последовательный алгоритм поиска минимального
    // и максимального элементов массива
 
    // Начальный момент времени перед выполнением алгоритма
    auto start = chrono::high_resolution_clock::now();

    int minVal = arr[0];
    int maxVal = arr[0];

    // Последовательный просмотр всех элементов массива
    // с обновлением текущих значений минимума и максимума
    for (int i = 1; i < N; ++i) {
        if (arr[i] < minVal) {
            minVal = arr[i];
        }
        if (arr[i] > maxVal) {
            maxVal = arr[i];
        }
    }

    // Конечный момент времени после выполнения алгоритма
    auto end = chrono::high_resolution_clock::now();

    // Вычисление времени выполнения алгоритма
    chrono::duration<double, milli> duration = end - start;

    // Вывод результатов вычислений и времени выполнения
    cout << "Minimum value: " << minVal << endl;
    cout << "Maximum value: " << maxVal << endl;
    cout << "Execution time: " << duration.count() << " ms" << endl;

    // Освобождение динамически выделенной памяти
    // Предотвращает утечку памяти после завершения программы
    delete[] arr;

    return 0;
}
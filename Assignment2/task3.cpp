#include <iostream>      // Подключаем библиотеку для ввода и вывода (cout, endl)
#include <vector>        // Подключаем контейнер vector для хранения массивов
#include <random>        // Подключаем генераторы случайных чисел
#include <chrono>        // Подключаем инструменты для измерения времени выполнения

#ifdef _OPENMP           // Проверяем, включена ли поддержка OpenMP
#include <omp.h>         // Подключаем заголовочный файл OpenMP
#endif

using namespace std;     // Используем стандартное пространство имён

int main() {             // Главная функция программы

    setlocale(LC_ALL, "Russian");   // Устанавливаем локаль для корректного отображения русского языка

    vector<int> sizes = { 1000, 10000 };   // Размеры массивов, на которых будем тестировать сортировку

    random_device rd;              // Источник начального случайного значения
    mt19937 gen(rd());             // Генератор псевдослучайных чисел 
    uniform_int_distribution<int> dist(0, 100000);  // Распределение для генерации чисел в диапазоне [0..100000]

    for (int n : sizes) {          // Проходим циклом по каждому размеру массива из списка sizes

        cout << "\nРазмер массива: " << n << endl;  // Выводим текущий размер массива

        vector<int> a(n);          // Создаём массив из n элементов

        for (int i = 0; i < n; ++i) {   // Заполняем массив случайными числами
            a[i] = dist(gen);      // Генерируем случайное число и записываем в массив
        }

        // Последовательная сортировка выбором

        vector<int> aSeq = a;      // Создаём копию массива для последовательной версии

        auto startSeq = chrono::high_resolution_clock::now();  // Фиксируем время начала последовательной сортировки

        for (int i = 0; i < n - 1; ++i) {   // Перебираем позиции, куда ставим минимальный элемент
            int minIdx = i;                 // Считаем текущий элемент минимальным
            for (int j = i + 1; j < n; ++j) {  // Ищем минимум в правой части массива
                if (aSeq[j] < aSeq[minIdx]) {  // Если нашли элемент меньше текущего минимума
                    minIdx = j;             // Обновляем индекс минимума
                }
            }
            swap(aSeq[i], aSeq[minIdx]);   // Меняем местами текущий элемент и найденный минимум
        }

        auto endSeq = chrono::high_resolution_clock::now();  // Фиксируем время окончания сортировки
        double timeSeq = chrono::duration<double, milli>(endSeq - startSeq).count();  // Вычисляем время в миллисекундах

        // Параллельная сортировка выбором

        vector<int> aPar = a;      // Создаём копию массива для параллельной версии

        auto startPar = chrono::high_resolution_clock::now();  // Фиксируем время начала параллельной сортировки

        for (int i = 0; i < n - 1; ++i) {   // Внешний цикл по позициям
            int globalMinIdx = i;           // Глобальный индекс минимума на этом шаге

#ifdef _OPENMP
#pragma omp parallel                      // Создаём параллельную область
            {
                int localMinIdx = globalMinIdx;   // Локальный минимум для каждого потока

#pragma omp for nowait                   // Делим цикл поиска минимума между потоками
                for (int j = i + 1; j < n; ++j) {  // Каждый поток ищет минимум в своей части массива
                    if (aPar[j] < aPar[localMinIdx]) { // Если найден меньший элемент
                        localMinIdx = j;  // Обновляем локальный минимум
                    }
                }

#pragma omp critical                    // Критическая секция для объединения локальных минимумов
                {
                    if (aPar[localMinIdx] < aPar[globalMinIdx]) { // Сравниваем локальный минимум с глобальным
                        globalMinIdx = localMinIdx;  // Обновляем глобальный минимум
                    }
                }
            }
#else
            for (int j = i + 1; j < n; ++j) {   // Если OpenMP нет, используем обычный поиск минимума
                if (aPar[j] < aPar[globalMinIdx]) {
                    globalMinIdx = j;
                }
            }
#endif

            swap(aPar[i], aPar[globalMinIdx]);   // Ставим минимальный элемент на позицию i
        }

        auto endPar = chrono::high_resolution_clock::now();  // Фиксируем время окончания сортировки
        double timePar = chrono::duration<double, milli>(endPar - startPar).count();  // Вычисляем время в миллисекундах

        // Результаты

        cout << "Последовательно: " << timeSeq << " ms" << endl;  // Вывод времени последовательной версии
        cout << "Параллельно:     " << timePar << " ms" << endl;  // Вывод времени параллельной версии

#ifdef _OPENMP
        cout << "Потоков OpenMP: " << omp_get_max_threads() << endl;  // Вывод максимального количества потоков
#endif
    }

    return 0;   // Завершаем программу
}

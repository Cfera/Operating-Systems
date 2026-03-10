#include "LR2.h"
#include <iostream>
#include <windows.h>
#include <vector>
#include <cassert>

using namespace std;

struct ThreadData {
    int* arr;
    int size;
    int min;
    int max;
    double average;
};

DWORD WINAPI min_max_thread(LPVOID arg) {
    ThreadData* data = (ThreadData*)arg;
    data->min = data->arr[0];
    data->max = data->arr[0];
    for (int i = 0; i < data->size; i++)
    {
        if (data->arr[i]< data->min){
            data->min = data->arr[i];
        }
        if (data->arr[i] > data->max){
            data->max = data->arr[i];
        }
        Sleep(7);
    }
    cout << "min: " << data->min << endl;
    cout << "max: " << data->max << endl;
    return 0;
}

DWORD WINAPI average_thread(LPVOID arg) {
    ThreadData* data = (ThreadData*)arg;
    long long sum = 0;
    for (int i = 0; i < data->size; i++)
    {
        sum += data->arr[i];
        Sleep(12);
    }
    data->average = (double) sum / data->size;
    cout << "Среднее арифметическое: " << data->average<< endl;
    return 0;
}

void test_min_max() {
    cout << "Тест 1: Поиск минимума и максимума" << endl;
    //Обычный массив
    ThreadData data1;
    int arr1[] = { 5, 2, 8, 1, 9, 3 };
    data1.arr = arr1;
    data1.size = 6;
    min_max_thread(&data1);
    assert(data1.min == 1);
    assert(data1.max == 9);
    cout << "  Тест 1.1 пройден: min=1, max=9" << endl;
    //Массив с отрицательными числами
    ThreadData data2;
    int arr2[] = { -5, -2, -8, -1, -9, -3 };
    data2.arr = arr2;
    data2.size = 6;
    min_max_thread(&data2);
    assert(data2.min == -9);
    assert(data2.max == -1);
    cout << "  Тест 1.2 пройден: min=-9, max=-1" << endl;
    //Массив с одним элементом
    ThreadData data3;
    int arr3[] = { 42 };
    data3.arr = arr3;
    data3.size = 1;

    min_max_thread(&data3);
    assert(data3.min == 42);
    assert(data3.max == 42);
    cout << "  Тест 1.3 пройден: min=42, max=42" << endl;
    //Массив с одинаковыми элементами
    ThreadData data4;
    int arr4[] = { 7, 7, 7, 7, 7 };
    data4.arr = arr4;
    data4.size = 5;
    min_max_thread(&data4);
    assert(data4.min == 7);
    assert(data4.max == 7);
    cout << "  Тест 1.4 пройден: min=7, max=7" << endl;
    cout << "Все тесты min_max пройдены успешно!\n" << endl;
}

void test_average() {
    cout << "Тест 2: Вычисление среднего арифметического" << endl;
    //Обычный массив
    ThreadData data1;
    int arr1[] = { 2, 4, 6, 8, 10 };
    data1.arr = arr1;
    data1.size = 5;

    average_thread(&data1);
    assert(abs(data1.average - 6.0) < 0.0001);
    cout << "   Тест 2.1 пройден: average=6.0" << endl;
    //Массив с отрицательными числами
    ThreadData data2;
    int arr2[] = { -1, -2, -3, -4, -5 };
    data2.arr = arr2;
    data2.size = 5;
    average_thread(&data2);
    assert(abs(data2.average + 3.0) < 0.0001);
    cout << "  Тест 2.2 пройден: average=-3.0" << endl;
    //Массив с одним элементом
    ThreadData data3;
    int arr3[] = { 100 };
    data3.arr = arr3;
    data3.size = 1;
    average_thread(&data3);
    assert(abs(data3.average - 100.0) < 0.0001);
    cout << "  Тест 2.3 пройден: average=100.0" << endl;
    // Тест 4: Массив с большими числами
    ThreadData data4;
    int arr4[] = { 1000000, 2000000, 3000000 };
    data4.arr = arr4;
    data4.size = 3;
    average_thread(&data4);
    assert(abs(data4.average - 2000000.0) < 0.0001);
    cout << "  Тест 2.4 пройден: average=2000000.0" << endl;
    cout << "Все тесты average пройдены успешно!\n" << endl;
}

void test_replacement() {
    cout << "Тест 3: Замена min и max на среднее арифметическое" << endl;
    //Замена в обычном массиве
    ThreadData data1;
    int arr1[] = { 5, 2, 8, 1, 9, 3 };
    data1.arr = arr1;
    data1.size = 6;
    min_max_thread(&data1);
    average_thread(&data1);
    int expected_arr1[] = { 5, 2, 8, (int)data1.average, (int)data1.average, 3 };
    for (int i = 0; i < data1.size; i++) {
        if (data1.arr[i] == data1.min || data1.arr[i] == data1.max) {
            data1.arr[i] = (int)data1.average;
        }
    }
    for (int i = 0; i < data1.size; i++) {
        assert(data1.arr[i] == expected_arr1[i]);
    }
    cout << "  Тест 3.1 пройден: замена выполнена корректно" << endl;
    //Все элементы одинаковые
    ThreadData data2;
    int arr2[] = { 7, 7, 7, 7, 7 };
    data2.arr = arr2;
    data2.size = 5;
    min_max_thread(&data2);
    average_thread(&data2);
    int expected_arr2[] = { 7, 7, 7, 7, 7 };
    for (int i = 0; i < data2.size; i++) {
        if (data2.arr[i] == data2.min || data2.arr[i] == data2.max) {
            data2.arr[i] = (int)data2.average;
        }
    }
    for (int i = 0; i < data2.size; i++) {
        assert(data2.arr[i] == expected_arr2[i]);
    }
    cout << "  Тест 3.2 пройден: замена выполнена корректно" << endl;
    cout << "Все тесты replacement пройдены успешно!\n" << endl;
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    cout << "Запуск unit-тестов:\n" << endl;
    test_min_max();
    test_average();
    test_replacement();
    ThreadData data;
    cout << "Введите размер массива: ";
    cin >> data.size;
    if (data.size <= 0) {
        cout << "Размер массива должен быть положительным!" << endl;
        return 1;
    }
    data.arr = new int[data.size];
    cout << "Введите элементы массива: " << endl;
    for (int i = 0; i < data.size; i++) {
        cout<< i+1<<" - ";
        cin >> data.arr[i];
    }
    cout << "Полученный массив: " << endl;
    for (int i = 0; i < data.size; i++) {
        cout << i + 1 << " - " << data.arr[i] << endl;
    }
    HANDLE hMinMax = CreateThread(NULL, 0, min_max_thread, &data, 0, NULL);
    HANDLE hAverage = CreateThread(NULL, 0, average_thread, &data, 0, NULL);
    if (hMinMax == NULL || hAverage == NULL) {
        cerr << "Ошибка создания потоков!" << endl;
        delete[] data.arr;
        return 1;
    }
    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);
    cout << "Минимальный элемент: " << data.min << endl;
    cout << "Максимальный элемент: " << data.max << endl;
    cout << "Среднее арифметическое: " << data.average << endl;
    for (int i = 0; i < data.size; i++) {
        if (data.arr[i] == data.min || data.arr[i] == data.max) {
            data.arr[i] = (int)data.average;
        }
    }
    cout << "\nМассив после замены min и max на среднее арифметическое значение: " << endl;
    for (int i = 0; i < data.size; i++) {
        cout << i+1 << " - " << data.arr[i] << endl;
    }
    CloseHandle(hMinMax);
    CloseHandle(hAverage);
    delete[] data.arr;
    return 0;
}
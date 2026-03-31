#include <windows.h>
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

struct ThreadData {
    int id;
    int size;
    int* array;
    HANDLE hStartEvent;
    HANDLE hStopEvent;
    HANDLE hCanContinueEvent;
    CRITICAL_SECTION* cs;
};

DWORD WINAPI marker(LPVOID arg) {
    ThreadData* data = (ThreadData*)arg;
    srand(data->id);
    vector<int> markedIndices;
    bool running = true;

    while (running) {
        WaitForSingleObject(data->hStartEvent, INFINITE);

        while (true) {
            int index = rand() % data->size;

            if (data->array[index] == 0) {
                Sleep(5);
                data->array[index] = data->id;
                markedIndices.push_back(index);
                Sleep(5);
            }
            else {
                EnterCriticalSection(data->cs);
                cout << "\n[Thread " << data->id << "] Помечено: " << markedIndices.size()
                    << ", Индекс занят: " << index << endl;
                LeaveCriticalSection(data->cs);

                SetEvent(data->hCanContinueEvent);
                HANDLE waitEvents[2] = { data->hStopEvent, data->hStartEvent };
                DWORD dwWait = WaitForMultipleObjects(2, waitEvents, FALSE, INFINITE);

                if (dwWait == WAIT_OBJECT_0) {
                    for (int idx : markedIndices) {
                        assert(data->array[idx] == data->id);
                        data->array[idx] = 0;
                    }
                    running = false;
                    break;
                }
                else {
                    continue;
                }
            }
        }
    }
    return 0;
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    int size, count;
    cout << "Введите размер массива: ";
    cin >> size;
    assert(size > 0);
    int* array = new int[size]();
    cout << "Введите количество потоков marker: ";
    cin >> count;
    assert(count > 0 && count <= 64);
    CRITICAL_SECTION cs;
    InitializeCriticalSection(&cs);
    vector<HANDLE> hThreads(count);
    vector<ThreadData*> tData(count);
    vector<bool> isTerminated(count, false);
    for (int i = 0; i < count; i++) {
        tData[i] = new ThreadData{ i + 1, size, array,
            CreateEvent(NULL, FALSE, FALSE, NULL),
            CreateEvent(NULL, FALSE, FALSE, NULL),
            CreateEvent(NULL, FALSE, FALSE, NULL),
            &cs };
        hThreads[i] = CreateThread(NULL, 0, marker, tData[i], 0, NULL);
        assert(hThreads[i] != NULL);
    }

    for (int i = 0; i < count; i++) SetEvent(tData[i]->hStartEvent);

    int activeThreads = count;
    while (activeThreads > 0) {
        vector<HANDLE> waitHandles;
        for (int i = 0; i < count; i++)
            if (!isTerminated[i]) waitHandles.push_back(tData[i]->hCanContinueEvent);
        WaitForMultipleObjects((DWORD)waitHandles.size(), waitHandles.data(), TRUE, INFINITE);
        cout << "\nМассив: ";
        for (int i = 0; i < size; i++) cout << array[i] << " ";
        cout << endl;
        int killNum;
        while (true) {
            cout << "Введите номер потока для завершения (1-" << count << "): ";
            cin >> killNum;
            if (killNum >= 1 && killNum <= count && !isTerminated[killNum - 1]) break;
        }
        SetEvent(tData[killNum - 1]->hStopEvent);
        WaitForSingleObject(hThreads[killNum - 1], INFINITE);
        isTerminated[killNum - 1] = true;
        activeThreads--;
        cout << "Массив после очистки потоком " << killNum << ": ";
        for (int i = 0; i < size; i++) {
            cout << array[i] << " ";
            assert(array[i] != killNum);
        }
        cout << endl;
        if (activeThreads > 0) {
            for (int i = 0; i < count; i++) {
                if (!isTerminated[i]) SetEvent(tData[i]->hStartEvent);
            }
        }
    }
    for (int i = 0; i < size; i++) assert(array[i] == 0);
    for (int i = 0; i < count; i++) {
        CloseHandle(hThreads[i]);
        CloseHandle(tData[i]->hStartEvent);
        CloseHandle(tData[i]->hStopEvent);
        CloseHandle(tData[i]->hCanContinueEvent);
        delete tData[i];
    }
    DeleteCriticalSection(&cs);
    delete[] array;
    return 0;
}
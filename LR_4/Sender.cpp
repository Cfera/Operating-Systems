#include <iostream>
#include <fstream>
#include <string>
#include "common.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;
    string fileName = argv[1];

    HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, (fileName + "_mutex").c_str());
    HANDLE semRead = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, (fileName + "_sem_read").c_str());
    HANDLE semWrite = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, (fileName + "_sem_write").c_str());
    HANDLE readyEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, (fileName + "_ready").c_str());

    SetEvent(readyEvent);

    while (true) {
        cout << "1. Send message\n2. Exit\nChoice: ";
        int choice;
        cin >> choice;
        if (choice == 2) break;

        cout << "Message (max 20 chars): ";
        string text;
        cin >> text;
        if (text.length() >= 20) text = text.substr(0, 19);

        WaitForSingleObject(semWrite, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        fstream file(fileName, ios::binary | ios::in | ios::out);
        ControlHeader h;
        file.read(reinterpret_cast<char*>(&h), sizeof(ControlHeader));

        Message msg;
        strcpy_s(msg.text, text.c_str());

        file.seekp(sizeof(ControlHeader) + h.tail * sizeof(Message), ios::beg);
        file.write(reinterpret_cast<char*>(&msg), sizeof(Message));

        h.tail = (h.tail + 1) % h.capacity;
        h.count++;

        file.seekp(0, ios::beg);
        file.write(reinterpret_cast<char*>(&h), sizeof(ControlHeader));
        file.close();

        ReleaseMutex(mutex);
        ReleaseSemaphore(semRead, 1, NULL);
    }

    return 0;
}
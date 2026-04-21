#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "common.h"

using namespace std;

int main() {
    string fileName;
    int recordCount, senderCount;

    cout << "Enter file name: ";
    cin >> fileName;
    cout << "Enter record count: ";
    cin >> recordCount;

    ofstream out(fileName+".bin", ios::binary | ios::trunc);
    ControlHeader header = { recordCount, 0, 0, 0 };
    out.write(reinterpret_cast<char*>(&header), sizeof(ControlHeader));
    Message emptyMsg = { "" };
    for (int i = 0; i < recordCount; ++i) {
        out.write(reinterpret_cast<char*>(&emptyMsg), sizeof(Message));
    }
    out.close();

    HANDLE mutex = CreateMutex(NULL, FALSE, (fileName + "_mutex").c_str());
    HANDLE semRead = CreateSemaphore(NULL, 0, recordCount, (fileName + "_sem_read").c_str());
    HANDLE semWrite = CreateSemaphore(NULL, recordCount, recordCount, (fileName + "_sem_write").c_str());
    HANDLE readyEvent = CreateEvent(NULL, TRUE, FALSE, (fileName + "_ready").c_str());

    cout << "Enter sender count: ";
    cin >> senderCount;

    vector<PROCESS_INFORMATION> pi(senderCount);
    for (int i = 0; i < senderCount; ++i) {
        STARTUPINFO si = { sizeof(si) };
        string cmd = "Sender.exe " + fileName;
        CreateProcess(NULL, &cmd[0], NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi[i]);
    }

    for (int i = 0; i < senderCount; ++i) {
        WaitForSingleObject(readyEvent, INFINITE);
    }

    while (true) {
        cout << "1. Read message\n2. Exit\nChoice: ";
        int choice;
        cin >> choice;
        if (choice == 2) break;

        WaitForSingleObject(semRead, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        fstream file(fileName, ios::binary | ios::in | ios::out);
        ControlHeader h;
        file.read(reinterpret_cast<char*>(&h), sizeof(ControlHeader));

        Message msg;
        file.seekg(sizeof(ControlHeader) + h.head * sizeof(Message), ios::beg);
        file.read(reinterpret_cast<char*>(&msg), sizeof(Message));

        h.head = (h.head + 1) % h.capacity;
        h.count--;

        file.seekp(0, ios::beg);
        file.write(reinterpret_cast<char*>(&h), sizeof(ControlHeader));
        file.close();

        cout << "Received: " << msg.text << endl;

        ReleaseMutex(mutex);
        ReleaseSemaphore(semWrite, 1, NULL);
    }

    for (int i = 0; i < senderCount; ++i) {
        TerminateProcess(pi[i].hProcess, 0);
        CloseHandle(pi[i].hProcess);
        CloseHandle(pi[i].hThread);
    }
    CloseHandle(mutex);
    CloseHandle(semRead);
    CloseHandle(semWrite);
    CloseHandle(readyEvent);

    return 0;
}
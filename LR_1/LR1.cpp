#include <iostream>
#include <string>
#include <Windows.h>
#include <fstream>
#include "employee.h"

using namespace std;

bool RunProcess(const string& kCmdLine) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(si);
    if (!CreateProcessA(
        NULL,
        const_cast<LPSTR>(kCmdLine.c_str()),
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi)){
        cerr << "Ошибка в запуске ["<< kCmdLine<< "], код="<< GetLastError()<< "\n";
        return false;
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return true;
}

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    string filename;
    int datacount;
    cout << "Введите имя создаваемого файла:\n";
    cin >> filename;
    filename += ".bin";
    cout << "Введите количество записей:\n";
    cin >> datacount;
    string creatorCmd = "Creator.exe " + filename + " " + to_string(datacount);
    if(!RunProcess(creatorCmd)) {
        cerr << "Не удалось запустить Creator" << endl;
        return 1;
    }
    ifstream readfile_bin(filename, ios::binary);
    employee ex;
    for (int i = 0; i < datacount; i++)
    {
        readfile_bin.read(reinterpret_cast<char*>(&ex), sizeof(ex));
        cout << "Запись №" << i + 1 << "\n";
        cout << ex.num << "\n";
        cout << ex.name << "\n";
        cout << ex.hours << "\n";
    }
    readfile_bin.close();
    string newfilename;
    int earn;
    cout << "Введите имя файла отчета:\n";
    cin >> newfilename;
    newfilename += ".txt";
    cout << "Введите оплату за час работы:\n";
    cin >> earn;
    string reporterCmd = "Reporter.exe " + filename+" " + newfilename + " " + to_string(earn);
    if (!RunProcess(reporterCmd)) {
        cerr << "Не удалось запустить Reporter" << endl;
        return 1;
    }
    ifstream readfile_txt(newfilename);
    string info;
    while (getline(readfile_txt, info)){
        cout << info << "\n";
    }
    readfile_txt.close();
    return 0;
}
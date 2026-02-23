#include <iostream>
#include <string>
#include <Windows.h>
#include <fstream>
#include "employee.h"

using namespace std;

int main(int argc, char* argv[])
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    string filename = argv[1];
    int datacount = atoi(argv[2]);
    if (datacount == 0)return 1;
    ofstream outfile(filename, ios::binary);
    if (!outfile.is_open()) {
        cerr << "Файл не создался"; return 1;
    }
    cout << "Введите данные в следующем порядке:\n\t1.Идентификационный номер сотрудника\n\t2.Имя сотрудника\n\t3.Количество отработанных часов\n";
    employee emploer;
    for (int i = 0; i < datacount; i++)
    {
        cout << "Запись №" << i + 1 << "\n";
        cin >> emploer.num;
        cin >> emploer.name;
        cin >> emploer.hours;
        outfile.write(reinterpret_cast<char*>(&emploer), sizeof(emploer));
    }
    outfile.close();
    return 0;
}
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
    string newfilename = argv[2];
    int earn = atoi(argv[3]);
    ifstream readfile(filename, ios::binary);
    if (!readfile.is_open()) {
        cerr << "Бинарный файл не открылся"; return 1;
    }
    ofstream infile(newfilename);
    if (!infile.is_open()) {
        cerr << "Отчет не открылся"; return 1;
    }
    int place = filename.find_last_of('.');
    filename = filename.substr(0, place);
    infile << "\t" << filename << "\n";
    employee emp;
    while (readfile.read(reinterpret_cast<char*>(&emp), sizeof(emp))) {
        infile << emp.num << " " << emp.name << " " << emp.hours << " " << emp.hours * earn << "\n";
    }
    infile.close();
    readfile.close();
    return 0;
}

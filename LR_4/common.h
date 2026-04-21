#pragma once
#include <windows.h>

struct Message {
    char text[20];
};

struct ControlHeader {
    int capacity;
    int head;
    int tail;
    int count;
};
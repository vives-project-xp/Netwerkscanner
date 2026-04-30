#pragma once
#include <stdint.h>

class TimeSync {
public:
    static void Init();
    static void Sync();
    static uint64_t Get();   // UNIX tijd (seconden)
    static bool IsValid();
};
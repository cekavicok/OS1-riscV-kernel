//
// Created by os on 6/23/26.
//

#ifndef PROJECT_BASE_KCONSOLE_H
#define PROJECT_BASE_KCONSOLE_H

#include "../lib/hw.h"

class Kbuffer;

class Kconsole {
public:
    static void init();
    static void putc(char c);
    static char getc();
    static void onRxAvailable();

    static void rawPutc(char c);
    static int rawGetc();

private:
    static bool txReady();
    static bool rxReady();
    static void writerThreadBody(void* arg);


    static Kbuffer* outputBuffer;
    static Kbuffer* inputBuffer;

};


#endif //PROJECT_BASE_KCONSOLE_H

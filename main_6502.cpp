#include<stdio.h>
#include<stdlib.h>

using Byte = unsigned char;
using Word = unsigned short;
using u32 = unsigned int;

struct Mem {
    static constexpr u32 MAX_MEM = 1024 * 64;
    Byte Data[MAX_MEM];
};

struct CPU {

    Word PC;        // program counter
    Word SP;        // stack pointer

    Byte A, X, Y;   // registers

    // process status flags
    Byte C : 1;
    Byte Z : 1;
    Byte I : 1;
    Byte D : 1;
    Byte B : 1;
    Byte V : 1;
    Byte N : 1;

    // reset cpu
    void Reset(){
        PC = 0xFFFC;
        SP = 0x0100;
        D = 0;

        C = Z = I = D = B = V = N = 0;
        A = X = Y = 0;
    }
};

int main(){

    CPU cpu;
    cpu.Reset();
    return 0;
}
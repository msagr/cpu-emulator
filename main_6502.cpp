#include<stdio.h>
#include<stdlib.h>

using Byte = unsigned char;
using Word = unsigned short;
using u32 = unsigned int;

struct Mem {
    static constexpr u32 MAX_MEM = 1024 * 64;
    Byte Data[MAX_MEM];

    // initializing memory
    void Initialise(){
        for(u32 i=0; i<MAX_MEM; i++){
            Data[i] = 0;
        }
    }

    // read 1 byte
    Byte operator[] (u32 Address) const {

        // assert here Address is < MAX_MEM
        return Data[Address];
    }

    // write 1 byte
    Byte& operator[] (u32 Address) { // writing Byte& operator[] (u32 Address) const is giving error, why?
        return Data[Address];
    }
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
    void Reset(Mem& memory){
        PC = 0xFFFC;
        SP = 0x0100;

        C = Z = I = D = B = V = N = 0;
        A = X = Y = 0;

        // initialize memory
        memory.Initialise();
    }

    Byte FetchByte(u32& cycle, Mem& memory){

        Byte Data = memory[PC];
        PC++;
        cycle--;
        return Data;
    }

    // opcode
    static constexpr Byte INS_LDA_IM = 0xA9;

    // to execute ins.
    void Execute(u32 cycle,  Mem& memory){

        while(cycle > 0){
            Byte Ins = FetchByte(cycle, memory);

            switch(Ins){
                case INS_LDA_IM:
                {
                    // fetching value which is 2nd byte
                    Byte value = FetchByte(cycle, memory);
                    
                    A = value;
                    Z = (A == 0);
                    N = (A & 0b10000000) > 0;

                } break;
                default:
                {
                    printf("Instruction not handled %d", Ins);
                } break;
            }
        }
    }
};

int main(){

    Mem mem;
    CPU cpu;
    cpu.Reset(mem);

    // start - inline a little program
    mem[0xFFFC] = CPU::INS_LDA_IM; // putting stuff in memory
    mem[0xFFFD] = 0x42; 


    // execute instruction
    cpu.Execute(2, mem); // Execute(clock_cycle, memory)
    return 0;
}
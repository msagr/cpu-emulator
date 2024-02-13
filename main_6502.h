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

    // writing 2 bytes
    void writeWord(Word Value, u32 Address, u32& cycle)
    {
        Data[Address] = Value & 0xFF;
        Data[Address+1] = (Value >> 8);
        cycle -= 2;
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

    Word FetchWord(u32& cycle, Mem& memory){

        // 6502 is little endian
        Word Data = memory[PC];
        PC++;

        Data |= (memory[PC] << 8);
        PC++;
        
        cycle -= 2;

        // if you want to handle endianness you would have to
        // swap bytes here.
        // if(PLATFORM_BIG_ENDIAN)
        // SwapBytesInWord(Data)

        return Data;
    }

    Byte ReadByte(u32& cycle, Byte& Address, Mem& memory){
        
        Byte Data = memory[Address];
        cycle--;
        return Data;
    }

    // opcode
    static constexpr Byte INS_LDA_IM = 0xA9;
    static constexpr Byte INS_LDA_ZP = 0xA5; // Zero page accumulator
    static constexpr Byte INS_LDA_ZPX = 0xB5;
    static constexpr Byte INS_JSR = 0x20; // JUMP ins.

    void LDASetStatus(){

        Z = (A == 0); // zero flag
        N = (A & 0b10000000) > 0; // negative flag
    }

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
                    LDASetStatus();

                } break;
                case INS_LDA_ZP:
                {
                    Byte ZeroPageAddr = FetchByte(cycle, memory);
                    
                    A = ReadByte(cycle, ZeroPageAddr, memory);
                    LDASetStatus();

                } break;
                case INS_LDA_ZPX:
                {
                    Byte ZeroPageAddr = FetchByte(cycle, memory);

                    ZeroPageAddr += X;
                    cycle--;
                    A = ReadByte(cycle, ZeroPageAddr, memory);
                    LDASetStatus();

                } break;
                case INS_JSR:
                {
                    Word SubAddr = FetchWord(cycle, memory);
                    memory[SP] = PC - 1;
                    memory.writeWord(PC - 1, SP, cycle);

                    PC = SubAddr;
                    cycle--;

                } break;
                default:
                {
                    printf("Instruction not handled %d", Ins);

                } break;
            }
        }
    }
};
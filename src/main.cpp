#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fileioc.h>
#include <ti/getcsc.h>

    using Byte = unsigned char;  //8bit
    using Word = unsigned short; //16bit
    using DWord = unsigned long; //32bit

    using u32 = unsigned short int;
    using u16 = Word;
struct Memory{
    static constexpr uint8_t MAX_MEM = uint8_t(65536); //64k

    uint8_t *MB1Ptr;
    uint8_t *MB2Ptr;

    ti_var_t MemB1;
    ti_var_t MemB2;

    void Init(){
        
        MemB1 = ti_Open("MB1", "r+");
        MemB2 = ti_Open("MB2", "r+");

        ti_Resize(MemB1, (MAX_MEM/2));
        ti_Resize(MemB2, (MAX_MEM/2));

        MB1Ptr = static_cast<uint8_t*>(ti_GetDataPtr(MemB1));
        MB2Ptr = static_cast<uint8_t*>(ti_GetDataPtr(MemB2));

        memset(MB1Ptr, 0, 32768);
        memset(MB2Ptr, 0, 32768);
        

        //ti_SetArchiveStatus(true, MemB1);
        //ti_SetArchiveStatus(true, MemB2);

        //ti_Close(MemB1);
        //ti_Close(MemB2);
    }

    //Byte operator[](u32 Address) const{
        //return Data[Address];
    //}

};

struct CPU{

    DWord PC; //Program Counter
    DWord SP; //Stack Pointer

    DWord D0,D1,D2,D3,D4,D5,D6,D7; //Data Registers
    DWord A0,A1,A2,A3,A4,A5,A6;    //Address Registers

    DWord SSP; //Supivisor Stack Pointer

    Byte C : 1; //User Status Flags
    Byte V : 1; //User Status Flags
    Byte Z : 1; //User Status Flags
    Byte N : 1; //User Status Flags
    Byte X : 1; //User Status Flags

    Byte I0 : 1; // System Status Flags
    Byte I1 : 1; // System Status Flags
    Byte I2 : 1; // System Status Flags
    Byte S  : 1; // System Status Flags
    Byte T  : 1; // System Status Flags

    void Reset(Memory mem){

        PC = 0x00;

        SSP = ti_Read((mem.MB1Ptr+PC), 1, 1, mem.MemB1);  //Load Contents of SSP with data at address $00

        PC = 0x00004;  //Sets program counter to address $04

        I0 = 1;       //Load interrupt registers with value of $07
        I1 = 1;
        I2 = 1;

        SP = 0x00100000;   //Initialize Stack Pointer to #$00100000

        mem.Init();
    }

    Byte FetchByte(int& Cycles, Memory mem){
            Byte Data = ti_Read((mem.MB1Ptr+PC), 1, 1, mem.MemB1);
            PC++;
            Cycles--;
            return Data;
    }

    void Execute(int Cycles, Memory mem){
        while(Cycles > 0){
            Byte Instruction = FetchByte(Cycles, mem);
        }
    }


};


int main(void){
    Memory mem;
    CPU cpu;

    cpu.Reset(mem);

    cpu.Execute(2, mem);

    while (!os_GetCSC());

    return 0;
}

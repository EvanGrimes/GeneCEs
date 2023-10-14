#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fileioc.h>
#include <ti/getcsc.h>
#include <debug.h>

    using Byte = unsigned char;  //8bit
    using Word = unsigned short; //16bit
    using DWord = unsigned long; //32bit

    using u32 = unsigned short;
    using u16 = unsigned long;

    // OPCODES
    static constexpr Word 
        INS_MOVEB = 0x157c;


struct Memory{
    static constexpr u16 MAX_MEM = size_t(65536); //64k

    static inline Byte Data[MAX_MEM]; //64k block of RAM

    void Init(){
        for(u16 i = 0; i < MAX_MEM; i++){  //Sets all of mem to 0x00
            Data[i] = 0;
        }
    }

    Byte operator[](u32 Address) const{   // Allows pointers
        return Data[Address];
    }

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

        SSP = mem.Data[PC];  //Load Contents of SSP with data at address $00

        PC = 0x00004;  //Sets program counter to address $04

        I0 = 1;       //Load interrupt registers with value of #$07
        I1 = 1;
        I2 = 1;

        SP = 0x00100000;   //Initialize Stack Pointer to #$00100000

        D0 = 0;
        D1 = 0;
        D2 = 0;
        D3 = 0;
        D4 = 0;
        D5 = 0;
        D6 = 0;
        D7 = 0;

        mem.Init();
    }


    Word FetchWord(int& Cycles, Memory mem){
            Word DP1 = mem.Data[PC];    //Stores half of Word
            dbg_printf("D1 = %x \n", DP1);
            dbg_printf("D1 S= %x \n", mem.Data[PC]);
            PC++;                    // Increases Program Counter

            Byte DP2 = mem.Data[PC];    //Stores other half of Word
            dbg_printf("D2 = %x \n", DP2);
            dbg_printf("D2 S= %x \n", mem.Data[PC]);

            DP1 = (DP1 << 8); //Moves first half of word over 8 bits


            Word Data = (DP1 + DP2);   //Adds two halfs of word together
            dbg_printf("Data Word = %x \n \n", Data);

            PC++;                    // Increases Program Counter
            Cycles--;               
            return Data;             // Returns word only
    }

    Byte FetchByte(int& Cycles, Memory mem){
        Byte Data = mem.Data[PC];

        dbg_printf("Data Byte = %x \n \n", Data);

        PC++;
        Cycles--;
        return Data;

    }
    


    void Execute(int Cycles, Memory mem){
        while(Cycles > 0){
            Word Instruction = FetchWord(Cycles, mem);        // Gets instruction
            switch(Instruction){


                case INS_MOVEB:{
                    dbg_printf("MOVEB FOUND\n");

                    Byte MoveData = FetchByte(Cycles, mem);      //Grabs data to be moved
                    Byte Register = FetchByte(Cycles, mem);      //Grabs Register
                    

                    switch(Register){
                        case 0x00D0:{
                            dbg_printf("REGISTER D0\n");
                            D0 = MoveData;
                            Z = (MoveData == 0x00);
                            N = (MoveData & 0b10000000) > 0;
                            break;}
                        case 0x00D1:{
                            dbg_printf("REGISTER D1\n");
                            D1 = MoveData;
                            Z = (MoveData == 0x00);
                            N = (MoveData & 0b10000000) > 0;
                            break;}
                        case 0x00D2:{
                            dbg_printf("REGISTER D2\n");
                            D2 = MoveData;
                            Z = (MoveData == 0x00);
                            N = (MoveData & 0b10000000) > 0;
                            break;}
                        case 0x00D3:{
                            dbg_printf("REGISTER D3\n");
                            D3 = MoveData;
                            Z = (MoveData == 0x00);
                            N = (MoveData & 0b10000000) > 0;
                            break;}
                        case 0x00D4:{
                            dbg_printf("REGISTER D4\n");
                            D4 = MoveData;
                            Z = (MoveData == 0x00);
                            N = (MoveData & 0b10000000) > 0;
                            break;}
                        case 0x00D5:{
                            dbg_printf("REGISTER D5\n");
                            D5 = MoveData;
                            Z = (MoveData == 0x00);
                            N = (MoveData & 0b10000000) > 0;
                            break;}
                        case 0x00D6:{
                            dbg_printf("REGISTER D6\n");
                            D6 = MoveData;
                            Z = (MoveData == 0x00);
                            N = (MoveData & 0b10000000) > 0;
                            break;}
                        case 0x00D7:{
                            dbg_printf("REGISTER D7\n");
                            D7 = MoveData;
                            Z = (MoveData == 0x00);
                            N = (MoveData & 0b10000000) > 0;
                            break;}
                        default:
                            dbg_printf("REGISTER ERROR");
                            break; 
                    }
























                }
                default:
                    break; 
            }
        }
    }
};


int main(void){
    Memory mem;
    CPU cpu;

    cpu.Reset(mem);

    //INLINE PRG
    mem.Data[4] = (INS_MOVEB >> 8);
    mem.Data[5] = INS_MOVEB;
    //mem.Data[6] = 0x00;
    mem.Data[6] = 0x25;
    //mem.Data[8] = 0x00;
    mem.Data[7] = 0xD7;

    //ENDPRG

    cpu.Execute(3, mem);

    mem.Data[17] = cpu.D0;

    dbg_printf("D0 = %x\n", cpu.D0);
    dbg_printf("D1 = %x\n", cpu.D1);
    dbg_printf("D2 = %x\n", cpu.D2);
    dbg_printf("D3 = %x\n", cpu.D3);
    dbg_printf("D4 = %x\n", cpu.D4);
    dbg_printf("D5 = %x\n", cpu.D5);
    dbg_printf("D6 = %x\n", cpu.D6);
    dbg_printf("D7 = %x\n", cpu.D7);

    
    while (!os_GetCSC());

    return 0;
}

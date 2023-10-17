#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <typeinfo>

#include <fileioc.h>
#include <ti/getcsc.h>
#include <debug.h>

    using Byte = unsigned char;  //8bit
    using Word = unsigned short; //16bit
    //using DWord = unsigned long; //32bit
    using u32 = unsigned long;
    using u16 = unsigned short;


    




    // OPCODES
    static constexpr Word 
        INS_MOVEB_IMM = 0x003C,
        INS_MOVEB_ADDR = 0x003A,
        //INS_MOVEW_IMM
        //INS_MOVEW_ADDR
        //INS_MOVEL_IMM
        //INS_MOVEL_ADDR
        
        INS_LEA = 0x00F8;

struct DWord{
    Word H1;
    Word H2;

    operator u32() {
        return (H1 + H2);
  }
};



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

    int TESTINT = 0;

    void setDWord(DWord& DW, u32 Data){
        dbg_printf(typeid(Data).name());
        DW.H1 = (Data >> 16);
        DW.H2 = (Data & 0b0000000011111111);
    }

    void Reset(Memory mem){

        setDWord(PC, 0x00);

        SSP.H1 = mem.Data[PC.H1];  //Load Contents of SSP with data at address $00
        SSP.H2 = mem.Data[PC.H2];  //Load Contents of SSP with data at address $00

        setDWord(PC, 0x00004);  //Sets program counter to address $04

        dbg_printf("PC == %x%x\n", PC.H1, PC.H2);

        I0 = 1;       //Load interrupt registers with value of #$07
        I1 = 1;
        I2 = 1;

        setDWord(SP, 0x00100000);   //Initialize Stack Pointer to #$00100000

        setDWord(D0, 0x00);
        setDWord(D1, 0x00);
        setDWord(D2, 0x00);
        setDWord(D3, 0x00);
        setDWord(D4, 0x00);
        setDWord(D5, 0x00);
        setDWord(D6, 0x00);
        setDWord(D7, 0x00);
        setDWord(A0, 0x00);
        setDWord(A1, 0x00);
        setDWord(A2, 0x00);
        setDWord(A3, 0x00);
        setDWord(A4, 0x00);
        setDWord(A5, 0x00);
        setDWord(A6, 0x00);


        

        mem.Init();
    }

    void FetchDWord(int& Cycles, Memory mem, DWord& DW){
            Cycles += 2;
            DW.H1 = FetchWord(Cycles, mem);
            DW.H2 = FetchWord(Cycles, mem);
            
            dbg_printf("Data Double Word = %x%x \n \n", DW.H1, DW.H2);

            Cycles--;               
    }

    Word FetchWord(int& Cycles, Memory mem){
            dbg_printf("PC Half 1 == %x\n", PC.H1);
            dbg_printf("PC Half 2 == %x\n", PC.H2);
            Word DP1 = mem.Data[PC.H2];    //Stores half of Word
            dbg_printf("D1 = %x \n", DP1);
            dbg_printf("D1 S= %x \n", mem.Data[PC.H2]);
            PC.H2++;                    // Increases Program Counter

            Byte DP2 = mem.Data[PC.H2];    //Stores other half of Word
            dbg_printf("D2 = %x \n", DP2);
            dbg_printf("D2 S= %x \n", mem.Data[PC.H2]);

            DP1 = (DP1 << 8); //Moves first half of word over 8 bits


            Word Data = (DP1 + DP2);   //Adds two halfs of word together
            dbg_printf("Data Word = %x \n \n", Data);

            PC.H2++;                    // Increases Program Counter
            Cycles--;               
            return Data;             // Returns word only
    }

    Byte FetchByte(int& Cycles, Memory mem){
        Byte Data = mem.Data[PC.H2];

        dbg_printf("Data Byte = %x \n \n", Data);

        PC.H1++;
        Cycles--;
        return Data;

    }
    


    void Execute(int Cycles, Memory mem){
        while(Cycles > 0){
            Word Instruction = FetchWord(Cycles, mem);        // Gets instruction
            Byte InstructionB = (Instruction & 0x00FF);
            dbg_printf("INS READ\n");
            switch(InstructionB){


                case INS_MOVEB_IMM:{
                    dbg_printf("MOVEB FOUND\n");

                    Byte MoveData = FetchByte(Cycles, mem);      //Grabs data to be moved
                    Byte Register = ((Instruction & 0b1111111100000000) >> 8);      //Grabs Register

                    dbg_printf("REGISTER HEX = %x\n", Register);
                    

                    switch(Register){
                        case 0x10:{
                            dbg_printf("REGISTER D0\n");
                            setDWord(D0,MoveData);
                            Z = (MoveData == 0x00);
                            N = (MoveData & 0b10000000) > 0;
                            dbg_printf("BYTE MOVED\n");
                            TESTINT++;
                            break;}
                        case 0x12:{
                            dbg_printf("REGISTER D1\n");
                            setDWord(D1,MoveData);
                            Z = (MoveData == 0x00);
                            N = (MoveData & 0b10000000) > 0;
                            break;}
                        case 0x14:{
                            dbg_printf("REGISTER D2\n");
                            setDWord(D2,MoveData);
                            Z = (MoveData == 0x00);
                            N = (MoveData & 0b10000000) > 0;
                            break;}
                        case 0x16:{
                            dbg_printf("REGISTER D3\n");
                            setDWord(D3,MoveData);
                            Z = (MoveData == 0x00);
                            N = (MoveData & 0b10000000) > 0;
                            break;}
                        case 0x18:{
                            dbg_printf("REGISTER D4\n");
                            setDWord(D4,MoveData);
                            Z = (MoveData == 0x00);
                            N = (MoveData & 0b10000000) > 0;
                            break;}
                        case 0x1A:{
                            dbg_printf("REGISTER D5\n");
                            setDWord(D5,MoveData);
                            Z = (MoveData == 0x00);
                            N = (MoveData & 0b10000000) > 0;
                            break;}
                        case 0x1C:{
                            dbg_printf("REGISTER D6\n");
                            setDWord(D6,MoveData);
                            Z = (MoveData == 0x00);
                            N = (MoveData & 0b10000000) > 0;
                            break;}
                        case 0x1E:{
                            dbg_printf("REGISTER D7\n");
                            setDWord(D7,MoveData);
                            Z = (MoveData == 0x00);
                            N = (MoveData & 0b10000000) > 0;
                            break;}
                        default:
                            dbg_printf("REGISTER ERROR 1\n");
                            break; 
                    }
                    break;
                }



                case INS_MOVEB_ADDR:{
                        dbg_printf("MOVEB ADDR FOUND\n");

                        Byte MoveData = FetchByte(Cycles, mem);      //Grabs data to be moved
                        Byte Register = FetchByte(Cycles, mem);      //Grabs Register
                    

                        switch(Register){
                            case 0x00D0:{
                                dbg_printf("REGISTER D0\n");
                                setDWord(D0,MoveData);
                                Z = (MoveData == 0x00);
                                N = (MoveData & 0b10000000) > 0;
                                dbg_printf("BYTE MOVED\n");
                                break;}
                            case 0x00D1:{
                                dbg_printf("REGISTER D1\n");
                                setDWord(D1,MoveData);
                                Z = (MoveData == 0x00);
                                N = (MoveData & 0b10000000) > 0;
                                break;}
                            case 0x00D2:{
                                dbg_printf("REGISTER D2\n");
                                setDWord(D2,MoveData);
                                Z = (MoveData == 0x00);
                                N = (MoveData & 0b10000000) > 0;
                                break;}
                            case 0x00D3:{
                                dbg_printf("REGISTER D3\n");
                                setDWord(D3,MoveData);
                                Z = (MoveData == 0x00);
                                N = (MoveData & 0b10000000) > 0;
                                break;}
                            case 0x00D4:{
                                dbg_printf("REGISTER D4\n");
                                setDWord(D4,MoveData);
                                Z = (MoveData == 0x00);
                                N = (MoveData & 0b10000000) > 0;
                                break;}
                            case 0x00D5:{
                                dbg_printf("REGISTER D5\n");
                                setDWord(D5,MoveData);
                                Z = (MoveData == 0x00);
                                N = (MoveData & 0b10000000) > 0;
                                break;}
                            case 0x00D6:{
                                dbg_printf("REGISTER D6\n");
                                setDWord(D6,MoveData);
                                Z = (MoveData == 0x00);
                                N = (MoveData & 0b10000000) > 0;
                                break;}
                            case 0x00D7:{
                                dbg_printf("REGISTER D7\n");
                                setDWord(D7,MoveData);
                                Z = (MoveData == 0x00);
                                N = (MoveData & 0b10000000) > 0;
                                break;}
                            default:
                                dbg_printf("REGISTER ERROR\n");
                                dbg_printf("%i\n"),TESTINT;
                                break; 
                        }
                    }



                    //Word Instruction = FetchWord(Cycles, mem);        // Gets instruction
                    //Byte InstructionB = (Instruction & 0x00FF);
                    case INS_LEA:{
                        dbg_printf("LEA FOUND\n");

                        DWord Address;  FetchDWord(Cycles, mem, Address);      //Grabs data to be moved
                        Byte Register = ((Instruction & 0b1111111100000000) >> 8);      //Grabs Register

                        switch(Register){
                            case 0x41:{
                                dbg_printf("REGISTER A1\n");
                                setDWord(A0, Address);
                                break;

                            }
                            case 0x43:{
                                
                            }
                            case 0x45:{
                                
                            }
                            case 0x47:{
                                
                            }
                            case 0x49:{
                                
                            }
                            case 0x4B:{
                                
                            }
                            case 0x4D:{
                                
                            }

                            default:
                                break;

                            
                        }
                    }


                    default:
                            dbg_printf("INS ERROR\n");
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

        //REGISTER
    mem.Data[4] = 0x12;
        //INS
    mem.Data[5] = INS_MOVEB_IMM;
        //DATA
    mem.Data[6] = 0x10;

    //ENDPRG

    cpu.Execute(2, mem);

    mem.Data[17] = cpu.D0;

    dbg_printf("D0 = %x%x\n", cpu.D0.H1, cpu.D0.H2);
    dbg_printf("D1 = %x%x\n", cpu.D1.H1, cpu.D1.H2);
    dbg_printf("D2 = %x%x\n", cpu.D2.H1, cpu.D2.H2);
    dbg_printf("D3 = %x%x\n", cpu.D3.H1, cpu.D3.H2);
    dbg_printf("D4 = %x%x\n", cpu.D4.H1, cpu.D4.H2);
    dbg_printf("D5 = %x%x\n", cpu.D5.H1, cpu.D5.H2);
    dbg_printf("D6 = %x%x\n", cpu.D6.H1, cpu.D6.H2);
    dbg_printf("D7 = %x%x\n", cpu.D7.H1, cpu.D7.H2);

    dbg_printf("D7 = %x%x\n", cpu.A0.H1, cpu.A0.H2);
    dbg_printf("D7 = %x%x\n", cpu.A1.H1, cpu.A1.H2);
    dbg_printf("D7 = %x%x\n", cpu.A2.H1, cpu.A2.H2);
    dbg_printf("D7 = %x%x\n", cpu.A3.H1, cpu.A3.H2);
    dbg_printf("D7 = %x%x\n", cpu.A4.H1, cpu.A4.H2);
    dbg_printf("D7 = %x%x\n", cpu.A5.H1, cpu.A5.H2);
    dbg_printf("D7 = %x%x\n", cpu.A6.H1, cpu.A6.H2);


    
    while (!os_GetCSC());

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fileioc.h>
#include <ti/getcsc.h>
#include <debug.h>

    using Byte = unsigned char;  //8bit
    using Word = unsigned short; //16bit
    using u32 = unsigned int;
    using u16 = unsigned short;


    




    // OPCODES
    static constexpr Word 
        INS_MOVEB_IMM_R = 0x003C,
        INS_MOVEB_IMM_A_W = 0x11FC,
        INS_MOVEB_IMM_A_D = 0x13FC,

        //INS_MOVEB_ADD_A = 0x00F8,
        INS_MOVEB_ADD_R = 0x0038,

        //INS_MOVEB_RG__A = 0x00C1,

        //INS_MOVEB_RG_D0 = 0x0000,
        //INS_MOVEB_RG_D1 = 0x0001,
        //INS_MOVEB_RG_D2 = 0x0002,
        //INS_MOVEB_RG_D3 = 0x0003,
        //INS_MOVEB_RG_D4 = 0x0004,
        //INS_MOVEB_RG_D5 = 0x0005,
        //INS_MOVEB_RG_D6 = 0x0006,
        //INS_MOVEB_RG_D7 = 0x0007,
        //INS_MOVEW_IMM
        //INS_MOVEW_ADDR
        //INS_MOVEL_IMM
        //INS_MOVEL_ADDR
        
        INS_LEA = 0x00F8;

struct DWord{
    Word H1;
    Word H2;
    int Testy;

    void Add(u16 Addend){
        if(H1 == 0xFFFF){
            H2 = 0x0000;
            H1 += Addend;
        }
        else{
            H2 += Addend;
        }
    }

    void Inc(){
        if(H1 == 0xFFFF){
            H2 = 0x0000;
            H1++;
        }
        else{
            H2++;
        }
    }
    operator u32() {
        return (H1 + H2);
  }
};



struct Memory{
    static constexpr u32 MAX_MEM = size_t((65536)); //64k

    static inline Byte Data[MAX_MEM]; //64k block of RAM

    void Init(){
        for(u32 i = 0; i < MAX_MEM; i++){  //Sets all of mem to 0x00
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
        if(DW.Testy == 1) dbg_printf("DATA == %x\n", Data);
        DW.H1 = (Data >> 16);
        if(DW.Testy == 1) dbg_printf("DW H1 == %x\n", DW.H1);
        DW.H2 = (Data);
        if(DW.Testy == 1) dbg_printf("DW H2 == %x\n", DW.H2);
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
            PC.Add(0x0001);                  // Increases Program Counter

            Byte DP2 = mem.Data[PC.H2];    //Stores other half of Word
            dbg_printf("D2 = %x \n", DP2);
            dbg_printf("D2 S= %x \n", mem.Data[PC.H2]);

            DP1 = (DP1 << 8); //Moves first half of word over 8 bits


            Word Data = (DP1 + DP2);   //Adds two halfs of word together
            dbg_printf("Data Word = %x \n \n", Data);

            PC.Add(0x0001);
                              // Increases Program Counter
            Cycles--;               
            return Data;             // Returns word only
    }

    Byte FetchByte(int& Cycles, Memory mem){
        Byte Data = mem.Data[PC.H2];

        dbg_printf("Data Byte = %x \n \n", Data);

        PC.Add(0x0001);
        Cycles--;
        return Data;

    }
    


    void Execute(int Cycles, Memory& mem){
        while(Cycles > 0){
            Word Instruction = FetchWord(Cycles, mem);        // Gets instruction
            Byte InstructionB = (Instruction & 0x00FF);
            dbg_printf("INS READ\n");
            switch(InstructionB){


                case INS_MOVEB_IMM_R:{
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

                case INS_MOVEB_ADD_R:{
                        dbg_printf("MOVEB ADDR_R FOUND\n");

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
                                break; 
                        }
                    }



                    //Word Instruction = FetchWord(Cycles, mem);        // Gets instruction
                    //Byte InstructionB = (Instruction & 0x00FF);
                    case INS_LEA:{
                        dbg_printf("LEA FOUND\n");

                        Byte Register = ((Instruction & 0b1111111100000000) >> 8);      //Grabs Register

                        switch(Register){
                            case 0x41:{
                                dbg_printf("REGISTER A0\n");
                                FetchDWord(Cycles, mem, A0);  // Grabs Address

                                dbg_printf("A0.H1 == %x\n", A0.H1);
                                dbg_printf("A0.H2 == %x\n\n", A0.H2);
                                break;
                            }
                            case 0x43:{
                                dbg_printf("REGISTER A1\n");
                                FetchDWord(Cycles, mem, A1);

                                dbg_printf("A0.H1 == %x\n", A1.H1);
                                dbg_printf("A0.H2 == %x\n\n", A1.H2);
                                break;
                                
                            }
                            case 0x45:{
                                dbg_printf("REGISTER A2\n");
                                FetchDWord(Cycles, mem, A2);

                                dbg_printf("A0.H1 == %x\n", A2.H1);
                                dbg_printf("A0.H2 == %x\n\n", A2.H2);
                                break;
                                
                            }
                            case 0x47:{
                                dbg_printf("REGISTER A3\n");
                                FetchDWord(Cycles, mem, A3);

                                dbg_printf("A0.H1 == %x\n", A3.H1);
                                dbg_printf("A0.H2 == %x\n\n", A3.H2);
                                break;
                                
                            }
                            case 0x49:{
                                dbg_printf("REGISTER A4\n");
                                FetchDWord(Cycles, mem, A4);

                                dbg_printf("A0.H1 == %x\n", A4.H1);
                                dbg_printf("A0.H2 == %x\n\n", A4.H2);
                                break;
                                
                            }
                            case 0x4B:{
                                dbg_printf("REGISTER A5\n");
                                FetchDWord(Cycles, mem, A5);

                                dbg_printf("A0.H1 == %x\n", A5.H1);
                                dbg_printf("A0.H2 == %x\n\n", A5.H2);
                                break;
                                
                            }
                            case 0x4D:{
                                dbg_printf("REGISTER A6\n");
                                FetchDWord(Cycles, mem, A6);

                                dbg_printf("A0.H1 == %x\n", A6.H1);
                                dbg_printf("A0.H2 == %x\n\n", A6.H2);
                                break;
                                
                            }

                            default:
                                break;

                            
                        }
                        break;
                    }


                    default:
                        switch(Instruction){                //When no 1-byte opcode found, start searching for 2-byte opcodes
                            case INS_MOVEB_IMM_A_W:{
                                dbg_printf("MOVEB IMM_A_W FOUND\n");

                                PC.Inc();

                                Byte MoveData = FetchByte(Cycles, mem);      //Grabs data to be moved
                                Word Address = FetchWord(Cycles, mem);       //Grabs recipient address

                                dbg_printf("MoveData = %x\n", MoveData);
                                dbg_printf("Address = %x\n", Address);

                                mem.Data[Address] = MoveData;     

                                dbg_printf("Address Cont. = %x\n", mem.Data[Address]); 
                                break;  
                            }
                            case INS_MOVEB_IMM_A_D:{    
                                //CURRENTLY CRASHES EMULATOR
                                //CURRENTLY CRASHES EMULATOR
                                //CURRENTLY CRASHES EMULATOR
                                //CURRENTLY CRASHES EMULATOR
                                //CURRENTLY CRASHES EMULATOR
                                //CURRENTLY CRASHES EMULATOR
                                //CURRENTLY CRASHES EMULATOR
                                dbg_printf("MOVEB IMM_A_D FOUND\n");

                                PC.Inc();

                                Byte MoveData = FetchByte(Cycles, mem);      //Grabs data to be moved

                                DWord Address;
                                FetchDWord(Cycles, mem, Address);       //Grabs recipient address

                                dbg_printf("MoveData = %x\n", MoveData);
                                dbg_printf("Address = %x%x\n", Address.H1, Address.H2);

                                mem.Data[(Address.H1 << 16) + Address.H2] = MoveData;   

                                dbg_printf("Address Cont. = %x\n", mem.Data[Address]); 
                                break;     
                            }
                        }
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
    mem.Data[4] = (INS_MOVEB_IMM_A_W >> 8);
        //INS
    mem.Data[5] = (Byte)INS_MOVEB_IMM_A_W;
        //DATA
    mem.Data[6] = 0x00;
    mem.Data[7] = 0x34;

    mem.Data[8] = 0x00;
    mem.Data[9] = 0x78;



    //ENDPRG

    cpu.Execute(2, mem);

    dbg_printf("$0078 = %x\n", mem.Data[0x0078]);

    dbg_printf("D0 = %x%x\n", cpu.D0.H1, cpu.D0.H2);
    dbg_printf("D1 = %x%x\n", cpu.D1.H1, cpu.D1.H2);
    dbg_printf("D2 = %x%x\n", cpu.D2.H1, cpu.D2.H2);
    dbg_printf("D3 = %x%x\n", cpu.D3.H1, cpu.D3.H2);
    dbg_printf("D4 = %x%x\n", cpu.D4.H1, cpu.D4.H2);
    dbg_printf("D5 = %x%x\n", cpu.D5.H1, cpu.D5.H2);
    dbg_printf("D6 = %x%x\n", cpu.D6.H1, cpu.D6.H2);
    dbg_printf("D7 = %x%x\n", cpu.D7.H1, cpu.D7.H2);

    dbg_printf("A0 = %x%x\n", cpu.A0.H1, cpu.A0.H2);
    dbg_printf("A1 = %x%x\n", cpu.A1.H1, cpu.A1.H2);
    dbg_printf("A2 = %x%x\n", cpu.A2.H1, cpu.A2.H2);
    dbg_printf("A3 = %x%x\n", cpu.A3.H1, cpu.A3.H2);
    dbg_printf("A4 = %x%x\n", cpu.A4.H1, cpu.A4.H2);
    dbg_printf("A5 = %x%x\n", cpu.A5.H1, cpu.A5.H2);
    dbg_printf("A6 = %x%x\n", cpu.A6.H1, cpu.A6.H2);


    
    while (!os_GetCSC());

    return 0;
}
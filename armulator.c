
#include "armdefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ARMul_State mystate;

ARMword
ARMul_Debug (ARMul_State * state, ARMword pc, ARMword instr)
{
    return(instr);
}
ARMword
read_cp15_reg (unsigned reg, unsigned opcode_2, unsigned CRm)
{
    return(0);
}

unsigned ARMul_OSHandleSWI (ARMul_State * state, ARMword number)
{
    return(0);
}
int ARMul_HandleIwmmxt (ARMul_State *state, ARMword word)
{
    return(0);
}

ARMword  ARMul_MRC           (ARMul_State *s, ARMword w) { exit(1); }
void     ARMul_CDP           (ARMul_State *s, ARMword w) { exit(1); }
void     ARMul_LDC           (ARMul_State *s, ARMword w, ARMword d) { exit(1); }
void     ARMul_STC           (ARMul_State *s, ARMword w, ARMword d) { exit(1); }
void     ARMul_MCR           (ARMul_State *s, ARMword w, ARMword d) { exit(1); }

//extern void ARMul_EmulateInit (void); asdf
////extern ARMul_State *ARMul_NewState (void);
//extern void ARMul_NewState ( ARMul_State * state);
//extern void ARMul_Reset (ARMul_State * state);
//extern ARMword ARMul_DoProg (ARMul_State * state);
//extern ARMword ARMul_DoInstr (ARMul_State * state);

unsigned int mymem[0x200000];

int stop_simulator ;




/***************************************************************************\
*        Get a Word from Virtual Memory, maybe allocating the page          *
\***************************************************************************/

extern unsigned int mymem[];

ARMword GetWord (ARMul_State * state, ARMword address, int check)
{
    ARMword data;

//printf("GetWord(0x%08X) ",address);

    //uart at 0x16000000
    if(address>=0x200000)
    {
        printf("ERROR GetWord(0x%X)\n",address);
        exit(1);
    }
    data=mymem[address>>2];
//printf("= 0x%08X\n",data);

    return(data);
}


void PutWord (ARMul_State * state, ARMword address, ARMword data, int check)
{
//printf("PutWord(0x%08X,0x%08X)\n",address,data);

    if(address==0x16000000)
    {
        //uart write
        fprintf(stdout,"%c",data&0xFF);
        fflush(stdout);
        return;
    }
    if(address==0xE0000000)
    {
        printf("\nERROR PutWord(0x%X,0x%X)\n",address,data);
        printf("NumScycles %lu\n",state->NumScycles);
        printf("NumNcycles %lu\n",state->NumNcycles);
        printf("NumIcycles %lu\n",state->NumIcycles);
        printf("NumCcycles %lu\n",state->NumCcycles);
        printf("NumFcycles %lu\n",state->NumFcycles);
        printf("NumInstrs  %lu\n",state->NumInstrs );
        {
            unsigned long la;
            la=0;
            la+=state->NumScycles;
            la+=state->NumNcycles;
            la+=state->NumIcycles;
            la+=state->NumCcycles;
            la+=state->NumFcycles;
            printf("TotlCycles %lu\n",la);
        }
        printf("\n");
        //exit(0);
    }
    if(address==0xF0000000)
    {
        printf("\nERROR PutWord(0x%X,0x%X)\n",address,data);
        printf("NumScycles %lu\n",state->NumScycles);
        printf("NumNcycles %lu\n",state->NumNcycles);
        printf("NumIcycles %lu\n",state->NumIcycles);
        printf("NumCcycles %lu\n",state->NumCcycles);
        printf("NumFcycles %lu\n",state->NumFcycles);
        printf("NumInstrs  %lu\n",state->NumInstrs );
        {
            unsigned long la;
            la=0;
            la+=state->NumScycles;
            la+=state->NumNcycles;
            la+=state->NumIcycles;
            la+=state->NumCcycles;
            la+=state->NumFcycles;
            printf("TotlCycles %lu\n",la);
        }
        exit(0);
    }

    if(address>=0x200000)
    {
        //printf("\nERROR PutWord(0x%X,0x%X)\n",address,data);
        printf("Memory Fault\n");
        exit(1);
    }
    mymem[address>>2]=data;
    printf("mem[%08x] = %08x\n", address, data);
}

int main ( void )
{
    FILE *fp;
    ARMword pc;

    memset(mymem,0,sizeof(mymem));
    fp=fopen("00000000.bin","rb");
    if(fp==NULL)
    {
        printf("Error opening file 00000000.bin\n");
        return(1);
    }
    fread(mymem,1,sizeof(mymem),fp);
    fclose(fp);

    ARMul_EmulateInit();

    stop_simulator=0;

    memset(&mystate,0,sizeof(mystate));
    ARMul_NewState(&mystate);
    pc = 0;
    pc = ARMul_DoProg(&mystate);
    //printf("\n");
    return(0);
}

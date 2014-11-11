/*  arminit.c -- ARMulator initialization:  ARM6 Instruction Emulator.
    Copyright (C) 1994 Advanced RISC Machines Ltd.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA. */

/* heavily modified to only support arm7 and thumb David Welch 2010 */

#include <string.h>

#include "armdefs.h"
#include "armemu.h"

#define RDIError_NoError                0

#define RDIError_Reset                  1
#define RDIError_UndefinedInstruction   2
#define RDIError_SoftwareInterrupt      3
#define RDIError_PrefetchAbort          4
#define RDIError_DataAbort              5
#define RDIError_AddressException       6
#define RDIError_IRQ                    7
#define RDIError_FIQ                    8
#define RDIError_Error                  9
#define RDIError_BranchThrough0         10

#define RDIError_NotInitialised         128
#define RDIError_UnableToInitialise     129
#define RDIError_WrongByteSex           130
#define RDIError_UnableToTerminate      131
#define RDIError_BadInstruction         132
#define RDIError_IllegalInstruction     133
#define RDIError_BadCPUStateSetting     134
#define RDIError_UnknownCoPro           135
#define RDIError_UnknownCoProState      136
#define RDIError_BadCoProState          137
#define RDIError_BadPointType           138
#define RDIError_UnimplementedType      139
#define RDIError_BadPointSize           140
#define RDIError_UnimplementedSize      141
#define RDIError_NoMorePoints           142
#define RDIError_BreakpointReached      143
#define RDIError_WatchpointAccessed     144
#define RDIError_NoSuchPoint            145
#define RDIError_ProgramFinishedInStep  146
#define RDIError_UserInterrupt          147
#define RDIError_CantSetPoint           148
#define RDIError_IncompatibleRDILevels  149

#define RDIError_CantLoadConfig         150
#define RDIError_BadConfigData          151
#define RDIError_NoSuchConfig           152
#define RDIError_BufferFull             153
#define RDIError_OutOfStore             154
#define RDIError_NotInDownload          155
#define RDIError_PointInUse             156
#define RDIError_BadImageFormat         157
#define RDIError_TargetRunning          158

#define RDIError_LittleEndian           240
#define RDIError_BigEndian              241
#define RDIError_SoftInitialiseError    242

#define RDIError_InsufficientPrivilege  253
#define RDIError_UnimplementedMessage   254
#define RDIError_UndefinedMessage       255

/***************************************************************************\
*                 Definitions for the emulator architecture                 *
\***************************************************************************/

void ARMul_EmulateInit (void);
//ARMul_State *ARMul_NewState (void);
void ARMul_NewState (ARMul_State * state);
void ARMul_Reset (ARMul_State * state);
ARMword ARMul_DoCycle (ARMul_State * state);
unsigned ARMul_DoCoPro (ARMul_State * state);
ARMword ARMul_DoProg (ARMul_State * state);
ARMword ARMul_DoInstr (ARMul_State * state);
void ARMul_Abort (ARMul_State * state, ARMword address);

unsigned ARMul_MultTable[32] =
{
   1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,
   9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 16
};
ARMword ARMul_ImmedTable[4096]; /* immediate DP LHS values */
char ARMul_BitList[256];    /* number of bits in a byte table */

/***************************************************************************\
*         Call this routine once to set up the emulator's tables.           *
\***************************************************************************/

void ARMul_EmulateInit (void)
{
    unsigned long i, j;

    for (i = 0; i < 4096; i++)
    {               /* the values of 12 bit dp rhs's */
        ARMul_ImmedTable[i] = ROTATER (i & 0xffL, (i >> 7L) & 0x1eL);
    }

    for (i = 0; i < 256; ARMul_BitList[i++] = 0); /* how many bits in LSM */
    for (j = 1; j < 256; j <<= 1)
        for (i = 0; i < 256; i++)
            if ((i & j) > 0)
                ARMul_BitList[i]++;

    for (i = 0; i < 256; i++)
        ARMul_BitList[i] *= 4;  /* you always need 4 times these values */
}

/***************************************************************************\
*            Returns a new instantiation of the ARMulator's state           *
\***************************************************************************/

void ARMul_NewState (ARMul_State * state)
{
    unsigned i, j;

    state->Emulate = RUN;
    for (i = 0; i < 16; i++)
    {
        state->Reg[i] = 0;
        for (j = 0; j < 7; j++)
            state->RegBank[j][i] = 0;
    }
    for (i = 0; i < 7; i++)
        state->Spsr[i] = 0;

    state->Mode = USER32MODE;

    state->CallDebug = FALSE;
    state->Debug = FALSE;
    state->VectorCatch = 0;
    state->Aborted = FALSE;
    state->Reseted = FALSE;
    state->Inted = 3;
    state->LastInted = 3;

    state->OSptr = NULL;
    state->CommandLine = NULL;

    state->LastTime = 0;

    state->EventSet = 0;
    state->Now = 0;
    state->EventPtr = (struct EventNode **) malloc ((unsigned) EVENTLISTSIZE *sizeof (struct EventNode *));
    for (i = 0; i < EVENTLISTSIZE; i++)  *(state->EventPtr + i) = NULL;

    state->lateabtSig = LOW;
    state->bigendSig = LOW;

    ARMul_Reset (state);

}

/***************************************************************************\
  Call this routine to set ARMulator to model certain processor properities
\***************************************************************************/

void
ARMul_SelectProcessor (ARMul_State * state, unsigned properties)
{

  state->lateabtSig = LOW;

}

/***************************************************************************\
* Call this routine to set up the initial machine state (or perform a RESET *
\***************************************************************************/

void ARMul_Reset (ARMul_State * state)
{
    state->NextInstr = 0;

      state->Reg[15] = 0;
      state->Cpsr = INTBITS | SVC32MODE;
      state->Mode = SVC32MODE;

    ARMul_CPSRAltered (state);
    state->Bank = SVCBANK;

    FLUSHPIPE;

    state->EndCondition = 0;
    state->ErrorCode = 0;

    state->Exception = FALSE;
    state->NresetSig = HIGH;
    state->NfiqSig = HIGH;
    state->NirqSig = HIGH;
    state->NtransSig = (state->Mode & 3) ? HIGH : LOW;
    state->abortSig = LOW;
    state->AbortAddr = 1;

    state->NumInstrs = 0;
    state->NumNcycles = 0;
    state->NumScycles = 0;
    state->NumIcycles = 0;
    state->NumCcycles = 0;
    state->NumFcycles = 0;
}


/***************************************************************************\
* Emulate the execution of an entire program.  Start the correct emulator   *
* (Emulate26 for a 26 bit ARM and Emulate32 for a 32 bit ARM), return the   *
* address of the last instruction that is executed.                         *
\***************************************************************************/

ARMword ARMul_DoProg (ARMul_State * state)
{
    ARMword pc = 0;

    state->Emulate = RUN;
    while (state->Emulate != STOP)
    {
        state->Emulate = RUN;
        pc = ARMul_Emulate32 (state);
    }
    return (pc);
}

/***************************************************************************\
* Emulate the execution of one instruction.  Start the correct emulator     *
* (Emulate26 for a 26 bit ARM and Emulate32 for a 32 bit ARM), return the   *
* address of the instruction that is executed.                              *
\***************************************************************************/

ARMword ARMul_DoInstr (ARMul_State * state)
{
    ARMword pc = 0;

    state->Emulate = ONCE;
    pc = ARMul_Emulate32 (state);

    return (pc);
}

/***************************************************************************\
* This routine causes an Abort to occur, including selecting the correct    *
* mode, register bank, and the saving of registers.  Call with the          *
* appropriate vector's memory address (0,4,8 ....)                          *
\***************************************************************************/

void ARMul_Abort (ARMul_State * state, ARMword vector)
{
    ARMword temp;
    int isize = INSN_SIZE;
    int esize = (TFLAG ? 0 : 4);
    int e2size = (TFLAG ? -4 : 0);

    state->Aborted = FALSE;

    temp = state->Reg[15];

    switch (vector)
    {
        case ARMul_ResetV:      /* RESET */
            SETABORT (INTBITS, SVC32MODE, 0);
            break;
        case ARMul_UndefinedInstrV: /* Undefined Instruction */
            SETABORT (IBIT, UNDEF32MODE, isize);
            break;
        case ARMul_SWIV:        /* Software Interrupt */
            SETABORT (IBIT, SVC32MODE, isize);
            break;
        case ARMul_PrefetchAbortV:  /* Prefetch Abort */
            state->AbortAddr = 1;
            SETABORT (IBIT, ABORT32MODE, esize);
            break;
        case ARMul_DataAbortV:  /* Data Abort */
            SETABORT (IBIT, ABORT32MODE, e2size);
            break;
        case ARMul_AddrExceptnV:    /* Address Exception */
            SETABORT (IBIT, SVC26MODE, isize);
            break;
        case ARMul_IRQV:        /* IRQ */
            break;
        case ARMul_FIQV:        /* FIQ */
        break;
    }
    if (ARMul_MODE32BIT)
        ARMul_SetR15 (state, vector);
    else
        ARMul_SetR15 (state, R15CCINTMODE | vector);

    if (ARMul_ReadWord (state, ARMul_GetPC (state)) == 0)
    {
        /* No vector has been installed.  Rather than simulating whatever
           random bits might happen to be at address 0x20 onwards we elect
           to stop.  */
        switch (vector)
        {
            case ARMul_ResetV: state->EndCondition = RDIError_Reset; break;
            case ARMul_UndefinedInstrV: state->EndCondition = RDIError_UndefinedInstruction; break;
            case ARMul_SWIV: state->EndCondition = RDIError_SoftwareInterrupt; break;
            case ARMul_PrefetchAbortV: state->EndCondition = RDIError_PrefetchAbort; break;
            case ARMul_DataAbortV: state->EndCondition = RDIError_DataAbort; break;
            case ARMul_AddrExceptnV: state->EndCondition = RDIError_AddressException; break;
            case ARMul_IRQV: state->EndCondition = RDIError_IRQ; break;
            case ARMul_FIQV: state->EndCondition = RDIError_FIQ; break;
            default: break;
        }
        state->Emulate = FALSE;
    }
}

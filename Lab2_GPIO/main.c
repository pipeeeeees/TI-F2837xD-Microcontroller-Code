// ECE 4550 Lab 2 Task 2
// Purpose: Blinking RED LED
// Author: David Pipes

#include "F2837xD_device.h"
#include "math.h"

#define pi 3.14159

// Put here global variable and function declarations
Uint16 output = 1;
Uint32 slow = 0;
Uint32 fast = 0;

#pragma CODE_SECTION(InitFlash, "RamFuncs")
void InitFlash(void)
{
    Flash0CtrlRegs.FPAC1.bit.PMPPWR = 0x1;
    Flash0CtrlRegs.FBFALLBACK.bit.BNKPWR0 = 0x3;
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.DATA_CACHE_EN = 0;
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.PREFETCH_EN = 0;
    Flash0CtrlRegs.FRDCNTL.bit.RWAIT = 0x3;
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.DATA_CACHE_EN = 1;
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.PREFETCH_EN = 1;
    Flash0EccRegs.ECC_ENABLE.bit.ENABLE = 0xA;
    asm(" RPT #6 || NOP");
}

extern Uint16 RamFuncs_loadstart;
extern Uint16 RamFuncs_loadsize;
extern Uint16 RamFuncs_runstart;

void main(void)
{
    EALLOW;
    WdRegs.WDCR.all = 0b0101000;

    ClkCfgRegs.SYSPLLMULT.all = 0;
    ClkCfgRegs.SYSCLKDIVSEL.all = 0;

    // GPIO0 configuration
    GpioCtrlRegs.GPAGMUX1.bit.GPIO0 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;
    GpioCtrlRegs.GPAODR.bit.GPIO0 = 1; // explain what this does. 1 is open drain 0 is normal
    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 0; // consider both 1 and 0. 0 enables pullup. 1 disables pullup
    // GPIO34 (RED LED) configuration
    GpioCtrlRegs.GPBGMUX1.bit.GPIO34 = 0;
    GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;
    GpioCtrlRegs.GPBODR.bit.GPIO34 = 1;
    GpioCtrlRegs.GPBPUD.bit.GPIO34 = 0;
    // GPIO31 (BLUE LED) configuration
    GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;
    GpioCtrlRegs.GPAODR.bit.GPIO31 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO31 = 0;

    memcpy(&RamFuncs_runstart, &RamFuncs_loadstart, (Uint32) &RamFuncs_loadsize);
    InitFlash();

    WdRegs.WDCR.all = 0b1101000;
    EDIS; // to disallow access to protected registers

    // set both RED and BLUE LED's to off
    GpioDataRegs.GPBSET.bit.GPIO34 = 1;
    GpioDataRegs.GPASET.bit.GPIO31 = 1;

    while(1)
    {
        EALLOW;
        WdRegs.WDKEY.all = 0x55;
        WdRegs.WDKEY.all = 0xAA;
        EDIS;

        fast = fast + 1;

        if (fast%100000 == 0){
            slow = slow + 1;
            GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
        }
        if (output == 0){ // pressed state
            GpioDataRegs.GPACLEAR.bit.GPIO0 = 1;
        }
        if (output == 1){ // non pressed state, DAT is pulled up
            GpioDataRegs.GPASET.bit.GPIO0 = 1;
        }
    }
}

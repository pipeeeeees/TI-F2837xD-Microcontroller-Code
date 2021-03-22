// David Pipes
// Lab 3 Task 2
// Purpose: introducing interrupts
#include "F2837xD_device.h"
#include "F2837xD_pievect.h"

interrupt void timerISR(void);

Uint32 count = 0;

void main(void){

    EALLOW;
    WdRegs.WDCR.all = 0b1101000;

    GpioCtrlRegs.GPAGMUX1.bit.GPIO6 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;
    GpioCtrlRegs.GPAODR.bit.GPIO6 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO6 = 0;

    GpioCtrlRegs.GPAGMUX1.bit.GPIO7 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO7 = 1;
    GpioCtrlRegs.GPAODR.bit.GPIO7 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO7 = 0;

    GpioCtrlRegs.GPAGMUX1.bit.GPIO8 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO8 = 1;
    GpioCtrlRegs.GPAODR.bit.GPIO8 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO8 = 0;

    GpioCtrlRegs.GPAGMUX1.bit.GPIO9 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO9 = 1;
    GpioCtrlRegs.GPAODR.bit.GPIO9 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO9 = 0;

    // Initialize the System Clock (CPU Clock), TRM Section 2.7.6.2 (page 115)
    ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL = 0x01; // Set the reference clock, XTAL
    ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 0;   	// Bypass the PLL
    ClkCfgRegs.SYSCLKDIVSEL.all = 0b000001;     // divide by 2
    ClkCfgRegs.SYSPLLMULT.all = 0b10100;        // mult by 20 (20 x 10MHz)
    while(ClkCfgRegs.SYSPLLSTS.bit.LOCKS != 1); // checks if PLL has reached steady state
    ClkCfgRegs.SYSCLKDIVSEL.all = 0b000010;     // set system clock divider and +1 to it
    ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 1;     // Switch to PLL as system clock
    ClkCfgRegs.SYSCLKDIVSEL.all = 0b000001;     // change divider to the appropriate value

    CpuTimer0Regs.PRD.all = 0b1001110000111; //4999 to bring timer freq to 20 kHz
    CpuTimer0Regs.TCR.bit.TSS = 1; // stops timer if 1
    CpuTimer0Regs.TCR.bit.TRB = 1; // equals 1 when we want to load timer with PRDH:PRD
    CpuTimer0Regs.TCR.bit.TIE = 1; // 1 is timer interrupt enabled

    // PIE level
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
    PieVectTable.TIMER0_INT = &timerISR;

    // CPU level
    IER = 1;
    EINT;

    CpuTimer0Regs.TCR.bit.TSS = 0;

    WdRegs.WDCR.all = 0b0101000;    // ENABLE WATCHDOG
    EDIS;


    while(1) {
        EALLOW;
        WdRegs.WDKEY.all = 0x55;
        WdRegs.WDKEY.all = 0xAA;
        EDIS;
    }
}

interrupt void timerISR(void)
{
    if (count < 8){
    	GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;
    	if (count%2 == 0){
    		GpioDataRegs.GPACLEAR.bit.GPIO6 = 1;
    	}
    	else if (count%2 == 1){
    		GpioDataRegs.GPASET.bit.GPIO6 = 1;
    	}
    	if (count <= 3){
    		GpioDataRegs.GPACLEAR.bit.GPIO8 = 1;
    	}
    	if (count >= 4){
    	    GpioDataRegs.GPASET.bit.GPIO8 = 1;
    	}
    	if (count == 0){
    		GpioDataRegs.GPACLEAR.bit.GPIO7 = 1;
    	}
    	if (count == 1){
    		GpioDataRegs.GPACLEAR.bit.GPIO7 = 1;
    	}
    	if (count == 2){
    		GpioDataRegs.GPASET.bit.GPIO7 = 1;
    	}
    	if (count == 3){
    		GpioDataRegs.GPASET.bit.GPIO7 = 1;
    	}
    	if (count == 4){
    		GpioDataRegs.GPACLEAR.bit.GPIO7 = 1;
    	}
    	if (count == 5){
    		GpioDataRegs.GPACLEAR.bit.GPIO7 = 1;
    	}
    	if (count == 6){
    		GpioDataRegs.GPASET.bit.GPIO7 = 1;
    	}
    	if (count == 7){
    		GpioDataRegs.GPASET.bit.GPIO7 = 1;
    	}
    }
    if (count >= 8){
    	GpioDataRegs.GPASET.bit.GPIO9 = 1;
    	if (count%2 == 0){
    	    GpioDataRegs.GPACLEAR.bit.GPIO6 = 1;
    	}
    	else if (count%2 == 1){
    	    GpioDataRegs.GPASET.bit.GPIO6 = 1;
    	}
    	if (count <= 11){
    	    GpioDataRegs.GPACLEAR.bit.GPIO8 = 1;
    	}
    	if (count >= 12){
    	    GpioDataRegs.GPASET.bit.GPIO8 = 1;
    	}
    	if (count == 8){
    	    GpioDataRegs.GPACLEAR.bit.GPIO7 = 1;
    	}
    	if (count == 9){
    	    GpioDataRegs.GPACLEAR.bit.GPIO7 = 1;
    	}
    	if (count == 10){
    	    GpioDataRegs.GPASET.bit.GPIO7 = 1;
    	}
    	if (count == 11){
    	    GpioDataRegs.GPASET.bit.GPIO7 = 1;
    	}
    	if (count == 12){
    	    GpioDataRegs.GPACLEAR.bit.GPIO7 = 1;
    	}
    	if (count == 13){
    	    GpioDataRegs.GPACLEAR.bit.GPIO7 = 1;
    	}
    	if (count == 14){
    	    GpioDataRegs.GPASET.bit.GPIO7 = 1;
    	}
    	if (count == 15){
    	    GpioDataRegs.GPASET.bit.GPIO7 = 1;
    	}
    }

    count = count + 1;
    if (count == 16){
    	count = 0;
    }


    PieCtrlRegs.PIEACK.all = 1;
}

// David Pipes
// Lab 4 Task 2
// Purpose: ADC and DAC
#include "F2837xD_device.h"
#include "F2837xD_pievect.h"
#include "stdlib.h"
#include "math.h"
#define pi 3.14159

interrupt void ADCISR(void);
extern void DelayUs(Uint16);

float32 rand_float (float32 a){         // random variable generator for U
    return ((float32) rand()/RAND_MAX)*2*a-a;
}
Uint32 cycles = 0;
Uint32 i = 0;
Uint32 j = 0;
float32 Vref = 3.3;
float32 t;
float32 U[1000];
float32 Ufilter[1000];
float32 Yfilter[1000];
float32 Y[1000];
float32 temp[3][1000];
float32 A[3][3] = { {-25132.7,1,0}, {-315827340,0,1}, {-1.9844e12,0,0} };
float32 B[3][1] = { {0},{0},{1.9844e12} };
float32 C[1][3] = {1,0,0};
void main(void){
    for (i=0; i<1000; i++){
        t = i*0.00001;
        U[i] = 1.5+sin(2*pi*1000*t)+rand_float(0.1);
    }
    for (i=0; i<1000; i++){
        for (j=0; j<3; j++){
            temp[j][i] = 1;
        }
    }
    float32 Vcmd = U[0];

    EALLOW;                         // Allow changes to protected registers
    WdRegs.WDCR.all = 0b1101000;    // DISABLE WATCHDOG

    // System Clock target = 200MHz             TRM Section 2.7.6.2 (page 115)
    ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL = 0x01; // Set the reference clock: XTAL
    ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 0;        // Bypass the PLL
    ClkCfgRegs.SYSCLKDIVSEL.all = 0b000000;     // divide by 1
    ClkCfgRegs.SYSPLLMULT.all = 0b10100;        // mult by 20 (20 x 10MHz)
    while(ClkCfgRegs.SYSPLLSTS.bit.LOCKS != 1); // checks if PLL has reached steady state
    ClkCfgRegs.SYSCLKDIVSEL.all = 0b000001;     // set system clock divider +1
    ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 1;     // Switch to PLL as system clock
    ClkCfgRegs.SYSCLKDIVSEL.all = 0b000000;     // change divider to the appropriate value

    // Timer Clock target = 100kHz
    CpuTimer0Regs.PRD.all = 0b11111001111; //1999 (+1, 200M/2k = 100k)
    CpuTimer0Regs.TCR.bit.TSS = 1; // stops timer if 1
    CpuTimer0Regs.TCR.bit.TRB = 1; // equals 1 when we want to load timer with PRDH:PRD
    CpuTimer0Regs.TCR.bit.TIE = 1; // 1 is timer interrupt enabled
    CpuTimer0Regs.TCR.bit.TSS = 0;

    // PIE level
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;
    PieVectTable.ADCA1_INT = &ADCISR;
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1; //locates interrupt source 1.1 (ADCA1)

    // initialize dac
    CpuSysRegs.PCLKCR16.bit.DAC_A = 1;  //select dac and turn on or off
    CpuSysRegs.PCLKCR16.bit.DAC_B = 1;  //select dac and turn on or off
    asm(" NOP");
    asm(" NOP");
    DacaRegs.DACCTL.bit.DACREFSEL = 1;  // set the reference voltage (1 -> 3.3V)
    DacaRegs.DACOUTEN.bit.DACOUTEN = 1; // enable the dac module output
    DacaRegs.DACVALS.all = 4096*Vcmd/Vref;

    DacbRegs.DACCTL.bit.DACREFSEL = 1;  // set the reference voltage (1 -> 3.3V)
    DacbRegs.DACOUTEN.bit.DACOUTEN = 1; // enable the dac module output
    DacbRegs.DACVALS.all = 4096*Vcmd/Vref;

    // initialize adc
    CpuSysRegs.PCLKCR13.bit.ADC_A = 1;  // set adc to on or off
    asm(" NOP");
    asm(" NOP");
    AdcaRegs.ADCCTL2.bit.PRESCALE = 6;      // based off sys clk (200M), 6 -> factor of 4
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;      // enable module power supply
    DelayUs(500); //delay of 500 microseconds

    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 1;    // initiates SoC (timer clock)
    AdcaRegs.ADCSOC1CTL.bit.TRIGSEL = 1;
    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 4;
    AdcaRegs.ADCSOC1CTL.bit.CHSEL = 2;      // input pin (ADCIN2?????????????????????)
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = 39;     // sample and hold duration
    AdcaRegs.ADCSOC1CTL.bit.ACQPS = 39;

    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;
    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 1;
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;    //enable interrupt

    // CPU level
    IER = 1;
    EINT;

    WdRegs.WDCR.all = 0b0101000;    // ENABLE WATCHDOG
    EDIS;                           // Disable changes to protected registers

    while(1) {
        EALLOW;
        WdRegs.WDKEY.all = 0x55;    // FEEDING WATCHDOG
        WdRegs.WDKEY.all = 0xAA;
        EDIS;
    }
}

interrupt void ADCISR(void)
{
    // 1. (Fast) Write an output voltage value to the DAC module.
    DacaRegs.DACVALS.all = 4096*U[cycles]/Vref; // Feeding in the U values to the DAC
    // 2. (Fast) Read an input voltage value from the ADC module.
    Ufilter[cycles] = AdcaResultRegs.ADCRESULT0*Vref/4096;
    // 3. (Slow) Compute and store the next output voltage value.
    temp[0][0] = Ufilter[0];

    temp[0][cycles+1] = temp[0][cycles] + A[0][0]*temp[0][cycles] +  A[0][1]*temp[1][cycles] + A[0][2]*temp[2][cycles] + B[0][0]*Ufilter[cycles];

    Yfilter[cycles] = temp[0][cycles];

    DacbRegs.DACVALS.all = 4096*Yfilter[cycles]/Vref;

    Y[cycles] = AdcaResultRegs.ADCRESULT1*Vref/4096;

    cycles++;
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; // clear the flag
    PieCtrlRegs.PIEACK.all = 1;
}

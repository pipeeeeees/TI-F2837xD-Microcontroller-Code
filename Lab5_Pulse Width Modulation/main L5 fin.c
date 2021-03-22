// David Pipes
// Lab 5 Task 2
// Purpose: PWM
#include "F2837xD_device.h"
#include "F2837xD_pievect.h"
#include "stdlib.h"
#include "math.h"
#define pi 3.14159

interrupt void ADCISR(void);
extern void DelayUs(Uint16);

Uint32 cycles = 0;
Uint32 i = 0;
Uint32 j = 0;
float32 Vref = 3.3;
float32 t;


float32 vo[2000];
float32 vi[2000];
float32 d[2000];
Uint16 dtest[2000];
float32 A = -10000;
float32 B = 10000;
float32 C = 1;
float32 vRes = 0.00165;
float32 V = 3.3;
float32 fpwm = 50000;
float32 T = 0.00002;
float32 GPIOcheck;

void main(void){

    // Code that runs once
    for (i = 0; i < 2000; i++){
        vi[i] = 1.5 + sin(2*pi*500*i*T);
        d[i] = vi[i]/V;
    }

    // START OF WATCHDOG / PROTECTED REGISTER SETTING
    EALLOW;                                        // Allow changes to protected registers
    WdRegs.WDCR.all = 0b1101000;                   // DISABLE WATCHDOG

    // System Clock target = 200MHz             TRM Section 2.7.6.2 (page 115)
    ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL = 0x01; // Set the reference clock: XTAL
    ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 0;        // Bypass the PLL
    ClkCfgRegs.SYSCLKDIVSEL.all = 0b000000;        // divide by 1
    ClkCfgRegs.SYSPLLMULT.all = 0b10100;           // mult by 20 (20 x 10MHz)
    while(ClkCfgRegs.SYSPLLSTS.bit.LOCKS != 1);    // checks if PLL has reached steady state
    ClkCfgRegs.SYSCLKDIVSEL.all = 0b000001;        // set system clock divider +1
    ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 1;        // Switch to PLL as system clock
    ClkCfgRegs.SYSCLKDIVSEL.all = 0b000000;        // change divider to the appropriate value

    // Timer Clock target = 200kHz
    CpuTimer0Regs.PRD.all = 0b1111100111;  //999 (+1, 200M/1k = 200k)
    CpuTimer0Regs.TCR.bit.TSS = 1;          // stops timer if 1
    CpuTimer0Regs.TCR.bit.TRB = 1;          // equals 1 when we want to load timer with PRDH:PRD
    CpuTimer0Regs.TCR.bit.TIE = 1;          // 1 is timer interrupt enabled



    // Initialize ADC
    CpuSysRegs.PCLKCR13.bit.ADC_B = 1;      // set ADCB to on or off
    asm(" NOP");
    asm(" NOP");
    AdcbRegs.ADCCTL2.bit.PRESCALE = 6;      // based off sys clk (200M), 6 -> factor of 4, so it is at 50MHz
    AdcbRegs.ADCCTL1.bit.ADCPWDNZ = 1;      // enable module power supply
    DelayUs(500); //delay of 500 microseconds
    AdcbRegs.ADCSOC0CTL.bit.TRIGSEL = 1;    // initiates SoC (timer clock)
    AdcbRegs.ADCSOC0CTL.bit.CHSEL = 5;      // input pin (ADCINB5) on SOC0 / pin J7-5
    AdcbRegs.ADCSOC0CTL.bit.ACQPS = 39;     // sample and hold duration
    AdcbRegs.ADCCTL1.bit.INTPULSEPOS = 1;   // set position of EOC to end of conversion
    AdcbRegs.ADCINTSEL1N2.bit.INT1SEL = 0;  // select EOC0 to trigger interrupt 1
    AdcbRegs.ADCINTSEL1N2.bit.INT1E = 1;    // enable interrupt

    // PWM
    // Setting pin multiplexer
    GpioCtrlRegs.GPEGMUX2.bit.GPIO158 = 0;
    GpioCtrlRegs.GPEMUX2.bit.GPIO158 = 0b01;
    GpioCtrlRegs.GPEDIR.bit.GPIO158 = 1; // 0 configures pin as input, 1 configures pin as output
    //GpioCtrlRegs.GPEODR.bit.GPIO158 = 1; // explain what this does. 1 is open drain 0 is normal
    //GpioCtrlRegs.GPEPUD.bit.GPIO158 = 0; // 0 enables pullup. 1 disables pullup

    ClkCfgRegs.PERCLKDIVSEL.bit.EPWMCLKDIV = 0x1; // set to 100MHz from CPU clock 200MHz

    CpuSysRegs.PCLKCR2.bit.EPWM7 = 1;         // turn on EPWM7 with 1
    asm(" NOP");
    asm(" NOP");

    // Configure Time Base clock and counter
    EPwm7Regs.TBCTL.bit.CTRMODE = 0b00;       // shape of the periodic signal, 00 is count up mode
    EPwm7Regs.TBPRD = 2000;                   // PRD = V/vRes = 3.3/0.00165 = 2000
    EPwm7Regs.TBCTL.bit.HSPCLKDIV = 0;        // 1 , TBCLK = 100MHz = fclk,pwm / (HSPCLKDIV x CLKDIV)
    EPwm7Regs.TBCTL.bit.CLKDIV = 0;           // 1

    // Set output actions
    EPwm7Regs.AQCTLB.bit.ZRO = 0b10;          // 00 does nothing, 01 sets EPWM low, 10 sets EPWM high, 11 toggles
    EPwm7Regs.AQCTLB.bit.CBU = 0b01;          // 00 does nothing, 01 sets EPWM low, 10 sets EPWM high, 11 toggles

    // Enable time base clock
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;     // sync all modules

    // Load CMPA register
    EPwm7Regs.CMPB.bit.CMPB = (Uint16) (d[cycles]*2000);

    // PIE level
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;
    PieVectTable.ADCB1_INT = &ADCISR;
    PieCtrlRegs.PIEIER1.bit.INTx2 = 1;      //locates interrupt source 1.2 (ADCB1)


    // CPU level
    IER = 1;
    EINT;

    CpuTimer0Regs.TCR.bit.TSS = 0;

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
    EPwm7Regs.CMPB.bit.CMPB = (Uint16) (d[cycles]*2000);
    vo[cycles] = AdcbResultRegs.ADCRESULT0*V/4069;

    cycles++;
    if (cycles == 2000){
        cycles = 0;
    }

    AdcbRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; 
    PieCtrlRegs.PIEACK.all = 1;
}

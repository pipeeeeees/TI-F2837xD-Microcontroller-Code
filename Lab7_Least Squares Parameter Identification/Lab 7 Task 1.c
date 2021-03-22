// David Pipes
// Lab 7

#include "F2837xD_device.h"
//#include "F2837xD_pievect.h"
//#include "stdlib.h"
#include "math.h"

#define choice 5
#include "examples.h"

#define pi 3.14159


float32 temp, maxval = 0, maxrow = 0, soln[N], numerr;
Uint32 i, j ,k;

void solve_system (float32 a[N][N+1], float32 soln[N]){
    for(i = 0 ; i < N ; i++){

        // get pivot
        maxval = fabs(a[i][i]);
        maxrow = i;
        for(k = i+1 ; k < N ; k++){
            temp = fabs(a[k][i]);
            if (temp > maxval){
                maxval = temp;
                maxrow = k;
            }
        }
        if (maxval < 0.000001){
            asm(" ESTOP0");
        }

        // exchange rows
        for(j = i ; j < N+1 ; j++){
            temp = a[(Uint32)maxrow][j];
            a[(Uint32)maxrow][j] = a[i][j];
            a[i][j] = temp;
        }

        // get upper triangular form
        for(k = i+1 ; k < N ; k++){
            temp = -a[k][i]/a[i][i];
            for(j = i ; j < N+1 ; j++){
                if(j == i){
                    a[k][j] = 0;
                }else{
                    a[k][j] = a[k][j]+temp*a[i][j];
                }
            }
        }
    }

    // solve by back substitution
    soln[N-1] = a[N-1][N]/a[N-1][N-1];
    for(i = N-2 ; i >= 0 && i < 600; i--){
        temp = 0;
        for(j = i+1 ; j < N ; j++){
            temp = temp+a[i][j]*soln[j];
        }
        soln[i] = (a[i][N]-temp)/a[i][i];
    }
}


float32 compute_error (float32 x[N], float32 soln[N]){
    temp = 0;
    for(i = 0 ; i < N ; i++){
        temp = temp+(x[i]-soln[i])*(x[i]-soln[i]);
    }
    numerr = sqrt(temp);
    return numerr;
}


void main(void){
    // START OF WATCHDOG / PROTECTED REGISTER SETTING
    EALLOW;                                        // Allow changes to protected registers
    WdRegs.WDCR.all = 0b1101000;                   // DISABLE WATCHDOG

    //--------------------------------------------------
    // System Clock target = 200MHz             TRM Section 2.7.6.2 (page 115)
    ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL = 0x01; // Set the reference clock: XTAL
    ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 0;        // Bypass the PLL
    ClkCfgRegs.SYSCLKDIVSEL.all = 0b000000;        // divide by 1
    ClkCfgRegs.SYSPLLMULT.all = 0b10100;           // mult by 20 (20 x 10MHz)
    while(ClkCfgRegs.SYSPLLSTS.bit.LOCKS != 1);    // checks if PLL has reached steady state
    ClkCfgRegs.SYSCLKDIVSEL.all = 0b000001;        // set system clock divider +1
    ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 1;        // Switch to PLL as system clock
    ClkCfgRegs.SYSCLKDIVSEL.all = 0b000000;        // change divider to the appropriate value
    //--------------------------------------------------


    solve_system(a,soln);
    numerr = compute_error(x,soln);


    WdRegs.WDCR.all = 0b0101000;    // ENABLE WATCHDOG
    EDIS;                           // Disable changes to protected registers


    while(1) {
        EALLOW;
        WdRegs.WDKEY.all = 0x55;    // FEEDING WATCHDOG
        WdRegs.WDKEY.all = 0xAA;
        EDIS;
    }
}

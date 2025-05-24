#include "F2837xD_device.h"
#include "F2837xD_Examples.h"
#include "stdio.h"

void ConfigureEPWM(void);
void ConfigureGPIO(void);

void main(void)
 {
    // Initialize system control
    InitSysCtrl();

    // Disable CPU interrupts
    DINT;

    // Initialize PIE control and clear flags
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();
    ConfigureGPIO();

    //  Configure the ePWM module
    ConfigureEPWM();

    //   Enable global interrupts
    EINT;
    ERTM;

    // Infinite loop to keep PWM running
    while(1){
        uint16_t counter_value = EPwm1Regs.TBCTR;
        printf("TBCTR: %u\n", counter_value);
    }
}

void ConfigureGPIO(void)
{
    EALLOW;
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;  // Set GPIO0 as General-Purpose I/O
    GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;   // Set GPIO0 as Output
    GpioDataRegs.GPACLEAR.bit.GPIO0 = 1; // Initialize GPIO0 to LOW
    EDIS;
}
void ConfigureEPWM(void)
{
    //  Configure ePWM1 for 1 kHz, 50% duty cycle
    EALLOW;

    // Set ePWM clock divider to /1
    CpuSysRegs.PCLKCR2.bit.EPWM1 = 1;

    // Set PWM frequency
    EPwm1Regs.TBPRD = 5000;  // Period = (SYSCLK / (Prescaler * Desired Frequency)) / 2

    // Set phase to zero
    EPwm1Regs.TBPHS.all = 0;

    // Counter mode: Up-down count mode
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
    EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    // Set duty cycle to 50% (High time = TBPRD / 2)
    EPwm1Regs.CMPA.bit.CMPA = 2500;

    // Configure PWM output mode
    EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm1Regs.AQCTLA.bit.CAD = AQ_SET;

    EDIS;
}

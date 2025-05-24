#include "F2837xD_device.h"
#include "F2837xD_Examples.h"
#include "stdio.h"

// Function prototypes
void InitTimer0(void);
__interrupt void Timer0_ISR(void);

void main(void)
{

    InitSysCtrl();


    EALLOW;
    GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;  // Set GPIO34 as GPIO
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;   // Set GPIO34 as output
    EDIS;


    DINT;                   // Disable CPU interrupts
    InitPieCtrl();           // Initialize PIE control registers
    IER = 0x0000;           // Clear CPU interrupt flags
    IFR = 0x0000;           // Clear CPU interrupt flag register


    InitPieVectTable();
    EALLOW;
    PieVectTable.TIMER0_INT = &Timer0_ISR; // Map Timer0 ISR to vector table
    EDIS;


    InitTimer0();


    IER |= M_INT1;               // Enable CPU interrupt group 1 (Timer0 belongs here)
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1; // Enable Timer0 interrupt in PIE group 1
    EINT;                        // Enable Global Interrupt (INTM)
    ERTM;                        // Enable Real-Time Debug mode


    CpuTimer0Regs.TCR.bit.TSS = 0; // Start Timer0

    while(1)
    {
        //  Idle
    }
}

// Timer0 Configuration Function
void InitTimer0(void)
{
    // Set up CPU Timer0 for 1-second interrupt
    ConfigCpuTimer(&CpuTimer0, 200, 1000000); // CPU Clock = 200 MHz, Period = 1 second

    EALLOW;
    CpuTimer0Regs.TCR.bit.TSS = 1; // Stop timer before configuration
    CpuTimer0Regs.TCR.bit.TRB = 1; // Reload Timer
    CpuTimer0Regs.TCR.bit.TIE = 1; // Enable Timer Interrupt
    EDIS;
}

// Timer0 ISR (Interrupt Service Routine)
__interrupt void Timer0_ISR(void)
{
    printf("\n in interrupt");
    GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1; // Toggle LED on GPIO34

    // Clear Timer0 interrupt flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

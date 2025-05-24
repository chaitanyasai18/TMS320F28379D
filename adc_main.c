#include "F2837xD_device.h"
#include "F2837xD_Examples.h"
#include "stdio.h"

void ConfigureADC(void);
void ConfigureEPWM(void);
void ConfigureADCSOC(void);

void main(void)
{
    InitSysCtrl(); // Initialize system control
    InitGpio();    // Initialize GPIO
    DINT;          // Disable CPU interrupts

    InitPieCtrl(); // Initialize PIE control registers
    IER = 0x0000;  // Clear CPU interrupt register
    IFR = 0x0000;  // Clear CPU interrupt flag register

    InitPieVectTable(); // Initialize PIE vector table
    EALLOW;
    GpioCtrlRegs.GPADIR.bit.GPIO0 = 0; // Set GPIO0 (ADCINA0) as input
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0; // Ensure it’s in ADC mode
    EDIS;
    ConfigureADC();     // Configure ADC
    ConfigureADCSOC();  // Configure ADC SOC

    EINT;  // Enable Global Interrupts
    ERTM;  // Enable real-time mode

    while(1)
    {
        AdcaRegs.ADCSOCFRC1.bit.SOC0 = 1; // Start conversion
        while(AdcaRegs.ADCINTFLG.bit.ADCINT1 == 0); // Wait for conversion completion
        AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; // Clear interrupt flag
        uint16_t result = AdcaResultRegs.ADCRESULT0; // Read ADC result

        printf("ADC Result: %u\n", result); // Print digital count
       // DELAY_US(1000000); // Delay for stability
    }
}

void ConfigureADC(void)
{
    EALLOW;
    AdcaRegs.ADCCTL2.bit.PRESCALE = 6;  // Set ADCCLK divider
    AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;  // Power up ADC
    //DELAY_US(1000);
    EDIS;
}

void ConfigureADCSOC(void)
{
    EALLOW;
    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 0; // Select channel ADCINA0
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = 14; // Set sampling window
    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 0; // Software trigger
    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0; // End of SOC0 will trigger ADCINT1
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1; // Enable interrupt
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; // Clear flag
    EDIS;
}

#include "F2837xD_device.h"
#include "F2837xD_Examples.h"
#include "stdio.h"

// Function declarations
void InitSCI(void);
void SendData(uint16_t data);
uint16_t ReceiveData(void);
void InitLED(void);
void ToggleLED(int success);

void main(void)
{
    InitSysCtrl();  // Initialize system control (clock, PLL, etc.)
    InitGpio();     // Initialize GPIO pins
    InitLED();      // Initialize LED for status indication
    DINT;           // Disable CPU interrupts
    InitPieCtrl();  // Initialize the PIE control registers
    IER = 0x0000;   // Clear interrupt enable register
    IFR = 0x0000;   // Clear interrupt flag register
    InitPieVectTable(); // Initialize the PIE vector table

    InitSCI();  // Initialize SCI module
    int i;
    uint16_t txData = 'A';  // Character to be sent through SCI
    SendData(txData);  // Transmit data

//    DELAY_US(1000);  // Small delay for transmission to complete
    for(i=0;i<10;i++);
    uint16_t rxData = ReceiveData();  // Receive data from SCI

    printf("\n Transmitted data: %hu",txData);
    printf("\n Received data: %hu",rxData);

    if(txData == rxData)
    {
        // If transmitted and received data match, indicate success with LED
        ToggleLED(1);
        printf("\n Loop back test Successful"); //sending success message to console
    }
    else
    {
        // If data mismatch, indicate failure with LED
        ToggleLED(0);
        printf("\n Loop back test Failed");  //sending failure message to console
    }

    while(1);  // Infinite loop to keep the program running
}

void InitSCI(void)
{
    EALLOW;
    GpioCtrlRegs.GPCMUX2.bit.GPIO84 = 1;  // Configure GPIO84 as SCIA TX
    GpioCtrlRegs.GPCMUX2.bit.GPIO85 = 1;  // Configure GPIO85 as SCIA RX
    EDIS;

    SciaRegs.SCICCR.all = 0x0007;  // 1 stop bit, no parity, 8-bit character format
    SciaRegs.SCICTL1.all = 0x0003; // Enable TX, RX functionalities
    SciaRegs.SCICTL1.bit.SWRESET = 1; // Release SCI from reset to begin operation

    // Set baud rate assuming a 200MHz system clock
    SciaRegs.SCILBAUD.bit.BAUD = 0x00C2;  // Low byte of baud rate register (115200 baud)
    SciaRegs.SCIHBAUD.bit.BAUD = 0;// High byte of baud rate register

    SciaRegs.SCICCR.bit.LOOPBKENA = 1; // Enable Loopback mode (internally connects TX to RX)
}

void SendData(uint16_t data)
{
    while(SciaRegs.SCICTL2.bit.TXRDY == 0); // Wait until TX buffer is ready
    SciaRegs.SCITXBUF.all = data; // Load data into transmit buffer
}

uint16_t ReceiveData(void)
{
    while(SciaRegs.SCIRXST.bit.RXRDY == 0); // Wait until data is received
    return SciaRegs.SCIRXBUF.all; // Read received data from buffer
}

void InitLED(void)
{
    EALLOW;
    GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;  // Set GPIO31 as a general-purpose output pin
    GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;   // Configure GPIO31 as an output
    EDIS;
}

void ToggleLED(int success)
{
    if (success)
        GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1; // Toggle LED state to indicate success
    else
        GpioDataRegs.GPACLEAR.bit.GPIO31 = 1;  // Turn off LED to indicate failure
}

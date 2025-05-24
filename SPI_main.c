#include "F28x_Project.h"

#define STRING_LENGTH 16
char txString[] = "Hello, SPI-B!";
char rxAck[4] = {0};  // To receive ACK

void InitSPI_A_Master(void);
void InitSPI_B_Slave(void);

void main(void)
{
    Uint16 i;
    int j;
    char receivedData[STRING_LENGTH] = {0};

    InitSysCtrl();          // Initialize System Control
    InitGpio();             // Initialize GPIO
    DINT;
    InitPieCtrl();
    IER=0x0000;
    IFR=0x0000;
    InitPieVectTable();
    InitSPI_A_Master();     // Initialize SPI-A as Master
    InitSPI_B_Slave();      // Initialize SPI-B as Slave

   // DELAY_US(1000);         // Small delay to stabilize SPI setup
    for(j=0;j<10;j++);

    // -------------------- Transmission from Master to Slave ---------------------
    for (i = 0; i < STRING_LENGTH && txString[i] != '\0'; i++)
    {
        SpiaRegs.SPITXBUF = txString[i];  // Send data byte
        while(SpiaRegs.SPISTS.bit.INT_FLAG == 0) {} // Wait for transmission complete
        (void)SpiaRegs.SPIRXBUF;  // Clear RX buffer (junk data in master)
    }

    // -------------------- Slave Receives Data ---------------------
    for (i = 0; i < STRING_LENGTH && txString[i] != '\0'; i++)
    {
        while(SpibRegs.SPISTS.bit.INT_FLAG == 0) {} // Wait until data is received
        receivedData[i] = SpibRegs.SPIRXBUF; // Read data from master
    }

    // -------------------- Master Receives ACK ---------------------
    SpibRegs.SPITXBUF = 'A';  // Prepare for ACK response (example: send 'A' for "ACK")
    while(SpibRegs.SPISTS.bit.INT_FLAG == 0);
    SpiaRegs.SPITXBUF = 0xFFFF;  // Send dummy data to generate clock for receiving ACK
    while(SpiaRegs.SPISTS.bit.INT_FLAG == 0) {}  // Wait until data received
    rxAck[i] = SpiaRegs.SPIRXBUF;  // Read ACK data

    while(1);  // End of communication
}

// ---------------------- SPI-A Master Initialization ----------------------
void InitSPI_A_Master(void)
{
    EALLOW;
    GpioCtrlRegs.GPBGMUX2.bit.GPIO54 = 0x00; // SPI-A SIMO
    GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 0x01;
    GpioCtrlRegs.GPBGMUX2.bit.GPIO55 = 0x00; // SPI-A SOMI
    GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 0x01;
    GpioCtrlRegs.GPBGMUX2.bit.GPIO56 = 0x00; // SPI-A CLK
    GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 0x01;
    GpioCtrlRegs.GPBGMUX2.bit.GPIO57 = 0x00; // SPI-A STE
    GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 0x01;

    EALLOW;
    GpioCtrlRegs.GPBPUD.bit.GPIO54 = 0;  // Enable pull-up on SPISIMO-A
    GpioCtrlRegs.GPBPUD.bit.GPIO55 = 0;  // Enable pull-up on SPISOMI-A
    GpioCtrlRegs.GPBPUD.bit.GPIO56 = 0;  // Enable pull-up on SPICLK-A
    GpioCtrlRegs.GPBPUD.bit.GPIO57 = 0;  // Enable pull-up on SPISTE-A
    EDIS;


    SpiaRegs.SPICCR.bit.SPISWRESET = 0;  // Put SPI in reset
    SpiaRegs.SPICCR.all = 0x004F;        // 16-bit char bits, reset enabled
    SpiaRegs.SPICTL.all = 0x000F;        // Enable master, phase, polarity
    SpiaRegs.SPIBRR.all = 0x007F;        // Baud rate
    SpiaRegs.SPICCR.bit.SPISWRESET = 1;  // Enable SPI
    EDIS;
}

// ---------------------- SPI-B Slave Initialization ----------------------
void InitSPI_B_Slave(void)
{
    EALLOW;
    GpioCtrlRegs.GPAGMUX2.bit.GPIO24 = 0x01; // SPI-B SIMO
    GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 0x10;
    GpioCtrlRegs.GPAGMUX2.bit.GPIO25 = 0x01; // SPI-B SOMI
    GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 0x10;
    GpioCtrlRegs.GPAGMUX2.bit.GPIO26 = 0x01; // SPI-B CLK
    GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 0x10;
    GpioCtrlRegs.GPAGMUX2.bit.GPIO27 = 0x01; // SPI-B STE
    GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 0x10;

    SpibRegs.SPICCR.bit.SPISWRESET = 0;  // Put SPI in reset
    SpibRegs.SPICCR.all = 0x004F;        // 16-bit char bits, reset enabled
    SpibRegs.SPICTL.all = 0x000B;        // Enable slave, phase, polarity
    SpibRegs.SPIBRR.all = 0x007F;        // Baud rate (ignored in slave mode but required to write)
    SpibRegs.SPICCR.bit.SPISWRESET = 1;  // Enable SPI
    EDIS;
}

#include "F2837xD_device.h"
#include "F2837xD_Examples.h"

void init_GPIO();
void main(void)
{
    int i;
    InitSysCtrl();
    InitGpio();
    DINT;
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();
    init_GPIO();

    while(1){
        GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;
       // DELAY_US(1000000);
        for(i=0;i<10;i++);
    }
}
void init_GPIO(void){
    EALLOW;
    GpioCtrlRegs.GPAGMUX2.bit.GPIO31 = 0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO31 = 0;
    EDIS;
}

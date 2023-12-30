#include "bgMeter.hpp"

void bgMeterWrite(SpiHandle &spi_handle, uint8_t controlSig)
{
    uint8_t BCr = 0x0F; //set the global brightness value for each color
    uint8_t BCg = 0x0F;
    uint8_t BCb = 0x0F;
    uint32_t command; //holds the 32 bit write command and static chip settings

    // Magic word for write
    command = 0x25;

    command <<= 5;
    // OUTTMG = 1, EXTGCK = 0, TMGRST = 1, DSPRPT = 1, BLANK = 0 -> 10110 ->0x16
    command |= 0x16;

    command <<= 7;
    command |= BCb;

    command <<= 7;
    command |= BCg;

    command <<= 7;
    command |= BCr;

    //noInterrupts();

    /*_spi_dev->beginTransaction();

        _spi_dev->transfer(command >> 24);
        _spi_dev->transfer(command >> 16);
        _spi_dev->transfer(command >> 8);
        _spi_dev->transfer(command);
    */

    uint8_t cmdBfr[4];
    cmdBfr[0] = command >> 24;
    cmdBfr[1] = command >> 16;
    cmdBfr[2] = command >> 8;
    cmdBfr[3] = command >> 0;

    spi_handle.BlockingTransmit(cmdBfr, 4);

    uint8_t pwmBfr[2]; //container for the 16bit LED PWM value for each of the 12 channels

    uint8_t ledCv = controlSig;

/*
    static uint32_t bgPrintTimer = hw.system.GetNow();
    if ((hw.system.GetNow() - bgPrintTimer) > 1000)
    {
        hw.PrintLine("%d\n", ledCv);
        bgPrintTimer = hw.system.GetNow();
    }
*/ //debug print
    
    uint8_t remainingLeds = (12 - ledCv);

        for (int8_t c = remainingLeds; c > 0; c--){
            pwmBfr[0] = 0x00;
            pwmBfr[1] = 0x00;
            
            spi_handle.BlockingTransmit(pwmBfr, 2);
        }
        for (int8_t cv = ledCv; cv >= 0; cv--) {
        // 16 bits per channel, send MSB first
            pwmBfr[0] = 0x7F;
            pwmBfr[1] = 0x7F;
        
            spi_handle.BlockingTransmit(pwmBfr, 2);

        //_spi_dev->transfer(pwmbuffer[12 + c] >> 8);
        //_spi_dev->transfer(pwmbuffer[12 + c]);
        }


    System::DelayUs(200);
    //_spi_dev->endTransaction();
}

void shapeIndicatorWrite(SpiHandle &spi_handle, uint8_t controlSig)
{
    uint8_t BCr = 0x0F; //set the global brightness value for each color
    uint8_t BCg = 0x0F;
    uint8_t BCb = 0x0F;
    uint32_t command; //holds the 32 bit write command and static chip settings

    // Magic word for write
    command = 0x25;

    command <<= 5;
    // OUTTMG = 1, EXTGCK = 0, TMGRST = 1, DSPRPT = 1, BLANK = 0 -> 10110 ->0x16
    command |= 0x16;

    command <<= 7;
    command |= BCb;

    command <<= 7;
    command |= BCg;

    command <<= 7;
    command |= BCr;

    //noInterrupts();

    /*_spi_dev->beginTransaction();

        _spi_dev->transfer(command >> 24);
        _spi_dev->transfer(command >> 16);
        _spi_dev->transfer(command >> 8);
        _spi_dev->transfer(command);
    */

    uint8_t cmdBfr[4];
    cmdBfr[0] = command >> 24;
    cmdBfr[1] = command >> 16;
    cmdBfr[2] = command >> 8;
    cmdBfr[3] = command >> 0;

    spi_handle.BlockingTransmit(cmdBfr, 4);

    uint8_t pwmBfr[2]; //container for the 16bit LED PWM value for each of the 12 channels


/*
    static uint32_t bgPrintTimer = hw.system.GetNow();
    if ((hw.system.GetNow() - bgPrintTimer) > 1000)
    {
        hw.PrintLine("%d\n", ledCv);
        bgPrintTimer = hw.system.GetNow();
    }
*/ //debug print

            pwmBfr[0] = 0x00;
            pwmBfr[1] = 0x00;

            spi_handle.BlockingTransmit(pwmBfr, 2);   
            spi_handle.BlockingTransmit(pwmBfr, 2);                

        for (int8_t c = 4; c >= 0; c--){
            if (c - controlSig)
            {
                pwmBfr[0] = 0x00;
                pwmBfr[1] = 0x00;
            }
            else
            {
                pwmBfr[0] = 0x7F;
                pwmBfr[1] = 0x7F;
            }
            spi_handle.BlockingTransmit(pwmBfr, 2);
        spi_handle.BlockingTransmit(pwmBfr, 2);
        }


    System::DelayUs(200);
    //_spi_dev->endTransaction();
}
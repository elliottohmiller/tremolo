/*
These TLC59711 write functions take heavy inspiration from Adafruit's https://github.com/adafruit/Adafruit_TLC59711
Adafruit's license is included below: 

Software License Agreement (BSD License)

Copyright (c) 2012, Adafruit Industries
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holders nor the
names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

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

    uint8_t cmdBfr[4];
    cmdBfr[0] = command >> 24;
    cmdBfr[1] = command >> 16;
    cmdBfr[2] = command >> 8;
    cmdBfr[3] = command >> 0;

    spi_handle.BlockingTransmit(cmdBfr, 4);

    uint8_t pwmBfr[2]; //container for the 16bit LED PWM value for each of the 12 channels

    uint8_t ledCv = controlSig;
    
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

        }


    System::DelayUs(200);
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

    uint8_t cmdBfr[4];
    cmdBfr[0] = command >> 24;
    cmdBfr[1] = command >> 16;
    cmdBfr[2] = command >> 8;
    cmdBfr[3] = command >> 0;

    spi_handle.BlockingTransmit(cmdBfr, 4);

    uint8_t pwmBfr[2]; //container for the 16bit LED PWM value for each of the 12 channels

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
}
#include "main.hpp"
#include "daisy_seed.h"
#include "daisysp.h"
//#include "reusable/console.h" ->added to libDaisy
#include <stm32h7xx_hal.h> //for HAL_Delay()
#include "per/spi.h"
#include <cmath> //for round();
#include "bgMeter.hpp"

using namespace daisy;
using namespace daisysp;

#define PWMTIMER_K 2500

uint32_t pwmTimer = System::GetNow();
uint32_t bgTimer = System::GetNow();
MeterState bgMeterState = MeterState::tremoloCV;

void AudioCallback(daisy::AudioHandle::InputBuffer  in,
                   daisy::AudioHandle::OutputBuffer out,
                   size_t                    size)
{

    for(size_t i = 0; i < size; i++)
    {
        float nextSample = in[0][i];
        out[0][i] = trem.Process(nextSample);
    }
}

void rateValChange(paramEncoder &rate, MeterState& bgMeterState, uint32_t& timer)
{
    rate.enc->Debounce();

    switch (rate.enc->Increment())
    {
        case -1: 
        if (rate.value > 1)
        {
            --rate.value;
            if ((rate.value%2))
            {
                hw.PrintLine("rate decreased to: %u.5Hz\n", rate.value / 2);
            }
            else
            {
                hw.PrintLine("rate decreased to: %uHz\n", rate.value / 2);
            }
        }
            bgMeterState = MeterState::rateCV;
            timer = System::GetNow() + BGTIMER_K;
        break;

        case +1: 
        if (rate.value < 20.0)
        {
            ++rate.value;
            if ((rate.value%2))
            {
                hw.PrintLine("rate increased to: %u.5Hz\n", rate.value / 2);
            }
            else
            {
                hw.PrintLine("rate increased to: %uHz\n", rate.value / 2);
            }
        }
        bgMeterState = MeterState::rateCV;
        timer = System::GetNow() + BGTIMER_K;
        break;

        default: 
        break;
    }

}

void depthValChange(paramEncoder &depth, MeterState& bgMeterState, uint32_t& timer)
{
    depth.enc->Debounce();
    switch (depth.enc->Increment())
    {
        case -1:
        if (depth.value > 0)
        {
            --depth.value;
            hw.PrintLine("depth decreased to: %u0%%\n", depth.value);
        }
        bgMeterState = MeterState::depthCV;
        timer = System::GetNow() + BGTIMER_K;
        
        break;

        case +1: 
        if (depth.value < 10)
        {
            ++depth.value;
            hw.PrintLine("depth increased to: %u0%%\n", depth.value);
        }
        bgMeterState = MeterState::depthCV;
        timer = System::GetNow() + BGTIMER_K;
        break;

        default: 
        break;
    }

}

void printShape(paramEncoder &shape)
{
    switch (shape.value)
    {
        case 0: hw.PrintLine("shape: sine\n");
        break;
        case 1: hw.PrintLine("shape: triangle\n");
        break;
        case 2: hw.PrintLine("shape: saw\n");
        break;
        case 3: hw.PrintLine("shape: ramp\n");
        break;
        case 4: hw.PrintLine("shape: square\n");
        break;
/*
        case 5: hw.PrintLine("shape: poly-tri\n");
        break;
        case 6: hw.PrintLine("shape: poly-saw\n");
        break;
        case 7: hw.PrintLine("shape: poly-square\n");
        break;
*/
        default: hw.PrintLine("something went wrong...\n");
        break;
    }
}

void shapeValChange(paramEncoder &shape, MeterState& bgMeterState, uint32_t& timer)
{
    shape.enc->Debounce();
    switch (shape.enc->Increment())
    {
        case -1: 
        if (shape.value > 0)
        {
            --shape.value;
            printShape(shape);
        }
        bgMeterState = MeterState::shapeCV; 
        timer = System::GetNow() + BGTIMER_K;
        break;

        case +1: 
        if (shape.value < 4)
        {
            ++shape.value;
            printShape(shape);
        }
        bgMeterState = MeterState::shapeCV; 
        timer = System::GetNow() + BGTIMER_K;
        break;

        default: 
        break;
    }
}

void pwmValChange(paramEncoder &pwm, MeterState& bgMeterState, uint32_t& bgtimer, uint32_t& pwmTimer)
{
    if (tremPwm.button)
    {
        bgMeterState = MeterState::pwmCV; 
        bgtimer = System::GetNow() + PWMTIMER_K;
        pwmTimer = System::GetNow() + PWMTIMER_K;
    }

    pwm.enc->Debounce();
    switch (pwm.enc->Increment())
    {
        case -1: 
        if (pwm.value > 1)
        {
            --pwm.value;
            hw.PrintLine("pwm decreased to: %u0%%\n", pwm.value); 
        }
        bgMeterState = MeterState::pwmCV; 
        bgtimer = System::GetNow() + PWMTIMER_K;
        pwmTimer = System::GetNow() + PWMTIMER_K;

        break;

        case +1: 
        if (pwm.value < 9)
        {
            ++pwm.value;
            hw.PrintLine("pwm increased to: %u0%%\n", pwm.value);
        }
        bgMeterState = MeterState::pwmCV; 
        bgtimer = System::GetNow() + PWMTIMER_K;
        pwmTimer = System::GetNow() + PWMTIMER_K;
        break;

        default: 
        break;
    }
}

void spiConfig(SpiHandle::Config &spi_conf)
{
    spi_conf.periph = SpiHandle::Config::Peripheral::SPI_1;
    spi_conf.mode = SpiHandle::Config::Mode::MASTER;
    spi_conf.direction = SpiHandle::Config::Direction::TWO_LINES_TX_ONLY;
    spi_conf.nss = SpiHandle::Config::NSS::SOFT;
    spi_conf.pin_config.sclk = seed::D8;
    spi_conf.pin_config.miso = Pin();
    spi_conf.pin_config.mosi = seed::D10;
    spi_conf.pin_config.nss = Pin();
}

uint8_t getCV(MeterState& bgMeterState, paramValues& cv,  Tremolo trem)
{
    switch(bgMeterState)
    {
        case MeterState::tremoloCV: return (static_cast<uint8_t>((trem.Process(1) + CV_BIAS) * TREM_CV_SCALER));
        case MeterState::rateCV:   return *cv.rateValue / 2;
        case MeterState::depthCV:  return *cv.depthValue;
        case MeterState::shapeCV:  return *cv.shapeValue;
        case MeterState::pwmCV:    return *cv.pwmValue;
        default : return 0;
    }   
}

void pwmModeSwitch()
{
    if  (tremShape.enc->FallingEdge() && System::GetNow() > pwmTimer)
    {
        tremPwm.button = 1;
        pwmValChange(tremPwm, bgMeterState, bgTimer, pwmTimer);
        hw.PrintLine("mode: %u\n", tremPwm.button);
        tremPwm.button = 0;
    }
    else if (tremShape.enc->FallingEdge() && (System::GetNow() < pwmTimer))
    {
        pwmTimer = 0;
        bgTimer = 0;
        hw.PrintLine("mode: %u\n", tremPwm.button);
        bgMeterState = MeterState::tremoloCV;
    }
}

void setTrem(Tremolo& trem, paramValues& tremEncoders)
{
    trem.SetFreq(*tremEncoders.rateValue / 2.f);
    trem.SetDepth(*tremEncoders.depthValue / 10.f);
    trem.SetWaveform(*tremEncoders.shapeValue);
    trem.SetPw(*tremEncoders.pwmValue / 10.f);
}



int main(void)
{

    hw.Init(); //initialize the Daisy Seed hardware
    spiConfig(spi_conf);
    spi_handle.Init(spi_conf);

    hw.usb_handle.Init(UsbHandle::FS_INTERNAL);
    HAL_Delay(1000); //allow serial communication setup time before first console message prints

	ConsoleInit(); //initialize the CLI

    trem.Init(hw.AudioSampleRate());

    rateEnc.Init(seed::D15, seed::D16, Pin());
    depthEnc.Init(seed::D17, seed::D18, Pin());
    shape_pwmEnc.Init(seed::D19, seed::D20, seed::D21);

    hw.StartAudio(AudioCallback);

    while(1) {

        ConsoleProcess();
        System::Delay(1);
        setTrem(trem, encoderValues);
        //update control values
        pwmModeSwitch();
        rateValChange(tremRate, bgMeterState, bgTimer);
        depthValChange(tremDepth, bgMeterState, bgTimer);

        if ((System::GetNow() < pwmTimer)) //if pwmTimer hasn't elapsed, we're in pwm mode
        {
            pwmValChange(tremPwm, bgMeterState, bgTimer, pwmTimer);
        }
        else 
        {
            tremPwm.button = 0;
            shapeValChange(tremShape, bgMeterState, bgTimer);
        }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        if (System::GetNow() > bgTimer) //bgTimer has elapsed; display trem CV on meter
        {
            bgMeterState = MeterState::tremoloCV;
        }

        if (bgMeterState == MeterState::shapeCV) //display shape indicator
        {
            shapeIndicatorWrite(spi_handle, getCV(bgMeterState, encoderValues, trem)); //call specific write function
        }
        else //display another value indicator 
        {
            bgMeterWrite(spi_handle, getCV(bgMeterState, encoderValues, trem));
        }



	}
}

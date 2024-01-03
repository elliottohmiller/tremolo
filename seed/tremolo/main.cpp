#include "main.hpp"
#include "daisy_seed.h"
#include "daisysp.h"
//#include "reusable/console.h" ->added to libDaisy
#include <stm32h7xx_hal.h> //for HAL_Delay()
#include "per/spi.h"
#include <cmath> //for round();
#include "bgMeter.hpp"
#include "interrupt.hpp"

using namespace daisy;
using namespace daisysp;
    
#define PWMTIMER_K 2500

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

void rateValChange(paramEncoder &rate, MeterControl& meterController)
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
        meterController.bgMeterState = MeterState::rateCV;
        meterController.bgTimer = System::GetNow() + BGTIMER_K;
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
        meterController.bgMeterState = MeterState::rateCV;
        meterController.bgTimer = System::GetNow() + BGTIMER_K;
        break;

        default: 
        break;
    }

}

void depthValChange(paramEncoder &depth, MeterControl& meterController)
{
    depth.enc->Debounce();
    switch (depth.enc->Increment())
    {
        case -1:
        if (depth.value > 1)
        {
            --depth.value;
            hw.PrintLine("depth decreased to: %u0%%\n", depth.value);
        }
        meterController.bgMeterState = MeterState::depthCV;
        meterController.bgTimer = System::GetNow() + BGTIMER_K;
        
        break;

        case +1: 
        if (depth.value < 10)
        {
            ++depth.value;
            hw.PrintLine("depth increased to: %u0%%\n", depth.value);
        }
        meterController.bgMeterState = MeterState::depthCV;
        meterController.bgTimer = System::GetNow() + BGTIMER_K;
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

void shapeValChange(paramEncoder &shape, MeterControl& meterController)
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
        meterController.bgMeterState = MeterState::shapeCV; 
        meterController.bgTimer = System::GetNow() + BGTIMER_K;
        break;

        case +1: 
        if (shape.value < 4)
        {
            ++shape.value;
            printShape(shape);
        }
        meterController.bgMeterState = MeterState::shapeCV; 
        meterController.bgTimer = System::GetNow() + BGTIMER_K;
        break;

        default: 
        break;
    }
}

void pwmValChange(encoderSet &tremEncoders, MeterControl& meterController)
{
    if (tremEncoders.pwm->button)
    {
        meterController.bgMeterState = MeterState::pwmCV; 
        meterController.bgTimer = System::GetNow() + PWMTIMER_K;
        meterController.pwmTimer = System::GetNow() + PWMTIMER_K;
    }
    if (System::GetNow() < meterController.pwmTimer)
    {
        tremEncoders.pwm->enc->Debounce();
        switch (tremEncoders.pwm->enc->Increment())
        {
            case -1: 
            if (tremEncoders.pwm->value > 1)
            {
                --tremEncoders.pwm->value;
                hw.PrintLine("pwm decreased to: %u0%%\n", tremEncoders.pwm->value); 
            }
            meterController.bgMeterState = MeterState::pwmCV; 
            meterController.bgTimer = System::GetNow() + PWMTIMER_K;
            meterController.pwmTimer = System::GetNow() + PWMTIMER_K;

            break;

            case +1: 
            if (tremEncoders.pwm->value < 9)
            {
                ++tremEncoders.pwm->value;
                hw.PrintLine("pwm increased to: %u0%%\n", tremEncoders.pwm->value);
            }
            meterController.bgMeterState = MeterState::pwmCV; 
            meterController.bgTimer = System::GetNow() + PWMTIMER_K;
            meterController.pwmTimer = System::GetNow() + PWMTIMER_K;
            break;

            default: 
            break;
        }
    }
    else 
    {
        tremEncoders.pwm->button = 0;
        shapeValChange(*tremEncoders.shape, meterController);
    }
}

void pwmModeSwitch(encoderSet& tremEncoders, MeterControl& meterController)
{
    if  (tremEncoders.shape->enc->FallingEdge() && System::GetNow() > meterController.pwmTimer)
    {
        tremEncoders.pwm->button = 1;
        pwmValChange(tremEncoders, meterController);
        hw.PrintLine("mode: %u\n", tremEncoders.pwm->button);
        tremEncoders.pwm->button = 0;
    }
    else if (tremEncoders.shape->enc->FallingEdge() && (System::GetNow() < meterController.pwmTimer))
    {
        meterController.pwmTimer = 0;
        meterController.bgTimer = 0;
        hw.PrintLine("mode: %u\n", tremPwm.button);
        meterController.bgMeterState = MeterState::tremoloCV;
    }
}

void setTrem(Tremolo& trem, paramValues& tremEncoders)
{
    trem.SetFreq(*tremEncoders.rateValue / 2.f);
    trem.SetDepth(*tremEncoders.depthValue / 10.f);
    trem.SetWaveform(*tremEncoders.shapeValue);
    trem.SetPw(*tremEncoders.pwmValue / 10.f);
}

void readControls(encoderSet& tremEncoders, MeterControl& meterController)
{
    pwmModeSwitch(tremEncoders, meterController);
    rateValChange(*tremEncoders.rate, meterController);
    depthValChange(*tremEncoders.depth, meterController);
    pwmValChange(tremEncoders, meterController);
}

void updateDisplay(SpiHandle& spi_handle, MeterControl& meterController, paramValues& encoderValues, const Tremolo& trem)
{
    if (System::GetNow() > meterController.bgTimer) //bgTimer has elapsed; display trem CV on meter
    {
        meterController.bgMeterState = MeterState::tremoloCV;
    }

    if (meterController.bgMeterState == MeterState::shapeCV) //display shape indicator
    {
        shapeIndicatorWrite(spi_handle, getCV(meterController.bgMeterState, encoderValues, trem)); //call specific write function
    }
    else //display another value indicator 
    {
        bgMeterWrite(spi_handle, getCV(meterController.bgMeterState, encoderValues, trem));
    }
}

void gpioInterruptCallback()
{
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0);
}

int main(void)
{

    hw.Init(); //initialize the Daisy Seed hardware
    spiConfig(spi_conf);
    spi_handle.Init(spi_conf);
    MeterControl meterController;

    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    std::function<void(void)> gpio_callback = gpioInterruptCallback;

    stm32_interrupt_enable(GPIOA, GPIO_PIN_1, gpio_callback, GPIO_MODE_IT_FALLING);

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

        //update control values
        readControls(tremEncoders, meterController);

        //read and set trem values from encoders
        setTrem(trem, encoderValues); 

        //update display
        updateDisplay(spi_handle, meterController, encoderValues, trem);

	}
}

#ifndef MAIN_HPP
#define MAIN_HPP

#include "daisy_seed.h"
#include "daisysp.h"
//#include "reusable/console.h" ->added to libDaisy
#include <stm32h7xx_hal.h> //for HAL_Delay()
#include "per/spi.h"
#include <cmath> //for round();

using namespace daisy;
using namespace daisysp;

DaisySeed hw;
Tremolo trem;
Encoder rateEnc, depthEnc, shape_pwmEnc;
//BGMeter meter; placeholder for LED BG Meter handle
SpiHandle spi_handle;
SpiHandle::Config spi_conf;

GPIO_TypeDef myButton;

struct paramEncoder
{
    Encoder* enc;
    uint8_t name[10];
    uint8_t value;
    volatile bool button;
};

paramEncoder tremRate{&rateEnc, "rate", 1};
paramEncoder tremDepth{&depthEnc, "depth", 10};
paramEncoder tremShape{&shape_pwmEnc, "shape", 0};
paramEncoder tremPwm{&shape_pwmEnc, "pwm", 5, 0};

typedef struct
{
    paramEncoder* rate = &tremRate;
    paramEncoder* depth = &tremDepth;
    paramEncoder* shape = &tremShape;
    paramEncoder* pwm = &tremPwm;

} encoderSet;

typedef struct
{
     uint8_t* rateValue = &tremRate.value;
     uint8_t* depthValue = &tremDepth.value;
     uint8_t* shapeValue = &tremShape.value;
     uint8_t* pwmValue = &tremPwm.value;
} paramValues;

encoderSet tremEncoders;

paramValues encoderValues;


#endif
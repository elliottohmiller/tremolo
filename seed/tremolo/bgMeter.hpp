#ifndef BGMETER_HPP
#define BGMETER_HPP

#include "per/spi.h"
#include "daisy_seed.h"
#include "daisysp.h"

#define CV_BIAS .045
#define TREM_CV_SCALER 10
#define BGTIMER_K 1000;

using namespace daisy;
using namespace daisysp;

void bgMeterWrite(SpiHandle &spi_handle, uint8_t controlSig);
void shapeIndicatorWrite(SpiHandle &spi_handle, uint8_t controlSig);

enum MeterState
{
    tremoloCV,
    rateCV,
    depthCV,
    shapeCV,
    pwmCV,

    maxMeterState
};

typedef struct
{
    uint32_t pwmTimer = System::GetNow();
    uint32_t bgTimer = System::GetNow();
    MeterState bgMeterState = MeterState::tremoloCV;
} MeterControl;

#endif
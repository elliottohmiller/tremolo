#ifndef TREM_CONTROLS_HPP
#define TREM_CONTROLS_HPP

#include "Effects/tremolo.h"
#include "encoder.h"
#include "bgMeter.hpp"

struct paramEncoder
{
    Encoder* enc;
    uint8_t name[10];
    uint8_t value;
    MeterState meterState;
};

bool pwmSwitch;

void rateValChange(paramEncoder &rate, MeterState& bgMeterState, uint32_t& timer);
void depthValChange(paramEncoder &depth, MeterState& bgMeterState, uint32_t& timer);
void printShape(paramEncoder &shape);
void shapeValChange(paramEncoder &shape, MeterState& bgMeterState, uint32_t& timer);
void pwmValChange(paramEncoder &pwm, MeterState& bgMeterState, uint32_t& bgtimer, uint32_t& pwmTimer, bool& pwmSwitch);

#endif
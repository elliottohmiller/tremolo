#ifndef TREM_CONTROLS_HPP
#define TREM_CONTROLS_HPP

#include "bgMeter.hpp"

void rateValChange(paramEncoder &rate, MeterState& bgMeterState, uint32_t& timer);
void depthValChange(paramEncoder &depth, MeterState& bgMeterState, uint32_t& timer);
void printShape(paramEncoder &shape);
void shapeValChange(paramEncoder &shape, MeterState& bgMeterState, uint32_t& timer);
void pwmValChange(paramEncoder &pwm, MeterState& bgMeterState, uint32_t& bgtimer, uint32_t& pwmTimer, bool& pwmSwitch);

#endif
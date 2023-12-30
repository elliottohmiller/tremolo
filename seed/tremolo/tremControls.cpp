#include "tremControls.hpp"

void rateValChange(paramEncoder &rate, MeterState& bgMeterState, uint32_t& timer)
{
    rate.enc->Debounce();
    float rateInHz;

    switch (rate.enc->Increment())
    {
        case -1: 
        if (rate.value > 1)
        {
            --rate.value;
            rateInHz = rate.value / 2.f;
            trem.SetFreq(rateInHz);
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
            rateInHz = rate.value / 2.f;
            trem.SetFreq(rateInHz);
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
            trem.SetDepth((depth.value/10.F)); 
            hw.PrintLine("depth decreased to: %u0%%\n", depth.value);
        }
        bgMeterState = MeterState::depthCV;
        timer = System::GetNow() + BGTIMER_K;
        
        break;

        case +1: 
        if (depth.value < 10)
        {
            ++depth.value;
            trem.SetDepth((depth.value/10.F));
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
            trem.SetWaveform((shape.value));
            printShape(shape);
        }
        bgMeterState = MeterState::shapeCV; 
        timer = System::GetNow() + BGTIMER_K;
        break;

        case +1: 
        if (shape.value < 4)
        {
            ++shape.value;
            trem.SetWaveform((shape.value));
            printShape(shape);
        }
        bgMeterState = MeterState::shapeCV; 
        timer = System::GetNow() + BGTIMER_K;
        break;

        default: 
        break;
    }
}

void pwmValChange(paramEncoder &pwm, MeterState& bgMeterState, uint32_t& bgtimer, uint32_t& pwmTimer, bool& pwmSwitch)
{
    if (pwmSwitch)
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
            trem.SetPw((pwm.value / 10.f));
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
            trem.SetPw((pwm.value / 10.f));
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
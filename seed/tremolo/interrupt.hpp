#ifndef INTERRUPT_HPP
#define INTERRUPT_HPP

#include <functional>
#include "stm32h7xx_hal_gpio.h"

typedef std::function<void()> callback_function_t;
void stm32_interrupt_enable(GPIO_TypeDef *port, uint16_t pin, callback_function_t callback, uint32_t mode);

#endif
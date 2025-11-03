#ifndef SETUP_HPP
#define SETUP_HPP

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>

#define lcd_en GPIO15
#define lcd_rs GPIO14

#define lcd_d4 GPIO5
#define lcd_d5 GPIO6
#define lcd_d6 GPIO7
#define lcd_d7 GPIO8


#define BAUD_RATE (9600)
#define M0	GPIO0//портВ  
#define M1	GPIO1

void clock_setup(void);
void gpio_setup(void);
void usart2_setup(void);//для общения с модулем
void usart3_setup(void);//для общения с ПК






#endif
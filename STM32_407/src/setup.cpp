#include "../inc/setup.hpp"

void clock_setup(void){
    // rcc_clock_setup_pll(&rcc_hse_16mhz_3v3[RCC_CLOCK_3V3_84MHZ]);

    // Включаем тактирование для GPIOA, GPIOB и USART1, USART2
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_USART3);
    rcc_periph_clock_enable(RCC_USART2);

}

void gpio_setup(void){
    //для M0 M1
    gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0 | GPIO1);


    //дисплей
	  gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT,GPIO_PUPD_NONE, lcd_rs);
	  gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT,GPIO_PUPD_NONE, lcd_en);
	  gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT,GPIO_PUPD_NONE, lcd_d4);
	  gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT,GPIO_PUPD_NONE, lcd_d5);
	  gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT,GPIO_PUPD_NONE, lcd_d6);
	  gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT,GPIO_PUPD_NONE, lcd_d7);

    //для светодиода
    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,GPIO1);
    // gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO1);
    // USART3: PB10 (TX), PB11 (RX) - для общения с ПК
    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO10 | GPIO11);
    gpio_set_af(GPIOB, GPIO_AF7, GPIO10 | GPIO11);
    
    // USART2: PA2 (TX), PA3 (RX) - для общения с E32
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2|GPIO3);
    gpio_set_af(GPIOA, GPIO_AF7, GPIO2|GPIO3); // AF7 для USART2 на этих пинах
    
}


void usart2_setup(void){ //для общения с модулем
      // Настройка USART2
    usart_set_baudrate(USART2, 9600);
    usart_set_databits(USART2, 8);
    usart_set_stopbits(USART2, USART_STOPBITS_1);
    usart_set_mode(USART2, USART_MODE_TX_RX);
    usart_set_parity(USART2, USART_PARITY_NONE);
    usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

    //активируем перрывания по приёму данных в UART2
  	usart_enable_rx_interrupt(USART2);
    nvic_enable_irq(NVIC_USART2_IRQ);
    nvic_set_priority(NVIC_USART2_IRQ, 0);
    
    // Включаем USART2
    usart_enable(USART2);
    

}
void usart3_setup(void){ //для общения с ПК
     // Настройка USART3
    usart_set_baudrate(USART3, 9600);
    usart_set_databits(USART3, 8);
    usart_set_stopbits(USART3, USART_STOPBITS_1);
    usart_set_mode(USART3, USART_MODE_TX_RX);
    usart_set_parity(USART3, USART_PARITY_NONE);
    usart_set_flow_control(USART3, USART_FLOWCONTROL_NONE);
    
    // Включаем USART3
    usart_enable(USART3);
}

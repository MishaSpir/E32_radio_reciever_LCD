// #include "src/LiquidCrystalSTM.h"
//этот файл является примером файла main.cpp, в котором реализуется работа с LCD дисплеем 20x04
//библиотечный файл LiquidCrystalSTM.cpp обязателен

//ВНИМАНИЕ! ТАЙМЕР2 занят функцией delay_us()

#include "src/LiquidCrystalSTM.cpp"
# include <libopencm3/stm32/rcc.h> 
# include <libopencm3/stm32/gpio.h> 
# include <libopencm3/stm32/timer.h>
# include <libopencm3/cm3/nvic.h>


//подключаение дисплея
#define lcd_en GPIO15
#define lcd_rs GPIO14

#define lcd_d4 GPIO5
#define lcd_d5 GPIO6
#define lcd_d6 GPIO7
#define lcd_d7 GPIO8



static void clock_setup(void)
{
	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
}

void gpio_setup(void)
{
	/* Enable GPIOC clock. */
	rcc_periph_clock_enable(RCC_GPIOB);

	/* Set GPIO9 (in GPIO port C) to 'output push-pull'. */
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,GPIO_CNF_OUTPUT_PUSHPULL, lcd_rs);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,GPIO_CNF_OUTPUT_PUSHPULL, GPIO2);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,GPIO_CNF_OUTPUT_PUSHPULL, lcd_en);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ	,GPIO_CNF_OUTPUT_PUSHPULL, lcd_d4);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,GPIO_CNF_OUTPUT_PUSHPULL, lcd_d5);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,GPIO_CNF_OUTPUT_PUSHPULL, lcd_d6);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,GPIO_CNF_OUTPUT_PUSHPULL, lcd_d7);
	
}







int main() {
	//строка для вывода на дисплей
	char str[]="Hello, STM32!";

	clock_setup();
	gpio_setup();
	timer_setup();


    //создаём объект lcd дисплея
	LiquidCrystal lcd(lcd_rs, lcd_en, lcd_d4, lcd_d5, lcd_d6, lcd_d7);
	//инициализируем дисплей 20x04
	 lcd.begin(20, 4,0x00);
	//тестрируем - выводим буквы 
	 lcd.write('A');
	 lcd.write('B');
	 lcd.write('C');
	 delay_us(5000000);
	 lcd.clear();
	 //выводим строку
	 lcd.setCursor(1,1);
	  for(uint8_t i = 0; i<(sizeof(str)/sizeof(str[0]))-1; i++){
		lcd.write(str[i]);
		delay_us(100000);// для эффекта 'титры' :) (можно убрать эту строку кода)
	  }
	  delay_us(1000000);
	lcd.clear();

	  





	
	

	while (true) {
		// gpio_toggle(GPIOB,GPIO2);
		// for(volatile uint32_t i =0; i < 1'000'000; i +=2);
	}
}


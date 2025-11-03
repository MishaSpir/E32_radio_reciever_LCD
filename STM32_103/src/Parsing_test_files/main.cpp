//эта программа  записывает конфигурацию в радиомодуль E32
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include  <cstdlib>

#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/vector.h>

#include "Pars.cpp"
  
#define BAUD_RATE (9600)
#define M0	GPIO0
#define M1	GPIO1

uint8_t data_buffer;	
Circular_buffer b;

volatile uint32_t tiks = 0;
uint32_t last_time = 0;

void sys_tick_handler(void){ //функция обработчик-прерываний systick
	tiks++;
}

void systick_setup(void){
systick_set_frequency(1000,72'000'000);
systick_counter_enable();
systick_interrupt_enable();

}

//Функия задержки в микросекундах
void delay_us(uint32_t us) {
	uint32_t start = systick_get_value();
	// Расчет тактов для указанного времени (72 тактов = 1 мкс при 72 МГц)
	uint32_t ticks = us * 72;
	while ((start - systick_get_value()) < ticks) {
		// Ждем, пока не пройдет нужное количество тактов
		}
	}

//  Функция задержки в миллисекундах
void delay_ms(uint32_t ms) {
	while (ms--) {
		delay_us(1000); // 1000 мкс = 1 мс
	}
}



static void clock_setup(void){
	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	// rcc_periph_clock_enable(RCC_AFIO);
	rcc_periph_clock_enable(RCC_USART2);
	rcc_periph_clock_enable(RCC_USART1);

}

static void gpio_setup(void){
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ,GPIO_CNF_OUTPUT_PUSHPULL, M0); //для M0
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ,GPIO_CNF_OUTPUT_PUSHPULL, M1); //для M1
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ,GPIO_CNF_OUTPUT_PUSHPULL, GPIO2); //для светодиода	
}

static void uart2_setup(void){
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART2_TX);
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT,GPIO_CNF_INPUT_FLOAT, GPIO_USART2_RX);

	/* Setup UART parameters. */
	usart_set_baudrate(USART2, BAUD_RATE);
	usart_set_databits(USART2, 8);
	usart_set_stopbits(USART2, USART_STOPBITS_1);
	usart_set_mode(USART2, USART_MODE_TX_RX);
	usart_set_parity(USART2, USART_PARITY_NONE);
	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

	//активируем перрывания по приёму данных в UART2
  	USART_CR1(USART2) |= USART_CR1_RXNEIE; 
	nvic_enable_irq(NVIC_USART2_IRQ);

	usart_enable(USART2);
}

static void uart1_setup(void){

/* Setup GPIO pin GPIO_USART1_TX. */
gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
gpio_set_mode(GPIOA, GPIO_MODE_INPUT,GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_RX);
/* Setup UART parameters. */
usart_set_baudrate(USART1, BAUD_RATE);
usart_set_databits(USART1, 8);
usart_set_stopbits(USART1, USART_STOPBITS_1);
usart_set_mode(USART1, USART_MODE_TX);
usart_set_parity(USART1, USART_PARITY_NONE);
usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
usart_enable(USART1);

}


  

  

void usart2_isr(void)
{
	if (((USART_CR1(USART2) & USART_CR1_RXNEIE) != 0) &&
	    ((USART_SR(USART2) & USART_SR_RXNE) != 0)) {
	
		// Чтение USART_DR автоматически очищает флаг USART_SR_RXNE
		b.put( static_cast<uint8_t>(usart_recv(USART2)));
		//активировать прерывание по готовности передатчика UART 
	}


}


  

int main(void){
	clock_setup();
	gpio_setup();
	uart2_setup();
	uart1_setup();
	systick_setup();
	//C0 00 00 18 06 44

/*-----------------пример, как отправлять разные по значению, но одинаковые по длине int
int int_temp = 6; 
char char_temp[3];
itoa(int_temp,char_temp,10);

//ФОРМИРУЕМ ПАКЕТ ДАННЫХ (ПРОТОКОЛ)
	// usart_send_blocking(USART2,'$');
	// // usart_send_blocking(USART2,'1');
	// // usart_send_blocking(USART2,'2');
	// // usart_send_blocking(USART2,'3');
	// usart_send_blocking(USART2,char_temp[0]);
	// usart_send_blocking(USART2,char_temp[1]);
	// usart_send_blocking(USART2,char_temp[2]);
	// usart_send_blocking(USART2,',');
	// usart_send_blocking(USART2,'4');
	// usart_send_blocking(USART2,'6');
	// usart_send_blocking(USART2,'*');
*/	


	uint8_t str[11];// в эту строку записывается принятый пакет
	for(int i=0; i<11; i++){str[i]=0;}
	//ФОРМИРУЕМ ПАКЕТ ДАННЫХ (ПРОТОКОЛ)
	usart_send_blocking(USART2,'$');
	usart_send_blocking(USART2,'1');
	usart_send_blocking(USART2,'2');
	usart_send_blocking(USART2,'3');
	usart_send_blocking(USART2,'\0');
	usart_send_blocking(USART2,',');
	usart_send_blocking(USART2,'5');
	usart_send_blocking(USART2,'6');
	usart_send_blocking(USART2,'7');
	usart_send_blocking(USART2,'\0');
	usart_send_blocking(USART2,'*');


	enum State {
	    idle, // = 0
	    data, // = 1
	    fin, // = 2
	    err // = 3
    };
	State state{idle};

	//БУФФЕР ПАРСИНГА
	//строка 0 --- первое рапарсированное число
	//строка 1 ---второе распарсированное число
	const uint8_t BUF_SIZE =4;
	char buf[3][BUF_SIZE];int m = 0;int k =0;
	for(int i = 0; i<3; i++){
		for(int j =0; j< BUF_SIZE; j++){
			buf[i][j]='\0';
		}
	}


	while (1) {
		gpio_toggle(GPIOB, GPIO2); //без переключения светодиоода ничего не работает 
	
		//если индексы чтения и записи в кольцевом буфере совпадают
		if(!b.empty()){

			if(b.readBytes(str,11)){//если пришло больше чем 11байт
				for(int i=0; i<11; ++i){
					usart_send_blocking(USART1,str[i]);
				}

				//КОНЕЧНЫЙ АВТОМАТ
				for(const char ch: str){//прогоняет по всей строке
					switch(state){
						case idle:
							if(ch == '$'){state = data;}// Принят маркер, переходим к приёму данных
							else {}
						break;
						case data:
							if(ch == '*'){state = fin;}//Приняттерминатор, заканчиваем приём данных
							else if (ch == '$'){state = err;}
							else if(ch!=',') {buf[k+1][m] = ch;m++; m %= BUF_SIZE;}
							else {k++; m = 0;}//Принят разделитель, принимаем новой число int 
						break;
						case fin:
							if (ch == '$'){state = data;} 
						else {state = err;} 
						break;
						case err:
						break;
						// default:
					}
				}
			}	
			

	
			
		}

	if(tiks-last_time>=1000){
			last_time = tiks;
	// usart_send_blocking(USART1,buf[0][0]);
	// usart_send_blocking(USART1,buf[0][1]);
	// usart_send_blocking(USART1,buf[0][2]);
	// usart_send_blocking(USART1,buf[0][3]);
	
	// usart_send_blocking(USART1,buf[1][0]);
	// usart_send_blocking(USART1,buf[1][1]);
	// usart_send_blocking(USART1,buf[1][2]);
	// usart_send_blocking(USART1,buf[1][3]);

	//ПАРСИНГ ИНТОВ
	int int_val[2];
	char str_from_int [2][4];
	for(int i=0; i<2; i++){
		for(int j=0; j<4; j++){
		str_from_int[i][j]=0;
		}
	}
	//формируем из строки число int
	int_val[0] = atoi(buf[1]);//buf[1]указатель на строку1 ---перове число
	int_val[1] = atoi(buf[2]);//buf[2]указатель на строку2 ---второе число
	//формируем из int строку(для вывода в UART)
	itoa(int_val[0],str_from_int[0],10);
	itoa(int_val[1],str_from_int[1],10);
	

	//если число меньше чем длина массива, в данном случае чем 4,
		//тогда itoa поставит '\0' на незаполненные места
	usart_send_blocking(USART1,str_from_int[0][0]);
	usart_send_blocking(USART1,str_from_int[0][1]);
	usart_send_blocking(USART1,str_from_int[0][2]);
	usart_send_blocking(USART1,str_from_int[0][3]);
	usart_send_blocking(USART1,'\t');
	
	usart_send_blocking(USART1,str_from_int[1][0]);
	usart_send_blocking(USART1,str_from_int[1][1]);
	usart_send_blocking(USART1,str_from_int[1][2]);
	usart_send_blocking(USART1,str_from_int[1][3]);
	usart_send_blocking(USART1,'\t');

		}

	// char *ptr1 = buf[1];
	// char str1[3];
	// ptr = buf;

	
	// int int_val1 = atoi(ptr1);
	// itoa(int_val1,str1,10);	

		gpio_set(GPIOB, M0);
		
		
			
			
		






	}

  

return 0;

}
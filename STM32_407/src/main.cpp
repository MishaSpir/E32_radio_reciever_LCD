//ОБЯЗАТЕЛЬНО!Перед использованием проверь таймеры! Настрой их,если надо: 
//таймер3 на прерывание каждую милисекнду
//таймер2 на прерывание каждую микросекунду(4 микросекунды)
//также проверь функции задержки delay_ms() и delay_us()
//это надо делать, так как я работал с черной платой с OZON и там бедас тактовой частотой




//В этой программе реализован приём данных по радиоканалу от моудля E32
//приём данных происодит по специальному протоколу: 
// $ - преамбула
// , - разделитель 
// * - терминатор

//для работы приёмника E32 необходим передатчик E32






#include "../inc/setup.hpp"
#include "../inc/Pars.hpp"
#include "../inc/time_setup.hpp"
#include "../inc/LiquidCrystalSTM.hpp"



uint8_t pkg_is_begin = 0;
uint8_t pkg_is_received = 0;

	
Circular_buffer b;
uint8_t ch;


//БУФФЕР ПАРСИНГА - хранит строку, очищенную от приамбулы и терминатора
const uint8_t PARS_SYMBOLS_SIZE =32;
int sym_index_2 = 0;int str_index_2 =0;
char pars_buf[PARS_SYMBOLS_SIZE];
void pars_buf_uart_print(void);


//БУФФЕР СТРОК ПОСЛЕ ПАРСИНГА - хранит строки, разделенные разделителем
//строка 0 --- первое рапарсированное число (Ключ)
//строка 1 ---второе распарсированное число
const uint8_t SYMBOLS_LENGTH =5;
const uint8_t STRINGS_LENGTH =5;
int sym_indx = 0;int str_indx =0;
char buf[STRINGS_LENGTH][SYMBOLS_LENGTH];
void buf_uart_print(void);
void buf_clear(void);


uint32_t last_time_1 = 0;




void usart2_isr(void)
{
    if (((USART_CR1(USART2) & USART_CR1_RXNEIE) != 0) &&
	    ((USART_SR(USART2) & USART_SR_RXNE) != 0)) {
        
        if(usart_recv(USART2) =='$' ){pkg_is_begin=1;}
		
		if(pkg_is_begin ){
			b.put( static_cast<uint8_t>(usart_recv(USART2)));
			if(!b.empty()){
				ch = b.get();
				usart_send_blocking(USART3,ch);
				//использование конечного автомата		
				FSM(pars_buf,PARS_SYMBOLS_SIZE, sym_index_2, str_index_2,pkg_is_begin,pkg_is_received,ch);
			}	
				
		}     
	}

}



int main(void)
{   
    clock_setup();
    gpio_setup();
    usart2_setup();
    usart3_setup();
    timer3_setup();
    timer2_setup();
	timer4_setup();

	//ИНИЦИАЛИЗАЦИЯ РАДИОМОДУЛЯ
	uint8_t str_tx[]={0xC0,0x00,0x00,0x1D,0x06,0x44}; 
	E32_InitConfig(GPIOB,M0,M1,str_tx);
	delay_ms(2000);
	
	//ИНИЦИАЛИЗАЦИЯ ДИСПЛЕЯ
	LiquidCrystal lcd(lcd_rs, lcd_en, lcd_d4, lcd_d5, lcd_d6, lcd_d7);
	lcd.begin(20,4,0x00);

	uint8_t str_init[] = "E32_radio_reciever";	//приветсвующая строка
	for(unsigned int i = 0; i< sizeof(str_init)/sizeof(str_init[0]); i++){
		lcd.write(str_init[i]);
		delay_ms(80);
	}

	uint8_t EraseSymbolForLCD[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00}; //пустой символ - выводится, когда посылаем '\0'	
	lcd.createChar(0, EraseSymbolForLCD);
	delay_ms(2000);


	
    while (1) {
        if(get_ms() - last_time_1 >= 1000){
            last_time_1 = get_ms();
			


			

        }
		//ВОТ САМОЕ ИНТЕРЕСНОЕ - если пакет получен
		if(pkg_is_received){
			pkg_is_received = 0;
			sym_indx = 0;
			str_indx = 0;
			buf_clear();

			//второй парсинг
			usart_send_blocking(USART3,'\t');
			for( char pars_buf_ch: pars_buf){
				if (pars_buf_ch=='\0'){continue;}
				usart_send_blocking(USART3,pars_buf_ch);
				if(pars_buf_ch!=',') {
                    buf[str_indx][sym_indx] = pars_buf_ch;
					// usart_send_blocking(USART3,buf[str_indx+1][sym_indx]);
					sym_indx++;
					sym_indx %= SYMBOLS_LENGTH;
					// usart_send_blocking(USART3,ch);
				}
				else if(pars_buf_ch=','){
					// pkg_is_received = 0;
					str_indx++; 
					str_indx %= STRINGS_LENGTH;
					sym_indx = 0;		
				}				
			}
			
			buf_uart_print();

			//ВЫВОДИМ НА ДИСПЛЕЙ ТО, ЧТО ПОЛУЧИЛИ
			 lcd.clear();
			 lcd.write(buf[1][0]);
			 lcd.write(buf[1][1]);
			 lcd.write(buf[1][2]);
			 lcd.write(buf[1][3]);
			 lcd.setCursor(0,5);
			 lcd.write(buf[2][0]);
			 lcd.write(buf[2][1]);
			 lcd.write(buf[2][2]);
			 lcd.setCursor(0,9);
			 lcd.write(buf[3][0]);
			 lcd.write(buf[3][1]);
			 lcd.write(buf[3][2]);	

		}
    }
    
return 0;
}

void buf_uart_print(void){//функция вывода ВСЕГО содержимого буфера на UART
	usart_send_blocking(USART3,'\t');	
			for(int i = 0; i<STRINGS_LENGTH; i++){
				for(int j =0; j< SYMBOLS_LENGTH; j++){
					if(buf[i][j]!='\0'){
						usart_send_blocking(USART3,buf[i][j]);
					}
					
				}
				usart_send_blocking(USART3,'\t');
			}usart_send_blocking(USART3,'\n');
}

void buf_clear(void){ //функция очистки буфера парсинга
	for(int i = 0; i<STRINGS_LENGTH; i++){
		for(int j =0; j< SYMBOLS_LENGTH; j++){
			buf[i][j]='\0';
		}
	}
}



void pars_buf_uart_print(void){
	usart_send_blocking(USART3,'\t');
			for(int i =0; i< PARS_SYMBOLS_SIZE; i++){
					if (pars_buf[i]=='\0'){continue;}
				usart_send_blocking(USART3,pars_buf[i]);	
			}
}




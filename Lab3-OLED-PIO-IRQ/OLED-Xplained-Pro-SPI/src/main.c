#include <asf.h>

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

// LED SAME70
#define LED_PIO PIOC
#define LED_PIO_ID ID_PIOC
#define LED_PIO_IDX 8
#define LED_PIO_IDX_MASK (1 << LED_PIO_IDX)

// OLED1 BUTTON 1
#define BUT_1_PIO  PIOD
#define BUT_1_PIO_ID  ID_PIOD
#define BUT_1_PIO_IDX  28
#define BUT_1_PIO_IDX_MASK  (1u << BUT_1_PIO_IDX)

// OLED1 BUTTON 2
#define BUT_2_PIO  PIOC
#define BUT_2_PIO_ID  ID_PIOC
#define BUT_2_PIO_IDX  31
#define BUT_2_PIO_IDX_MASK  (1u << BUT_2_PIO_IDX)

// OLED1 BUTTON 3
#define BUT_3_PIO  PIOA
#define BUT_3_PIO_ID  ID_PIOA
#define BUT_3_PIO_IDX  19
#define BUT_3_PIO_IDX_MASK  (1u << BUT_3_PIO_IDX)

// Globals
int delay = 300;
char delayChar[128];

// Flags
volatile char but_1_flag;
volatile char but_2_flag;
volatile char but_3_flag;

// Prototypes
void io_init(void);
void pisca_led(int n, int delay);

// Handlers / Callbacks
void button_1_callback(void) {
	pio_get(BUT_1_PIO, PIO_INPUT, BUT_1_PIO_IDX_MASK) ? (but_1_flag = 0) : (but_1_flag = 1);
}

void button_2_callback(void) {
	pio_get(BUT_2_PIO, PIO_INPUT, BUT_2_PIO_IDX_MASK) ? (but_2_flag = 1) : (but_2_flag = 0);
}

void button_3_callback(void) {
	pio_get(BUT_3_PIO, PIO_INPUT, BUT_3_PIO_IDX_MASK) ? (but_3_flag = 1) : (but_3_flag = 0);
}

// Funções
void dec_delay() {
	delay -= 100;
}

void inc_delay() {
	delay += 100;
}

void refresh_display(int delay) {
	sprintf(delayChar, "delay: %d ms", delay);
	gfx_mono_draw_string(delayChar, 0, 0, &sysfont);
}

void pisca_led(int n, int t) {
		
	for (int i=0;i<n;i++){
		pio_clear(LED_PIO, LED_PIO_IDX_MASK);
		delay_ms(t);
		pio_set(LED_PIO, LED_PIO_IDX_MASK);
		delay_ms(t);
		
		if (but_2_flag) {
			but_2_flag = 0;
			break;
		}
	}	
}

void io_init(void)
{

	// Configura SAME70 LED como output
	pmc_enable_periph_clk(LED_PIO_ID);
	pio_configure(LED_PIO, PIO_OUTPUT_0, LED_PIO_IDX_MASK, PIO_DEFAULT);

	// Inicializa clock do periférico PIO responsavel pelos OLED1 BUTTONS
	pmc_enable_periph_clk(BUT_1_PIO_ID);
	pmc_enable_periph_clk(BUT_2_PIO_ID);
	pmc_enable_periph_clk(BUT_3_PIO_ID);

	// Configura PIO para lidar com o pino dos OLED1 BUTTONS como inputs com pull-up
	pio_configure(BUT_1_PIO, PIO_INPUT, BUT_1_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT_1_PIO, BUT_1_PIO_IDX_MASK, 60);
	
	pio_configure(BUT_2_PIO, PIO_INPUT, BUT_2_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT_2_PIO, BUT_2_PIO_IDX_MASK, 60);
	
	pio_configure(BUT_3_PIO, PIO_INPUT, BUT_3_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT_3_PIO, BUT_3_PIO_IDX_MASK, 60);

	// Configura interrupção no pino referente aos OLED1 BUTTONS e associa
	// função de callback caso uma interrupção seja gerada
	pio_handler_set(BUT_1_PIO, BUT_1_PIO_ID, BUT_1_PIO_IDX_MASK, PIO_IT_EDGE, button_1_callback);
	pio_handler_set(BUT_2_PIO, BUT_2_PIO_ID, BUT_2_PIO_IDX_MASK, PIO_IT_RISE_EDGE, button_2_callback);
	pio_handler_set(BUT_3_PIO, BUT_3_PIO_ID, BUT_3_PIO_IDX_MASK, PIO_IT_RISE_EDGE, button_3_callback);

	// Ativa interrupção e limpa primeira IRQ gerada na ativação
	pio_enable_interrupt(BUT_1_PIO, BUT_1_PIO_IDX_MASK);
	pio_get_interrupt_status(BUT_1_PIO);
	
	pio_enable_interrupt(BUT_2_PIO, BUT_2_PIO_IDX_MASK);
	pio_get_interrupt_status(BUT_2_PIO);
	
	pio_enable_interrupt(BUT_3_PIO, BUT_3_PIO_IDX_MASK);
	pio_get_interrupt_status(BUT_3_PIO);
	
	// Configura NVIC para receber interrupções do PIO dos OLED1 BUTTONS
	// com prioridade 4 (quanto mais próximo de 0 maior)
	NVIC_EnableIRQ(BUT_1_PIO_ID);
	NVIC_SetPriority(BUT_1_PIO_ID, 4); // Prioridade 4
	
	NVIC_EnableIRQ(BUT_2_PIO_ID);
	NVIC_SetPriority(BUT_2_PIO_ID, 5); // Prioridade 4
	
	NVIC_EnableIRQ(BUT_3_PIO_ID);
	NVIC_SetPriority(BUT_3_PIO_ID, 6); // Prioridade 4
}

int main (void) {
	
	// Inicializa clock
	sysclk_init();
	
	// Desativa watchdog
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	board_init();
	delay_init();
	
	// configura OLED1 BUTTONS com interrupção
	io_init();

	// Init OLED
	gfx_mono_ssd1306_init();
	
	// SAME70 LED começa apagado
	pio_set(LED_PIO, LED_PIO_IDX_MASK);
	
	// OLED1 Display começa com a frequência inicial
	refresh_display(delay);
	  
	while(1) {
		
		if (but_1_flag) {
			dec_delay();
			refresh_display(delay);
			but_1_flag = 1;
		}
		
		int c = 0;
		while (but_1_flag) {
			if (c > 25000000) {
				inc_delay();
				refresh_display(delay);
				c = 0;
			}
			c++;
		}
		
		if (but_2_flag) {
			but_2_flag = 0;
			pisca_led(30, delay);
		} 
		
		if (but_3_flag) {
			inc_delay();
			refresh_display(delay);
			but_3_flag = 0;
		}
		
				
		// Entra em sleep mode
		pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
	}
	
	
	return 0;
}

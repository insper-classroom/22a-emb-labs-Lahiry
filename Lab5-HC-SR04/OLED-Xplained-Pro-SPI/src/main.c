#include <asf.h>

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"
#include "defines.h"
#include "auxiliar.h"

/************************************************************************/
/* VAR globais                                                          */
/************************************************************************/

float sound_speed = 340.0;
int frequency = 1 / (2 * 0.000058);
int rtt_elapsed_time;
char dist[20];
float distance = 0.0;

volatile char flag_echo;

/************************************************************************/
/* PROTOTYPES                                                           */
/************************************************************************/

void echo_callback(void);
void trigger_pulse(void);
void init(void);

/************************************************************************/
/* Handlers                                                             */
/************************************************************************/

void echo_callback(void)
{
	if (pio_get(ECHO_PIO, PIO_INPUT, ECHO_IDX_MASK)) {
		RTT_init(frequency, 0, 0); // inicia o RTT na borda de subida do pino Echo
	} else {
		flag_echo = 1;
		rtt_elapsed_time = rtt_read_timer_value(RTT); // lê valor do RTT em borda de descida do pino Echo
	}
}

/************************************************************************/
/* Funções                                                              */
/************************************************************************/

void trigger_pulse() { // gera o pulso no pino de Trig com delay_us
	pio_set(TRIG_PIO, TRIG_IDX_MASK);
	delay_us(10);
	pio_clear(TRIG_PIO, TRIG_IDX_MASK);
}

void init (void) {
	
	// Inicializa clock
	sysclk_init();
	
	// Desativa watchdog
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	board_init();
	delay_init();
	
	// Init OLED
	oled_init();
	gfx_mono_ssd1306_init();
	
	// Init ECHO
	pmc_enable_periph_clk(TRIG_ID);
	pio_configure(TRIG_PIO, PIO_OUTPUT_1, TRIG_IDX_MASK, PIO_PULLUP);
	config_button(ECHO_PIO, ECHO_IDX_MASK, ECHO_ID, echo_callback, 0, 0);
}

int main (void) {
	
	init();
	
	/* Insert application code here, after the board has been initialized. */
	while(1) {
		
		if (flag_but_1) {
			trigger_pulse();
			flag_but_1 = 0;
		}
		
		if (flag_echo) {
			distance = (sound_speed * rtt_elapsed_time * 100) / (2.0 * frequency); // o sensor fornece a distância dobrado (tempo de ir e voltar)!
			sprintf(dist, "%f", distance);
			gfx_mono_draw_string(dist, 0, 16, &sysfont);
			flag_echo = 0;
			
			if (distance < 2 || distance > 400) { // erro caso a distancia seja menor que a mínima ou maior que a máxima
				erase_oled();
				gfx_mono_draw_string("Error :(", 0, 16, &sysfont);
			}
		}
				
		pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
	}
}

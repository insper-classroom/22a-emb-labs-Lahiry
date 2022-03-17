#include <asf.h>

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

/************************************************************************/
/* DEFINES                                                              */
/************************************************************************/

// OLED1 LED 1
#define LED_1_PIO  PIOA
#define LED_1_ID  ID_PIOA
#define LED_1_IDX  0
#define LED_1_IDX_MASK  (1 << LED_1_IDX)

// OLED1 LED 2
#define LED_2_PIO  PIOC
#define LED_2_ID  ID_PIOC
#define LED_2_IDX  30
#define LED_2_IDX_MASK  (1 << LED_2_IDX)

// OLED1 LED 3
#define LED_3_PIO  PIOB
#define LED_3_ID  ID_PIOB
#define LED_3_IDX  2
#define LED_3_IDX_MASK  (1 << LED_3_IDX)

// OLED1 BUTTON 1
#define BUT_1_PIO  PIOD
#define BUT_1_ID  ID_PIOD
#define BUT_1_IDX  28
#define BUT_1_IDX_MASK  (1u << BUT_1_IDX)

// OLED1 BUTTON 2
#define BUT_2_PIO  PIOC
#define BUT_2_ID  ID_PIOC
#define BUT_2_IDX  31
#define BUT_2_IDX_MASK  (1u << BUT_2_IDX)

// OLED1 BUTTON 3
#define BUT_3_PIO  PIOA
#define BUT_3_ID  ID_PIOA
#define BUT_3_IDX  19
#define BUT_3_IDX_MASK  (1u << BUT_3_IDX)

typedef struct  {
	uint32_t year;
	uint32_t month;
	uint32_t day;
	uint32_t week;
	uint32_t hour;
	uint32_t minute;
	uint32_t second;
} calendar;

/************************************************************************/
/* VAR globais                                                          */
/************************************************************************/

volatile char flag_rtc_alarm = 0;
volatile char flag_but_1 = 0;

uint32_t current_hour, current_min, current_sec;
uint32_t current_year, current_month, current_day, current_week;

/************************************************************************/
/* PROTOTYPES                                                           */
/************************************************************************/

void LED_init(int estado);
void TC_init(Tc * TC, int ID_TC, int TC_CHANNEL, int freq);
static void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource);
void RTC_init(Rtc *rtc, uint32_t id_rtc, calendar t, uint32_t irq_type);
void but_1_callback(void);
void pin_toggle(Pio *pio, uint32_t mask);
void pisca_led(int n, int delay);

/************************************************************************/
/* Handlers                                                             */
/************************************************************************/

void TC1_Handler(void) {
	/**
	* Devemos indicar ao TC que a interrupção foi satisfeita.
	* Isso é realizado pela leitura do status do periférico
	**/
	volatile uint32_t status = tc_get_status(TC0, 1);

	/** Muda o estado do LED (pisca) **/
	pin_toggle(LED_1_PIO, LED_1_IDX_MASK);  
}

void RTT_Handler(void) {
	uint32_t ul_status;

	/* Get RTT status - ACK */
	ul_status = rtt_get_status(RTT);

	/* IRQ due to Alarm */
	if ((ul_status & RTT_SR_ALMS) == RTT_SR_ALMS) {
		RTT_init(4, 16, RTT_MR_ALMIEN);
	}
	
	/* IRQ due to Time has changed */
	if ((ul_status & RTT_SR_RTTINC) == RTT_SR_RTTINC) {
		pin_toggle(LED_2_PIO, LED_2_IDX_MASK);    // BLINK Led
	}

}

void RTC_Handler(void) {
	uint32_t ul_status = rtc_get_status(RTC);
	
	/* seccond tick */
	if ((ul_status & RTC_SR_SEC) == RTC_SR_SEC) {
		// o código para irq de segundo vem aqui
	}
	
	/* Time or date alarm */
	if ((ul_status & RTC_SR_ALARM) == RTC_SR_ALARM) {
		// o código para irq de alame vem aqui
		flag_rtc_alarm = 1;
	}

	rtc_clear_status(RTC, RTC_SCCR_SECCLR);
	rtc_clear_status(RTC, RTC_SCCR_ALRCLR);
	rtc_clear_status(RTC, RTC_SCCR_ACKCLR);
	rtc_clear_status(RTC, RTC_SCCR_TIMCLR);
	rtc_clear_status(RTC, RTC_SCCR_CALCLR);
	rtc_clear_status(RTC, RTC_SCCR_TDERRCLR);
}

void but_1_callback(void) {
	pio_get(BUT_1_PIO, PIO_INPUT, BUT_1_IDX_MASK) ? (flag_but_1 = 0) : (flag_but_1 = 1);
}

/************************************************************************/
/* Funções                                                              */
/************************************************************************/

void LED_1_init(int estado) {
	pmc_enable_periph_clk(LED_1_ID);
	pio_set_output(LED_1_PIO, LED_1_IDX_MASK, estado, 0, 0);
};

void LED_2_init(int estado) {
	pmc_enable_periph_clk(LED_2_ID);
	pio_set_output(LED_2_PIO, LED_2_IDX_MASK, estado, 0, 0);
};

void LED_3_init(int estado) {
	pmc_enable_periph_clk(LED_3_ID);
	pio_set_output(LED_3_PIO, LED_3_IDX_MASK, estado, 0, 0);
};

void pin_toggle(Pio *pio, uint32_t mask) {
	if (pio_get_output_data_status(pio, mask)) {
		pio_clear(pio, mask);
	} else {
		pio_set(pio,mask);
	}
}

void pisca_led (int n, int t) {
	for (int i=0;i<n;i++){
		pio_clear(LED_3_PIO, LED_3_IDX_MASK);
		delay_ms(t);
		pio_set(LED_3_PIO, LED_3_IDX_MASK);
		delay_ms(t);
	}
}

void TC_init(Tc * TC, int ID_TC, int TC_CHANNEL, int freq){
	uint32_t ul_div;
	uint32_t ul_tcclks;
	uint32_t ul_sysclk = sysclk_get_cpu_hz();

	/* Configura o PMC */
	pmc_enable_periph_clk(ID_TC);

	/** Configura o TC para operar em  freq hz e interrupçcão no RC compare */
	tc_find_mck_divisor(freq, ul_sysclk, &ul_div, &ul_tcclks, ul_sysclk);
	tc_init(TC, TC_CHANNEL, ul_tcclks | TC_CMR_CPCTRG);
	tc_write_rc(TC, TC_CHANNEL, (ul_sysclk / ul_div) / freq);

	/* Configura NVIC*/
	NVIC_SetPriority(ID_TC, 4);
	NVIC_EnableIRQ((IRQn_Type) ID_TC);
	tc_enable_interrupt(TC, TC_CHANNEL, TC_IER_CPCS);
}

static float get_time_rtt(){
	uint ul_previous_time = rtt_read_timer_value(RTT);
}

static void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource) {

	uint16_t pllPreScale = (int) (((float) 32768) / freqPrescale);
	
	rtt_sel_source(RTT, false);
	rtt_init(RTT, pllPreScale);
	
	if (rttIRQSource & RTT_MR_ALMIEN) {
		uint32_t ul_previous_time;
		ul_previous_time = rtt_read_timer_value(RTT);
		while (ul_previous_time == rtt_read_timer_value(RTT));
		rtt_write_alarm_time(RTT, IrqNPulses+ul_previous_time);
	}

	/* config NVIC */
	NVIC_DisableIRQ(RTT_IRQn);
	NVIC_ClearPendingIRQ(RTT_IRQn);
	NVIC_SetPriority(RTT_IRQn, 4);
	NVIC_EnableIRQ(RTT_IRQn);

	/* Enable RTT interrupt */
	if (rttIRQSource & (RTT_MR_RTTINCIEN | RTT_MR_ALMIEN))
	rtt_enable_interrupt(RTT, rttIRQSource);
	else
	rtt_disable_interrupt(RTT, RTT_MR_RTTINCIEN | RTT_MR_ALMIEN);
	
}

void RTC_init(Rtc *rtc, uint32_t id_rtc, calendar t, uint32_t irq_type) {
	/* Configura o PMC */
	pmc_enable_periph_clk(ID_RTC);

	/* Default RTC configuration, 24-hour mode */
	rtc_set_hour_mode(rtc, 0);

	/* Configura data e hora manualmente */
	rtc_set_date(rtc, t.year, t.month, t.day, t.week);
	rtc_set_time(rtc, t.hour, t.minute, t.second);

	/* Configure RTC interrupts */
	NVIC_DisableIRQ(id_rtc);
	NVIC_ClearPendingIRQ(id_rtc);
	NVIC_SetPriority(id_rtc, 4);
	NVIC_EnableIRQ(id_rtc);

	/* Ativa interrupcao via alarme */
	rtc_enable_interrupt(rtc,  irq_type);
}

void init (void) {
	
	// Inicializa clock
	sysclk_init();
	
	// Desativa watchdog
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	board_init();
	delay_init();
	
	// Init OLED
	gfx_mono_ssd1306_init();
	
	LED_1_init(1);
	LED_2_init(1);
	LED_3_init(1);
	
	pmc_enable_periph_clk(BUT_1_ID);
	//pmc_enable_periph_clk(BUT_2_PIO_ID);
	//pmc_enable_periph_clk(BUT_3_PIO_ID);
	
	pio_configure(BUT_1_PIO, PIO_INPUT, BUT_1_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);	
	//pio_configure(BUT_2_PIO, PIO_INPUT, BUT_2_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	//pio_configure(BUT_3_PIO, PIO_INPUT, BUT_3_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	
	pio_set_debounce_filter(BUT_1_PIO, BUT_1_IDX_MASK, 60);
	//pio_set_debounce_filter(BUT_2_PIO, BUT_2_PIO_IDX_MASK, 60);
	//pio_set_debounce_filter(BUT_3_PIO, BUT_3_PIO_IDX_MASK, 60);
	
	// Configura interrupção no pino referente aos OLED1 BUTTONS e associa
	// função de callback caso uma interrupção seja gerada
	pio_handler_set(BUT_1_PIO, BUT_1_ID, BUT_1_IDX_MASK, PIO_IT_EDGE, but_1_callback);
	
	// Ativa interrupção e limpa primeira IRQ gerada na ativação
	pio_enable_interrupt(BUT_1_PIO, BUT_1_IDX_MASK);
	pio_get_interrupt_status(BUT_1_PIO);
	
	// Configura NVIC para receber interrupções do PIO dos OLED1 BUTTONS
	// com prioridade 4 (quanto mais próximo de 0 maior)
	NVIC_EnableIRQ(BUT_1_ID);
	NVIC_SetPriority(BUT_1_ID, 4); // Prioridade 4
	
	/* Configure timer TC0, canal 1 */
	/* e inicializa a contagem */
	TC_init(TC0, ID_TC1, 1, 4);
	tc_start(TC0, 1);
	
	/* Configure RTT */
	RTT_init(4, 16, RTT_MR_ALMIEN);
	
	/* Configura RTC */
	calendar rtc_initial = {2018, 3, 19, 12, 15, 45 ,1};
	RTC_init(RTC, ID_RTC, rtc_initial, RTC_IER_ALREN);

}

int main (void) {
	
	init();
	
	/* Insert application code here, after the board has been initialized. */
	while(1) {
		
		rtc_get_time(RTC, &current_hour, &current_min, &current_sec);
		rtc_get_date(RTC, &current_year, &current_month, &current_day, &current_week);
		
		if (flag_but_1) {
			/* configura alarme do RTC para daqui 20 segundos */
			rtc_set_date_alarm(RTC, 1, current_month, 1, current_day);
			rtc_set_time_alarm(RTC, 1, current_hour, 1, current_min, 1, current_sec + 5);
		}
		
		if(flag_rtc_alarm) {
			pisca_led(5, 200);
			flag_rtc_alarm = 0;
		}
		pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
	}

}

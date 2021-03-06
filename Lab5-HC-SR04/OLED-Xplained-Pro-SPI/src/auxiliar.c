// TC-RTT-RTC support

typedef struct  {
	uint32_t year;
	uint32_t month;
	uint32_t day;
	uint32_t week;
	uint32_t hour;
	uint32_t minute;
	uint32_t second;
} calendar;

uint32_t current_hour, current_min, current_sec;
uint32_t current_year, current_month, current_day, current_week;

void TC_init(Tc * TC, int ID_TC, int TC_CHANNEL, int freq){
	uint32_t ul_div;
	uint32_t ul_tcclks;
	uint32_t ul_sysclk = sysclk_get_cpu_hz();

	/* Configura o PMC */
	pmc_enable_periph_clk(ID_TC);

	/** Configura o TC para operar em  freq hz e interrup?c?o no RC compare */
	tc_find_mck_divisor(freq, ul_sysclk, &ul_div, &ul_tcclks, ul_sysclk);
	tc_init(TC, TC_CHANNEL, ul_tcclks | TC_CMR_CPCTRG);
	tc_write_rc(TC, TC_CHANNEL, (ul_sysclk / ul_div) / freq);

	/* Configura NVIC*/
	NVIC_SetPriority(ID_TC, 4);
	NVIC_EnableIRQ((IRQn_Type) ID_TC);
	tc_enable_interrupt(TC, TC_CHANNEL, TC_IER_CPCS);
}

void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource) {

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

void RTT_Handler(void) {
	uint32_t ul_status;

	/* Get RTT status - ACK */
	ul_status = rtt_get_status(RTT);

	/* IRQ due to Alarm */
	if ((ul_status & RTT_SR_ALMS) == RTT_SR_ALMS) {
		//RTT_init(4, 16, RTT_MR_ALMIEN);
	}
	
	/* IRQ due to Time has changed */
	if ((ul_status & RTT_SR_RTTINC) == RTT_SR_RTTINC) {
		//pin_toggle(LED_2_PIO, LED_2_IDX_MASK);    // BLINK Led
	}

}

void RTC_Handler(void) {
	uint32_t ul_status = rtc_get_status(RTC);
	
	/* seccond tick */
	if ((ul_status & RTC_SR_SEC) == RTC_SR_SEC) {
		// o c?digo para irq de segundo vem aqui
		//flag_rtc_sec = 1;
	}
	
	/* Time or date alarm */
	if ((ul_status & RTC_SR_ALARM) == RTC_SR_ALARM) {
		// o c?digo para irq de alame vem aqui
		//flag_rtc_alarm = 1;
	}

	rtc_clear_status(RTC, RTC_SCCR_SECCLR);
	rtc_clear_status(RTC, RTC_SCCR_ALRCLR);
	rtc_clear_status(RTC, RTC_SCCR_ACKCLR);
	rtc_clear_status(RTC, RTC_SCCR_TIMCLR);
	rtc_clear_status(RTC, RTC_SCCR_CALCLR);
	rtc_clear_status(RTC, RTC_SCCR_TDERRCLR);
}

/* Configure timer TC0, canal 1
 e inicializa a contagem 
TC_init(TC0, ID_TC1, 1, 4);
tc_start(TC0, 1);

Configure RTT
RTT_init(4, 16, RTT_MR_ALMIEN);

Configura RTC
calendar rtc_initial = {2018, 3, 19, 12, 15, 45 ,1};
RTC_init(RTC, ID_RTC, rtc_initial, RTC_IER_ALREN | RTC_IER_SECEN); */


// Aux functions

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

void draw_time (uint32_t current_hour, uint32_t current_min, uint32_t current_sec){
	char tempo[20];
	sprintf(tempo, "%02d:%02d:%02d", current_hour, current_min, current_sec);
	gfx_mono_draw_string(tempo, 5,16, &sysfont);
}

// Configure button

void config_button(Pio *p_pio, const uint32_t ul_mask, uint32_t ul_id, void (*p_handler) (uint32_t, uint32_t), int it_rise, int filter) {
	pmc_enable_periph_clk(ul_id);
	filter ? pio_configure(p_pio, PIO_INPUT, ul_mask, PIO_PULLUP | PIO_DEBOUNCE) : pio_configure(p_pio, PIO_INPUT, ul_mask, PIO_DEFAULT);

	pio_set_debounce_filter(p_pio, ul_mask, 60);

	uint32_t but_attr = (it_rise ? PIO_IT_RISE_EDGE : PIO_IT_EDGE);
	pio_handler_set(p_pio,
	ul_id,
	ul_mask,
	but_attr,
	p_handler);

	pio_enable_interrupt(p_pio, ul_mask);
	pio_get_interrupt_status(p_pio);
	
	NVIC_EnableIRQ(ul_id);
	NVIC_SetPriority(ul_id, 4);
}

// OLED support

volatile char flag_but_1;
volatile char flag_but_2;
volatile char flag_but_3;

void but_1_callback(void) {
	flag_but_1 = 1;
}

void but_2_callback(void) {
	flag_but_2 = 1;
}

void but_3_callback(void) {
	flag_but_3 = 1;
}

void oled_init(void) {
	// LEDS
	pmc_enable_periph_clk(LED_1_ID);
	pmc_enable_periph_clk(LED_2_ID);
	pmc_enable_periph_clk(LED_3_ID);
	pio_configure(LED_1_PIO, PIO_OUTPUT_1, LED_1_IDX_MASK, PIO_DEFAULT);
	pio_configure(LED_2_PIO, PIO_OUTPUT_1, LED_2_IDX_MASK, PIO_DEFAULT);
	pio_configure(LED_3_PIO, PIO_OUTPUT_1, LED_3_IDX_MASK, PIO_DEFAULT);
	
	// BUTTONS
	config_button(BUT_1_PIO, BUT_1_IDX_MASK, BUT_1_ID, but_1_callback, 1, 1);
	config_button(BUT_2_PIO, BUT_2_IDX_MASK, BUT_2_ID, but_2_callback, 1, 1);
	config_button(BUT_3_PIO, BUT_3_IDX_MASK, BUT_3_ID, but_3_callback, 1, 1);
}

void erase_oled(void) {
	gfx_mono_draw_string("             ", 0, 5, &sysfont);
	gfx_mono_draw_string("             ", 0, 16, &sysfont);
}
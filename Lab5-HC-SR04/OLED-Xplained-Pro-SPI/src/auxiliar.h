#include "auxiliar.c"

void TC_init(Tc * TC, int ID_TC, int TC_CHANNEL, int freq);
void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource);
void RTC_init(Rtc *rtc, uint32_t id_rtc, calendar t, uint32_t irq_type);

void pin_toggle(Pio *pio, uint32_t mask);
void pisca_led(int n, int delay);
void draw_time(uint32_t current_hour, uint32_t current_min, uint32_t current_sec);

void config_button(Pio *p_pio, const uint32_t ul_mask, uint32_t ul_id, void (*p_handler) (uint32_t, uint32_t), int it_rise, int filter);

void but_1_callback(void);
void but_2_callback(void);
void but_3_callback(void);

void oled_init(void);
void erase_oled(void);
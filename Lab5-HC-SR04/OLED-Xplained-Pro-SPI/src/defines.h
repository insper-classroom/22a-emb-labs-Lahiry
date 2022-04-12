/************************************************************************/
/* DEFINES                                                              */
/************************************************************************/

// SAME70 LED
#define LED_PIO PIOC
#define LED_ID ID_PIOC
#define LED_IDX 8
#define LED_IDX_MASK (1 << LED_IDX)

// SAME70 BUTTON
#define BUT_PIO PIOA
#define BUT_ID ID_PIOA
#define BUT_IDX 11
#define BUT_IDX_MASK (1u << BUT_IDX)

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

// ECHO
#define ECHO_PIO PIOA
#define ECHO_ID ID_PIOA
#define ECHO_IDX 2
#define ECHO_IDX_MASK (1u << ECHO_IDX)

// TRIG
#define TRIG_PIO PIOA
#define TRIG_ID ID_PIOA
#define TRIG_IDX 24
#define TRIG_IDX_MASK (1u << TRIG_IDX)
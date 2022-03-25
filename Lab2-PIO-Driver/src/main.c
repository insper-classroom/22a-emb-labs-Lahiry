/**
 * 5 semestre - Eng. da Computação - Insper
 * Rafael Corsi - rafael.corsi@insper.edu.br
 *
 * Projeto 0 para a placa SAME70-XPLD
 *
 * Objetivo :
 *  - Introduzir ASF e HAL
 *  - Configuracao de clock
 *  - Configuracao pino In/Out
 *
 * Material :
 *  - Kit: ATMEL SAME70-XPLD - ARM CORTEX M7
 */

/************************************************************************/
/* includes                                                             */
/************************************************************************/

#include "asf.h"

/************************************************************************/
/* defines                                                              */
/************************************************************************/

/*--------------------------------LEDS----------------------------------*/

        /*------------------------SAME70---------------------------*/

#define LED_PIO  PIOC                          // periferico que controla o LED
#define LED_PIO_ID  ID_PIOC                    // ID do periférico PIOC (controla LED)
#define LED_PIO_IDX  8                         // ID do LED no PIO
#define LED_PIO_IDX_MASK  (1 << LED_PIO_IDX)   // Mascara para CONTROLARMOS o LED

        /*------------------------OLED1---------------------------*/

#define LED_1_PIO  PIOA                            // periferico que controla o LED 01
#define LED_1_PIO_ID  ID_PIOA                      // ID do periférico PIOA (controla LED 01)
#define LED_1_PIO_IDX  0                           // ID do LED 01 no PIO
#define LED_1_PIO_IDX_MASK  (1 << LED_1_PIO_IDX)   // Mascara para CONTROLARMOS o LED 01

#define LED_2_PIO  PIOC                            // periferico que controla o LED 02
#define LED_2_PIO_ID  ID_PIOC                      // ID do periférico PIOC (controla LED 02)
#define LED_2_PIO_IDX  30                          // ID do LED 02 no PIO
#define LED_2_PIO_IDX_MASK  (1 << LED_2_PIO_IDX)   // Mascara para CONTROLARMOS o LED 02

#define LED_3_PIO  PIOB                            // periferico que controla o LED 03
#define LED_3_PIO_ID  ID_PIOB                      // ID do periférico PIOB (controla LED 03)
#define LED_3_PIO_IDX  2                           // ID do LED 03 no PIO
#define LED_3_PIO_IDX_MASK  (1 << LED_3_PIO_IDX)   // Mascara para CONTROLARMOS o LED 03

/*-------------------------------BUTTONS-------------------------------*/

        /*------------------------SAME70---------------------------*/
		
#define BUT_PIO  PIOA                           // periferico que controla o BUTTON SW300
#define BUT_PIO_ID  ID_PIOA                     // ID do periférico PIOA (controla BUTTON SW300)
#define BUT_PIO_IDX  11                         // ID do BUTTON SW300 no PIO
#define BUT_PIO_IDX_MASK  (1u << BUT_PIO_IDX)   // Mascara para CONTROLARMOS o BUTTON SW300

        /*------------------------OLED1---------------------------*/

#define BUT_1_PIO  PIOD                             // periferico que controla o BUTTON 1
#define BUT_1_PIO_ID  ID_PIOD                       // ID do periférico PIOD (controla BUTTON 1)
#define BUT_1_PIO_IDX  28                           // ID do BUTTON 1 no PIO
#define BUT_1_PIO_IDX_MASK  (1u << BUT_1_PIO_IDX)   // Mascara para CONTROLARMOS o BUTTON 1

#define BUT_2_PIO  PIOC                             // periferico que controla o BUTTON 2
#define BUT_2_PIO_ID  ID_PIOC                       // ID do periférico PIOC (controla BUTTON 2)
#define BUT_2_PIO_IDX  31                           // ID do BUTTON 2 no PIO
#define BUT_2_PIO_IDX_MASK  (1u << BUT_2_PIO_IDX)   // Mascara para CONTROLARMOS o BUTTON 2
		
#define BUT_3_PIO  PIOA                             // periferico que controla o BUTTON 3
#define BUT_3_PIO_ID  ID_PIOA                       // ID do periférico PIOA (controla BUTTON 3)
#define BUT_3_PIO_IDX  19                           // ID do BUTTON 3 no PIO
#define BUT_3_PIO_IDX_MASK  (1u << BUT_3_PIO_IDX)   // Mascara para CONTROLARMOS o BUTTON 3

/*  Default pin configuration (no attribute). */
#define _PIO_DEFAULT             (0u << 0)
/*  The internal pin pull-up is active. */
#define _PIO_PULLUP              (1u << 0)
/*  The internal glitch filter is active. */
#define _PIO_DEGLITCH            (1u << 1)
/*  The internal debouncing filter is active. */
#define _PIO_DEBOUNCE            (1u << 3)


/************************************************************************/
/* constants                                                            */
/************************************************************************/

/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/

/************************************************************************/
/* prototypes                                                           */
/************************************************************************/

void init(void);

/************************************************************************/
/* interrupcoes                                                         */
/************************************************************************/

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

void _pio_set(Pio *p_pio, const uint32_t ul_mask) {
	p_pio->PIO_SODR = ul_mask;
}

void _pio_clear(Pio *p_pio, const uint32_t ul_mask) {
	p_pio->PIO_CODR = ul_mask;
}

void _pio_pull_up(Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_pull_up_enable) {
	if (ul_pull_up_enable) {
		p_pio->PIO_PUER = ul_mask;
	} else {
		p_pio->PIO_PUDR = ul_mask;
	}
 }
 
void _pio_set_input(Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_attribute) {
	p_pio->PIO_ODR = ul_mask;
	p_pio->PIO_PER = ul_mask;
	
	_pio_pull_up(p_pio, ul_mask, ul_attribute & _PIO_PULLUP);
	
	if (ul_attribute & (_PIO_DEGLITCH | _PIO_DEBOUNCE)) {
		p_pio->PIO_IFER = ul_mask;
		} else {
		p_pio->PIO_IFDR = ul_mask;
	}
	
	if (ul_attribute & _PIO_DEGLITCH) {
		p_pio->PIO_IFSCDR = ul_mask;
		} else {
		if (ul_attribute & _PIO_DEBOUNCE) {
			p_pio->PIO_IFSCER = ul_mask;
		}
	}
}

void _pio_set_output(Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_default_level, const uint32_t ul_multidrive_enable, const uint32_t ul_pull_up_enable) {
	p_pio->PIO_OER = ul_mask;
	p_pio->PIO_PER = ul_mask;
	
	_pio_pull_up(p_pio, ul_mask, ul_pull_up_enable);

	if (ul_multidrive_enable) {
		p_pio->PIO_MDER = ul_mask;
	} else {
		p_pio->PIO_MDDR = ul_mask;
	}

	if (ul_default_level) {
		_pio_set(p_pio, ul_mask);
	} else {
		_pio_clear(p_pio, ul_mask);
	}
}

uint32_t _pio_get(Pio *p_pio, const pio_type_t ul_type, const uint32_t ul_mask) {
	uint32_t ul_reg;
	
	if (ul_type == PIO_OUTPUT_0) {
		ul_reg = p_pio->PIO_ODSR;
	} else if (ul_type == PIO_INPUT) {
		ul_reg = p_pio->PIO_PDSR;
	}

	if ((ul_reg & ul_mask) == 0) {
		return 0;
	} else {
		return 1;
	}
}

void _delay_ms(int ms) {
	for (int i = 0; i < 150000*ms; i++) {
		asm("NOP");
	}
}

// Função de inicialização do uC
void init(void) {
	// Initialize the board clock
	sysclk_init();

	// Desativa WatchDog Timer
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	// Ativa o PIO na qual o LED foi conectado
	// para que possamos controlar o LED.
	pmc_enable_periph_clk(LED_1_PIO_ID);
	pmc_enable_periph_clk(LED_2_PIO_ID);
	pmc_enable_periph_clk(LED_3_PIO_ID);
	
	// Inicializa PIO do botao
	pmc_enable_periph_clk(BUT_1_PIO_ID);
	pmc_enable_periph_clk(BUT_2_PIO_ID);
	pmc_enable_periph_clk(BUT_3_PIO_ID);
	
	//Inicializa PA0 como saída
	_pio_set_output(LED_1_PIO, LED_1_PIO_IDX_MASK, 0, 0, 0);
	
	//Inicializa PC30 como saída
	_pio_set_output(LED_2_PIO, LED_2_PIO_IDX_MASK, 0, 0, 0);
	
	//Inicializa PB2 como saída
	_pio_set_output(LED_3_PIO, LED_3_PIO_IDX_MASK, 0, 0, 0);
	
	//Inicializa PD28 como entrada
	_pio_set_input(BUT_1_PIO, BUT_1_PIO_IDX_MASK, _PIO_PULLUP | _PIO_DEBOUNCE);
	
	//Inicializa PC31 como entrada
	_pio_set_input(BUT_2_PIO, BUT_2_PIO_IDX_MASK, _PIO_PULLUP | _PIO_DEBOUNCE);
	
	//Inicializa PA19 como entrada
	_pio_set_input(BUT_3_PIO, BUT_3_PIO_IDX_MASK, _PIO_PULLUP | _PIO_DEBOUNCE);
}

/************************************************************************/
/* Main                                                                 */
/************************************************************************/

// Funcao principal chamada na inicalizacao do uC.
int main(void) {
	// inicializa sistema e IOs
	init();

	// super loop
	// aplicacoes embarcadas não devem sair do while(1).
	while(1) {
		while (!_pio_get(BUT_1_PIO, PIO_INPUT, BUT_1_PIO_IDX_MASK)) {
			_pio_clear(LED_1_PIO, LED_1_PIO_IDX_MASK);
			_delay_ms(100);
			_pio_set(LED_1_PIO, LED_1_PIO_IDX_MASK);
			_delay_ms(100);
		}
		_pio_set(LED_1_PIO, LED_1_PIO_IDX_MASK);
		while (!_pio_get(BUT_2_PIO, PIO_INPUT, BUT_2_PIO_IDX_MASK)) {
			_pio_clear(LED_2_PIO, LED_2_PIO_IDX_MASK);
			_delay_ms(200);
			_pio_set(LED_2_PIO, LED_2_PIO_IDX_MASK);
			_delay_ms(200);
		}
		_pio_set(LED_2_PIO, LED_2_PIO_IDX_MASK);
		while (!_pio_get(BUT_3_PIO, PIO_INPUT, BUT_3_PIO_IDX_MASK)) {
			_pio_clear(LED_3_PIO, LED_3_PIO_IDX_MASK);
			_delay_ms(400);
			_pio_set(LED_3_PIO, LED_3_PIO_IDX_MASK);
			_delay_ms(400);
		}
		_pio_set(LED_3_PIO, LED_3_PIO_IDX_MASK);
		
	}
	// Nunca devemos chegar aqui !
	return 0;
}
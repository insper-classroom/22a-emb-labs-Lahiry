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

// Função de inicialização do uC
void init(void){
	// Initialize the board clock
	sysclk_init();

	// Desativa WatchDog Timer
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	// Ativa o PIO na qual o LED foi conectado
	// para que possamos controlar o LED.
	pmc_enable_periph_clk(LED_PIO_ID);
	pmc_enable_periph_clk(LED_1_PIO_ID);
	pmc_enable_periph_clk(LED_2_PIO_ID);
	pmc_enable_periph_clk(LED_3_PIO_ID);
	
	// Inicializa PIO do botao
	pmc_enable_periph_clk(BUT_PIO_ID);
	pmc_enable_periph_clk(BUT_1_PIO_ID);
	pmc_enable_periph_clk(BUT_2_PIO_ID);
	pmc_enable_periph_clk(BUT_3_PIO_ID);
	
	//Inicializa PC8 como saída
	pio_set_output(LED_PIO, LED_PIO_IDX_MASK, 0, 0, 0);
	
	//Inicializa PA0 como saída
	pio_set_output(LED_1_PIO, LED_1_PIO_IDX_MASK, 0, 0, 0);
	
	//Inicializa PC30 como saída
	pio_set_output(LED_2_PIO, LED_2_PIO_IDX_MASK, 0, 0, 0);
	
	//Inicializa PB2 como saída
	pio_set_output(LED_3_PIO, LED_3_PIO_IDX_MASK, 0, 0, 0);
	
	//Inicializa PA11 como entrada
	pio_set_input(BUT_PIO, BUT_PIO_IDX_MASK, PIO_DEFAULT);
	
	//Inicializa PD28 como entrada
	pio_set_input(BUT_1_PIO, BUT_1_PIO_IDX_MASK, PIO_DEFAULT);
	
	//Inicializa PC31 como entrada
	pio_set_input(BUT_2_PIO, BUT_2_PIO_IDX_MASK, PIO_DEFAULT);
	
	//Inicializa PA19 como entrada
	pio_set_input(BUT_3_PIO, BUT_3_PIO_IDX_MASK, PIO_DEFAULT);
	
	pio_pull_up(BUT_PIO, BUT_PIO_IDX_MASK, 1);
	pio_pull_up(BUT_1_PIO, BUT_1_PIO_IDX_MASK, 1);
	pio_pull_up(BUT_2_PIO, BUT_2_PIO_IDX_MASK, 1);
	pio_pull_up(BUT_3_PIO, BUT_3_PIO_IDX_MASK, 1);
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
		if (!pio_get(BUT_PIO, PIO_INPUT, BUT_PIO_IDX_MASK)) {
			for(int i = 0; i < 5; i++) {
				pio_clear(LED_PIO, LED_PIO_IDX_MASK);
				delay_ms(200);
				pio_set(LED_PIO, LED_PIO_IDX_MASK);
				delay_ms(200);
			}
		}
		
		pio_set(LED_PIO, LED_PIO_IDX_MASK);
		
		while (!pio_get(BUT_1_PIO, PIO_INPUT, BUT_1_PIO_IDX_MASK)) {
			pio_clear(LED_1_PIO, LED_1_PIO_IDX_MASK);
			delay_ms(100);
			pio_set(LED_1_PIO, LED_1_PIO_IDX_MASK);
			delay_ms(100);
		}
		pio_set(LED_1_PIO, LED_1_PIO_IDX_MASK);
		while (!pio_get(BUT_2_PIO, PIO_INPUT, BUT_2_PIO_IDX_MASK)) {
			pio_clear(LED_2_PIO, LED_2_PIO_IDX_MASK);
			delay_ms(200);
			pio_set(LED_2_PIO, LED_2_PIO_IDX_MASK);
			delay_ms(200);
		}
		pio_set(LED_2_PIO, LED_2_PIO_IDX_MASK);
		while (!pio_get(BUT_3_PIO, PIO_INPUT, BUT_3_PIO_IDX_MASK)) {
			pio_clear(LED_3_PIO, LED_3_PIO_IDX_MASK);
			delay_ms(400);
			pio_set(LED_3_PIO, LED_3_PIO_IDX_MASK);
			delay_ms(400);
		}
		pio_set(LED_3_PIO, LED_3_PIO_IDX_MASK);
		
	}
	// Nunca devemos chegar aqui !
	return 0;
}
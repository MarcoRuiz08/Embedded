/*
 * PIT_KinetisL.c
 *
 *  Created on: Nov 15, 2013
 *      Author: CARLOS
 */


#include "derivative.h"
#include "PIT_KinetisL.h"


void Init_PIT(void){
	
	//habilitamos el reloj del PIT
	SIM_SCGC6 |= (SIM_SCGC6_PIT_MASK);
	PIT_MCR = (MODULE_ENABLE << PIT_MCR_MDIS_SHIFT);
	PIT_MCR |= (ENABLE_FREEZE << PIT_MCR_FRZ_SHIFT);
	TIMER_PIT_LDVAL = (TIMER_LOAD_VALUE);
	TIMER_PIT_TCTRL |= (CHAIN_MODE_DISABLE << PIT_TCTRL_CHN_SHIFT) | (INTERRUPT_TIMER_ENABLE << PIT_TCTRL_TIE_SHIFT) |
			(TIMER_PIT_ENABLE << PIT_TCTRL_TEN_SHIFT);
	
	//limpiamos algun pendiente en el PIT y habilitamos la interrupcion
	NVIC_ICPR |= (1<<22);
	NVIC_ISER |= (1<<22);
	
	
}

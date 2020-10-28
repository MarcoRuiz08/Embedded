/*
 * UART.c
 *
 *  Created on: Nov 13, 2013
 *      Author: CARLOS
 */

#include "derivative.h"
#include "UART_KinetisL.h"



void Init_Uart(void){
	//habilitamos el reloj para la UART
	SIM_SCGC4 |= (SIM_SCGC4_UART0_MASK);
	//seleccionamos la funte de reloj para la UART Y seleccionamos el MCGPLLCLOCK
	SIM_SOPT2 |= (UART_CLOCK_FLL_OR_PLL << SIM_SOPT2_UART0SRC_SHIFT) | (SIM_SOPT2_PLLFLLSEL_MASK);
	//funcion para configurar el baudrate
	UART_Configure_BaudRate( UART_ENABLE_EDGE_INTERRUPT, UART_1STOP_BIT, UART_BAUDRATEH, UART_BAUDRATEL);
	//funcion para configurar C4
	UART_ConfigureC4(UART_OSR);
	//funcion para configurar C1
	UART_ConfigureC1( UART_MODE_8BITS, UART_PARITY_DISABLE, UART_EVEN_PARITY);
	//funcion para configurar C2
	UART_ConfigureC2( UART_IRQ_DISABLE_IRQ_EMPTY_BUFFER, UART_IRQ_DISABLE_TRANS_COMPLETE, UART_IRQ_DISABLE_BUFFER_FULL,
			UART_ENABLE_TRANSMITTER, UART_ENABLE_RECEIVER);
	//funcion para configurar C3
	UART_ConfigureC3( UART_DATA_TRANS_NOT_INVERTED, UART_IRQ_DISABLE_ERROR_OVERRUN, UART_IRQ_DISABLE_ERROR_NOISE,
			UART_IRQ_DISABLE_ERROR_FRAME, UART_IRQ_DISABLE_ERROR_PARITY);
	
	//limpiamos algun pendiente en la UART y habilitamos la interrupcion
	NVIC_ICPR |= (1<<12);
	NVIC_ISER |= (1<<12);
	
}


void UART_Configure_BaudRate(uint8_t Interrup_Edge, uint8_t Stop_bits, uint8_t BaudRateH_Div,uint8_t BaudRateL_Div){
	
	UART_BDH |= (Interrup_Edge << UART_BDH_RXEDGIE_SHIFT);
	UART_BDH |= (Stop_bits << UART_BDH_SBNS_SHIFT);
	UART_BDH |= (BaudRateH_Div << UART_BDH_SBR_SHIFT);
	UART_BDL = BaudRateL_Div;
	
	
}


void UART_ConfigureC1(uint8_t Sel_Mod_8_9, uint8_t Parity_Enable, uint8_t Mode_Parity){
	
	UART_C1 |= (Sel_Mod_8_9 << UART_C1_M_SHIFT) | (Parity_Enable << UART_C1_PE_SHIFT) | 
			(Mode_Parity << UART_C1_PT_SHIFT);
}


void UART_ConfigureC2(uint8_t Interrupt_Empty_buffer, uint8_t Interrupt_Complete_Trans, 
			uint8_t Interrupt_Receiver_full, uint8_t Enable_Transmitter, uint8_t Enable_Receiver){
	
	UART_C2 |= (Interrupt_Empty_buffer << UART_C2_TIE_SHIFT) | (Interrupt_Complete_Trans << UART_C2_TCIE_SHIFT) |
			(Interrupt_Receiver_full << UART_C2_RIE_SHIFT) | (Enable_Transmitter << UART_C2_TE_SHIFT) | 
			(Enable_Receiver << UART_C2_RE_SHIFT);
}


void UART_ConfigureC3(uint8_t Inverted_Data, uint8_t Interrupt_Overrun, uint8_t Interrupt_Noise, 
			uint8_t Interrupt_Framing, uint8_t Interrupt_Parity){
	UART_C3 |= (Inverted_Data << UART_C3_TXINV_SHIFT) | (Interrupt_Overrun << UART_C3_ORIE_SHIFT) | 
			(Interrupt_Noise << UART_C3_NEIE_SHIFT) | (Interrupt_Framing << UART_C3_FEIE_SHIFT) | 
			(Interrupt_Parity << UART_C3_PEIE_SHIFT);
}


void UART_ConfigureC4(uint8_t Osr){
	UART_C4 = (Osr << UART0_C4_OSR_SHIFT);
}







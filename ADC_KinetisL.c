/*
 * ADC_KinetisL.c
 *
 *  Created on: Oct 3, 2013
 *      Author: CARLOS
 */

#include "derivative.h"
#include "ADC_KinetisL.h"


void Init_ADC(){
	// <<<<<<<<<<<	INICIALIZACION Y CONFIGURACION DEL ADC	>>>>>>>>>>>>>>>>>>>>>
	//				  *fuente de reloj	 *divisor del reloj	 *resolucion
	ADC_ConfigureCFG1 (ADC_CLK_BUS, ADC_CLK_DIV1, ADC_RESOLUTION_12);	
	//				  *trigger de la conversion
	ADC_ConfigureSC2 (ADC_SW_TRIGGER_ENABLE);
	//				 *conversion continua	 *promedio por HW		*muestras para el promedio
	ADC_ConfigureSC3 (ADC_CONT_CONV_DISABLE, ADC_HW_AVERAGE_DISABLE, ADC_AVGS_16SAMPLES);
	//				 *interrupcion por conversion  *modo diferencial
	ADC_ConfigureSC1 (ADC_INTERRUPT_ENABLE, ADC_DIFF_DISABLE); 

	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
}


void ADC_ConfigureCFG1 (uint8_t Input_Clock_Sel, uint8_t Clock_Divide_Sel, uint8_t ADC_Resolution)
{
	//habilitamos el reloj del ADC0
	SIM_SCGC6 |= (SIM_SCGC6_ADC0_MASK);
	//configuramos el "Configuration Register 1"
	ADC0_CFG1 |= (Clock_Divide_Sel << ADC_CFG1_ADIV_SHIFT) | (ADC_Resolution << ADC_CFG1_MODE_SHIFT) | \
			(Input_Clock_Sel << ADC_CFG1_ADICLK_SHIFT) | (1 << ADC_CFG1_ADLSMP_MASK);
	
}


void ADC_ConfigureSC2 (uint8_t Conv_Trigger_Sel)
{
	//configuramos el "Status and Control Register 2"
	ADC0_SC2 |= (Conv_Trigger_Sel << ADC_SC2_ADTRG_SHIFT);
	
}

void ADC_ConfigureSC3 (uint8_t Continuous_Conv_Enable, uint8_t HW_Average_Enable, uint8_t HW_Average_Sel)
{
	//configuramos el "Status and Control Register 3"
	ADC0_SC3 |= (Continuous_Conv_Enable << ADC_SC3_ADCO_SHIFT) | (HW_Average_Enable << ADC_SC3_AVGE_SHIFT) | \
			 	(HW_Average_Sel << ADC_SC3_AVGS_SHIFT);
}

void ADC_ConfigureSC1 (uint8_t Interrupt_Enable, uint8_t Differencial_Mode_Enable)
{
	//configuramos el "Status and Control Register 1"
	ADC0_SC1A |= (Interrupt_Enable << ADC_SC1_AIEN_SHIFT) | (Differencial_Mode_Enable << ADC_SC1_DIFF_SHIFT);
	//limpiamos pendientes en la interrupcion
	NVIC_ICPR |= (1<<15);
	//activamos interrupciones con el ADC
	NVIC_ISER |= (1<<15);

}

void ADC_Conversion_Enable (uint8_t Channel_Sel)
{
	uint32_t ADC_SC1A_Temp=0;
	//configuramos el canal para la conversion
	ADC_SC1A_Temp = (ADC0_SC1A & 0b1100000);
	ADC_SC1A_Temp |= Channel_Sel;
	ADC0_SC1A = (ADC_SC1A_Temp);
	
}

void Read_ADC_Value(void)
{
	//leemos el dato del ADC y lo almacenamos en la variable Conv_ADC
	Conv_ADC = ADC0_RA;
	COCO=1;
}
	
	
	
	



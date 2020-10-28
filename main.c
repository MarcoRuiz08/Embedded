/*
  * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /*include peripheral declarations*/
#include "Clock_KinetisL.h" /*include header file of Clock_KinetisL*/
#include "ADC_KinetisL.h" /*include header file of ADC_KinetisL*/
#include "PIT_KinetisL.h" /*include header file of PIT_Kinetis*/
#include "DAC_KinetisL.h" /*include header file of DAC_KinetisL*/
#include "UART_KinetisL.h" /*include header file of UART_KinetisL*/

#define ADD_GAIN_LP		2
#define SUB_GAIN_LP		4
#define ADD_GAIN_HP		6
#define SUB_GAIN_HP		7
#define ADD_GAIN_BP   3
#define SUB_GAIN_BP   5
#define OFFSET		1316

//PTE_23 ADC
//PTE_30 DAC

void Interrupt_PIT(void);	/*PIT interrupt*/
void Sampling(void);	/*function for sampling*/
void Setting_GPIO(void);	/*function to set all GPIOs*/
void FILTER(); /*function to do the equation of ECHO*/
void Gain_Filters(uint8_t);
void SetUp_Array(void);

uint8_t Sampling_Flag=0; /*flag for sample*/
uint16_t DAC=0;	/*variable to send a value to DAC*/
uint8_t DAC_Low=0; /*DAC´s variable*/
uint8_t DAC_High=0; /*DAC´s variable*/
uint16_t x[7]={0}; /*array which contains the values of the sampling of ADC*/
uint16_t Array_Position=0;
/*uint8_t Gain_HP=10;
uint8_t Gain_LP=10;
uint16_t Coef_LP[]={0x0A09,0x16AB,0x1C2E,0x2351,0x1C2E,0x16AB,0x0A09}; //coeficientes para pasabajas
uint16_t Coef_HP[]={0x0B56,0x199A,0x1102,0x6345,0x1102,0x199A,0x0B56}; //Coeficientes para pasa altas
uint16_t q,g;
*/
 uint16_t Coef_LP[]={0x0127, 0x0127, 0x0127, 0x0127, 0x0127, 0x0127, 0x0127}; //coeficientes para pasabajas
 uint16_t Coef_BP[]={0x01DE, 0x1024, 0x1C8B, 0x2172, 0x1C8B, 0x1024, 0x01DE}; //Coeficientes pasa bandas
 uint16_t Coef_HP[]={0x077A, 0x1430, 0x1EB2, 0x5D3F, 0x1EB2, 0x1430, 0x077A}; //Coeficientes para pasa altas
   uint16_t q,g,h;
   uint8_t Gain_HP=10;
   uint8_t Gain_LP=10;
   uint8_t Gain_BP =10;


int main(void)
{
	Init_Clock(); /*setup the clock*/
	Setting_GPIO(); /*setup GPIOs*/
	Init_ADC(); /*setup ADC*/
	Init_DAC(); /*setup DAC*/
	Init_PIT(); /*setup PIT*/
	
	for(;;) {	  
		if(Sampling_Flag==1){
			Sampling();	/*call to Sampling function*/
			Sampling_Flag=0; /*clean flag*/
		}
	}
	
	return 0;
}

void Setting_GPIO(){
	//declaramos los pines de los switch como GPIO, activamos PULL, ponemos PULL-UP, ponemos interrupcion en flanco de bajada
	PORTD_PCR2 = (1<<PORT_PCR_MUX_SHIFT) | (1<<PORT_PCR_PE_SHIFT) | (1<<PORT_PCR_PS_SHIFT) | (0xA<<PORT_PCR_IRQC_SHIFT);
	PORTD_PCR3 = (1<<PORT_PCR_MUX_SHIFT) | (1<<PORT_PCR_PE_SHIFT) | (1<<PORT_PCR_PS_SHIFT) | (0xA<<PORT_PCR_IRQC_SHIFT);
	PORTD_PCR4 = (1<<PORT_PCR_MUX_SHIFT) | (1<<PORT_PCR_PE_SHIFT) | (1<<PORT_PCR_PS_SHIFT) | (0xA<<PORT_PCR_IRQC_SHIFT);
	PORTD_PCR5 = (1<<PORT_PCR_MUX_SHIFT) | (1<<PORT_PCR_PE_SHIFT) | (1<<PORT_PCR_PS_SHIFT) | (0xA<<PORT_PCR_IRQC_SHIFT);
	PORTD_PCR6 = (1<<PORT_PCR_MUX_SHIFT) | (1<<PORT_PCR_PE_SHIFT) | (1<<PORT_PCR_PS_SHIFT) | (0xA<<PORT_PCR_IRQC_SHIFT); 
	PORTD_PCR7 = (1<<PORT_PCR_MUX_SHIFT) | (1<<PORT_PCR_PE_SHIFT) | (1<<PORT_PCR_PS_SHIFT) | (0xA<<PORT_PCR_IRQC_SHIFT);
	
	
	
	
	//declaramos los pines de los switch´s como entradas
	GPIOD_PDDR |= (0<<ADD_GAIN_LP) | (0<<SUB_GAIN_LP);
	GPIOD_PDDR |= (0<<ADD_GAIN_BP) | (0<<SUB_GAIN_BP);
	GPIOD_PDDR |= (0<<ADD_GAIN_HP) | (0<<SUB_GAIN_HP);
	
	//limpiamos algun pendiente en el puerto D
	NVIC_ICPR |= (1<<31);
	
	//habilitamos interrupciones en el puerto D
	NVIC_ISER |= (1<<31);
	
	/*set pin to output DAC*/
	PORTE_PCR30 = (0<<PORT_PCR_MUX_SHIFT);
}

void Interrupt_PIT(void){
	/*clean interrupt flag of PIT*/
	CLEAR_TIF;
	/*set flag to indicate that there must be a sampling*/
	Sampling_Flag=1;
	
}

void Interrupt_Port_CD(){
	//PORTD_ISFR |= (1<<ADD_GAIN_LP) | (1<<ADD_GAIN_HP) | (1<<SUB_GAIN_LP) | (1<<SUB_GAIN_HP);
	PORTD_ISFR |= (1<<ADD_GAIN_LP) | (1<<ADD_GAIN_BP) | (1<<ADD_GAIN_HP) |(1<<SUB_GAIN_LP) | (1<<SUB_GAIN_BP) | (1<<SUB_GAIN_HP);
	
	//desabilitamos interrupciones en SW1 y SW2 y limpiamos los pendientes
	NVIC_ICPR |= (1<<31);
	NVIC_ICER &= (1<<31);
	if(((GPIOD_PDIR & (1<<ADD_GAIN_LP))>>ADD_GAIN_LP == 0)){
		Gain_Filters(ADD_GAIN_LP);
	}
	else if(((GPIOD_PDIR & (1<<SUB_GAIN_LP))>>SUB_GAIN_LP == 0)){
		Gain_Filters(SUB_GAIN_LP);
	}
	
	  else if(((GPIOD_PDIR & (1<<ADD_GAIN_BP))>>ADD_GAIN_BP == 0)){
		Gain_Filters(ADD_GAIN_BP);
	}
	else if(((GPIOD_PDIR & (1<<SUB_GAIN_BP))>>SUB_GAIN_BP == 0)){
		Gain_Filters(SUB_GAIN_BP);
	}
	else if(((GPIOD_PDIR & (1<<ADD_GAIN_HP))>>ADD_GAIN_HP == 0)){
		Gain_Filters(ADD_GAIN_HP);
	}
	else if(((GPIOD_PDIR & (1<<SUB_GAIN_HP))>>SUB_GAIN_HP == 0)){
		Gain_Filters(SUB_GAIN_HP);
	}
	
	/*enable the interrupts and clean the pending*/
	NVIC_ICPR |= (1<<31);
	NVIC_ISER |= (1<<31);
}

void Sampling(void){
	/*start the ADC conversation*/
	ADC_Conversion_Enable(ADC_CH_AD7);
	while(COCO==0);
	COCO=0;
	/*modify the value of ADC*/
	DAC=Conv_ADC;
	if(DAC>=OFFSET){	
		DAC=DAC-OFFSET;
		FILTER();
		DAC=DAC+OFFSET;
	}
	else if(DAC<OFFSET){ 
		DAC=OFFSET-DAC;
		FILTER();
		DAC=OFFSET-DAC;
	}
	/*move the value to DAC*/
	DAC_Low=DAC & 0b0000000011111111;
	DAC_High=(DAC & 0b1111111100000000)>>8;
	DAC_VALUE_L =DAC_Low;		/*up to 256*/
	DAC_VALUE_H =DAC_High;		/*up to 16*/
}

void FILTER(){ 
	//uint32_t temp;
	SetUp_Array();
	x[0]=DAC;
	q=(uint16_t)((x[0]*Coef_LP[0])>>15)+(uint16_t)((x[1]*Coef_LP[1])>>15)+(uint16_t)((x[2]*Coef_LP[2])>>15)+(uint16_t)((x[3]*Coef_LP[3])>>15)+(uint16_t)((x[4]*Coef_LP[4])>>15)+(uint16_t)((x[5]*Coef_LP[5])>>15)+(uint16_t)((x[6]*Coef_LP[6])>>15);
	h=(uint16_t)((x[0]*Coef_BP[0])>>15)+(uint16_t)((x[1]*Coef_BP[1])>>15)+(uint16_t)((x[2]*Coef_BP[2])>>15)+(uint16_t)((x[3]*Coef_BP[3])>>15)+(uint16_t)((x[4]*Coef_BP[4])>>15)+(uint16_t)((x[5]*Coef_BP[5])>>15)+(uint16_t)((x[6]*Coef_BP[6])>>15);
	g=-(uint16_t)((x[0]*Coef_HP[0])>>15)-(uint16_t)((x[1]*Coef_HP[1])>>15)-(uint16_t)((x[2]*Coef_HP[2])>>15)+(uint16_t)((x[3]*Coef_HP[3])>>15)-(uint16_t)((x[4]*Coef_HP[4])>>15)-(uint16_t)((x[5]*Coef_HP[5])>>15)-(uint16_t)((x[6]*Coef_HP[6])>>15);
	
	//q=Multiplication(q,Gain_LP); NO SIRVE
	//g=Multiplication(g,Gain_HP);	NO SIRVE
	q=(q*Gain_LP)/10;
	h = (q*Gain_BP)/10; //nuevo
	g=(g*Gain_HP)/10;
	//DAC=q+g;
	DAC=q+h+g;	//nuevo
}

void Gain_Filters(uint8_t button){
	switch(button){
	case ADD_GAIN_LP:
		if(Gain_LP<12){ //si la ganancia es menor a 1.9
			Gain_LP=Gain_LP+1; //aumentamos 0.1(0xCDD)
		}
		break;
	case ADD_GAIN_HP:
		if(Gain_HP<12){ //si la ganancia es menor a 1.9
			Gain_HP=Gain_HP+1; //aumentamos 0.1(0xCDD)
		}
		break;
	case ADD_GAIN_BP:
		if(Gain_BP<12){ //si la ganancia es menor a 1.9
			Gain_LP=Gain_BP+1; //aumentamos 0.1(0xCDD)
		}
		break;
	 
	case SUB_GAIN_LP:
		if(Gain_LP>1){ //si la ganancia es mayor a 0.1
			Gain_LP=Gain_LP-1; //Disminuimos 0.1(0xCDD)
		}
		break;
		case SUB_GAIN_BP:
		if(Gain_BP>1){ //si la ganancia es mayor a 0.1
			Gain_BP=Gain_BP-1; //Disminuimos 0.1(0xCDD)
		}
		break;
	case SUB_GAIN_HP:
		if(Gain_HP>1){ //si la ganancia es menor a 1.9
			Gain_HP=Gain_HP-1; //Disminuimos 0.1(0xCDD)
		}
		break;
	}
}

void SetUp_Array(void){
	uint8_t i;
	for(i=(sizeof(x)/2)-1;i>0;i--){
		x[i]=x[i-1];
	}
}


/*
 * DAC_KinetisL.c
 *
 *  Created on: Mar 25, 2014
 *      Author: Carlos
 */

#include "derivative.h"
#include "DAC_KinetisL.h"

void Init_DAC(void){
	/*habilitamos el reloj del DAC*/
	SIM_SCGC6 |= (SIM_SCGC6_DAC0_MASK);
	DAC_REF_1;
	HIGH_POWER;
	DAC_ENABLE;
	DAC_VALUE_L =255;		/*llega hasta 256*/
	DAC_VALUE_H =0b1111;		/*llega hasta 16*/
}

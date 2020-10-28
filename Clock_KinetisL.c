/*
 * Clock_KinetisL.c
 *
 *  Created on: Dec 6, 2013
 *      Author: CARLOS
 */

#include "Clock_KinetisL.h"
#include "derivative.h"

void Init_Clock(){
	//habilitamos peticion de reloj externo
	OSC0_CR = OSC_CR_ERCLKEN_MASK;
	//ponemos a RANGE0 para alta frecuencia, y EREFS0 para indicar que se usa cristal externo
	MCG_C2 = (MCG_C2_RANGE0(2) | (MCG_C2_EREFS0_MASK));
	//ponemos como fuente de reloj el reloj externo, FRDIV igual a 3 para que divida entre 256, y 
	//hacemos 0 IREFS para que la entrada al FLL sea la del reloj externo dividido entre 256 
	MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3) | (0<<MCG_C1_IREFS_SHIFT); 
	//esperamos a que el cristal de IREFST sea inicializado
	while (!(MCG_S & (MCG_S_OSCINIT0_MASK))); 
	//esperamos a que la referencia extrena sea la fuente del reloj
	while (MCG_S & (MCG_S_IREFST_MASK)); 
	//esperamos a que el reloj externo sea la fuente de MCGOUTCLK
	while ((MCG_S & MCG_S_CLKST_MASK) != 0x08);
	//dividimos la referencia externa del PLL entre 2 = 4MHz
	MCG_C5 = MCG_C5_PRDIV0(1);
	//ponemos el PLLS en 1 para seleccionar la salida del PLL, y VDIV0 es 0 para que multiplique por 24 = 96MHz
	MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV0(0);
	//esperamos a que la fuente del PLLS sea la del PLL
	while (!(MCG_S & (MCG_S_LOCK0_MASK | MCG_S_PLLST_MASK)));
	//ponemos a OUTDIV1 para que divida entre 2, entonces CLK del Core = 48MHz, y 
	//ponemos el OUTDIV4 para que divida entre 2, entonces CLK del Bus = 24MHz
	SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(1) | SIM_CLKDIV1_OUTDIV4(1);
	//ponemos a CLKS en cero para que seleccione la salida del PLL
	MCG_C1 = 0x18;
	//esperamos a que la salida del PLL esta como reloj
	while ((MCG_S & MCG_S_CLKST_MASK) != 0x0C);
	//////ACTIVAMOS LOS RELOJES DE LOS PUERTOS (CLOCK GATING)
	//habilitamos los relojes de los puestos
	SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK) | (SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTC_MASK) | (SIM_SCGC5_PORTD_MASK) |
			(SIM_SCGC5_PORTE_MASK);

}

#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include <stdint.h>

#define APB2_BASEADDR		0x40010000UL

#define EXTI_BASEADDR		(APB2_BASEADDR + 0x3C00)

typedef struct
{
	volatile uint32_t IMR;						/* Interrupt mask register */
	volatile uint32_t EMR;						/* Event mask register */
	volatile uint32_t RTSR;						/* Rising trigger selection register */
	volatile uint32_t FTSR;						/* Falling trigger selection register */
	volatile uint32_t SWIER;					/* Software interrupt event register */
	volatile uint32_t PR;						/* Pending register */
} EXTI_RegDef_t;

#define EXTI		((EXTI_RegDef_t *)EXTI_BASEADDR)

extern volatile uint8_t motion_detected;

#endif // PERIPHERALS_H

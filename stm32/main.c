#include <stdint.h>
#include <stdbool.h>

/**
 * Address macros
 */

#define AHB1_BASEADDR		0x40020000UL

#define GPIOA_BASEADDR		(AHB1_BASEADDR + 0x0000)
#define GPIOB_BASEADDR		(AHB1_BASEADDR + 0x0400)
#define GPIOC_BASEADDR		(AHB1_BASEADDR + 0x0800)
#define GPIOD_BASEADDR		(AHB1_BASEADDR + 0x0C00)
#define GPIOE_BASEADDR		(AHB1_BASEADDR + 0x1000)
#define GPIOF_BASEADDR		(AHB1_BASEADDR + 0x1400)
#define GPIOG_BASEADDR		(AHB1_BASEADDR + 0x1800)
#define GPIOH_BASEADDR		(AHB1_BASEADDR + 0x1C00)

#define RCC_BASEADDR		(AHB1_BASEADDR + 0x3800)

/**
 * Peripheral register structs
 */

typedef struct 
{
	volatile uint32_t MODER;					/*  */
	volatile uint32_t OTYPER;					/*  */
	volatile uint32_t OSPEEDR;					/*  */
	volatile uint32_t PUPDR;					/*  */
	volatile uint32_t IDR;						/*  */
	volatile uint32_t ODR;						/*  */
	volatile uint32_t BSRR;						/*  */
	volatile uint32_t LCKR;						/*  */
	volatile uint32_t AFR[2];					/*  */
} GPIO_RegDef_t;

typedef struct
{
	volatile uint32_t CR;						/*  */
	volatile uint32_t PLL_CFGR;					/*  */
	volatile uint32_t CFGR;						/*  */
	volatile uint32_t CIR;						/*  */
	volatile uint32_t AHB1RSTR;					/*  */
	volatile uint32_t AHB2RSTR;					/*  */
	volatile uint32_t AHB3RSTR;					/*  */
	volatile uint32_t RESERVED0;				/*  */
	volatile uint32_t APB1RSTR;					/*  */
	volatile uint32_t APB2RSTR;					/*  */
	volatile uint32_t RESERVED1[2];				/*  */
	volatile uint32_t AHB1ENR;					/*  */
	volatile uint32_t AHB2ENR;					/*  */
	volatile uint32_t AHB3ENR;					/*  */
	volatile uint32_t RESERVED2;				/*  */
	volatile uint32_t APB1ENR;					/*  */
	volatile uint32_t APB2ENR;					/*  */
	volatile uint32_t RESERVED3[2];				/*  */
	volatile uint32_t AHB1LPENR;				/*  */
	volatile uint32_t AHB2LPENR;				/*  */
	volatile uint32_t AHB3LPENR;				/*  */
	volatile uint32_t RESERVED4;				/*  */
	volatile uint32_t APB1LPENR;				/*  */
	volatile uint32_t APB2LPENR;				/*  */
	volatile uint32_t RESERVED5[2];				/*  */
	volatile uint32_t BDCR;						/*  */
	volatile uint32_t CSR;						/*  */
	volatile uint32_t RESERVED6[2];				/*  */
	volatile uint32_t SSCGR;					/*  */
	volatile uint32_t PLLI2SCFGR;				/*  */
	volatile uint32_t PLLSAICFGR;				/*  */
	volatile uint32_t DCKCFGR;					/*  */
	volatile uint32_t CKGATENR;					/*  */
	volatile uint32_t DCKCFGR2;					/*  */
} RCC_RegDef_t;

/**
 * Peripheral macros
 */

#define GPIOA		((GPIO_RegDef_t *)GPIOA_BASEADDR)

#define RCC			((RCC_RegDef_t *)RCC_BASEADDR)

int main(void)
{
	RCC->AHB1ENR |= (1 << 0);

	GPIOA->MODER |= (1 << 10);

	for (;;) {
		GPIOA->BSRR = (1 << 5);
		for (uint32_t i = 0; i < 1000000; i++) {}
		GPIOA->BSRR = (1 << (5 + 16));
		for (uint32_t i = 0; i < 1000000; i++) {}
	}

	return 0;
}

#include <stdint.h>
#include <stdbool.h>

/**
 * Bus address macros
 */

#define AHB1_BASEADDR		0x40020000UL
#define APB1_BASEADDR		0x40000000UL
#define APB2_BASEADDR		0x40010000UL

/*
 * Peripheral address macros
 */

#define RCC_BASEADDR		(AHB1_BASEADDR + 0x3800)

#define GPIOA_BASEADDR		(AHB1_BASEADDR + 0x0000)
#define GPIOB_BASEADDR		(AHB1_BASEADDR + 0x0400)
#define GPIOC_BASEADDR		(AHB1_BASEADDR + 0x0800)
#define GPIOD_BASEADDR		(AHB1_BASEADDR + 0x0C00)
#define GPIOE_BASEADDR		(AHB1_BASEADDR + 0x1000)
#define GPIOF_BASEADDR		(AHB1_BASEADDR + 0x1400)
#define GPIOG_BASEADDR		(AHB1_BASEADDR + 0x1800)
#define GPIOH_BASEADDR		(AHB1_BASEADDR + 0x1C00)

#define USART2_BASEADDR		(APB1_BASEADDR + 0x4400)
#define USART3_BASEADDR		(APB1_BASEADDR + 0x4800)
#define UART4_BASEADDR		(APB1_BASEADDR + 0x4C00)
#define UART5_BASEADDR		(APB1_BASEADDR + 0x5000)
#define USART1_BASEADDR		(APB2_BASEADDR + 0x1000)
#define USART6_BASEADDR		(APB2_BASEADDR + 0x1400)

/**
 * Peripheral register structs
 */

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
	volatile uint32_t SR;
	volatile uint32_t DR;
	volatile uint32_t BRR;
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t CR3;
	volatile uint32_t GTPR;
} USART_RegDef_t;

/**
 * Peripheral macros
 */

#define RCC			((RCC_RegDef_t *)RCC_BASEADDR)

#define GPIOA		((GPIO_RegDef_t *)GPIOA_BASEADDR)
#define GPIOB		((GPIO_RegDef_t *)GPIOB_BASEADDR)

#define USART2		((USART_RegDef_t *)USART2_BASEADDR)
#define USART3		((USART_RegDef_t *)USART3_BASEADDR)
#define UART4		((USART_RegDef_t *)UART4_BASEADDR)
#define UART5		((USART_RegDef_t *)UART5_BASEADDR)
#define USART1		((USART_RegDef_t *)USART1_BASEADDR)
#define USART6		((USART_RegDef_t *)USART6_BASEADDR)

void uart_write_byte(uint8_t byte)
{
	while (!(USART2->SR & (1 << 7)));
	// Set data register to byte to send
	USART2->DR = byte;
	// Wait for byte to be sent (check for TC to SET)
	while (!(USART2->SR & (1 << 6)));
}

void uart_send_string(char *string)
{
	while (*string) uart_write_byte(*string++);
}

uint8_t uart_read_byte(void)
{
	uint8_t temp;
	while (!(USART2->SR & (1 << 5)));
	temp = USART2->DR;
	return temp;
}

void delay(uint32_t time)
{
	while (time--);
}

int main(void)
{
	// Enable GPIOA peripheral clock
	RCC->AHB1ENR |= (1 << 0);

	// Enable USART2 peripheral clock
	RCC->APB1ENR |= (1 << 17);
	// Set alternate function for PA2 (TX) and PA3 (RX)
	GPIOA->MODER |= (2 << 4);
	GPIOA->MODER |= (2 << 6);
	GPIOA->AFR[0] |= (7 << 8);
	GPIOA->AFR[0] |= (7 << 12);

	// Clear all bits
	USART2->CR1 = 0x00;
	// Enable USART
	USART2->CR1 |= (1 << 13);
	// Enable transmitter
	USART2->CR1 |= (1 << 3);
	// Enable receiver
	USART2->CR1 |= (1 << 2);
	// Set baud rate to 115200 @ 16MHz
	USART2->BRR = (11 << 0) | (8 << 4);

	// Write hello world to serial monitor
	uart_send_string("hello world\n");
	delay(5000000);

	// Set PA5 mode
	GPIOA->MODER |= (1 << 10);

	// Blink onboard LED forever
	for (;;) {
		GPIOA->BSRR = (1 << 5);
		delay(1000000);
		GPIOA->BSRR = (1 << (5 + 16));
		delay(1000000);
	}

	return 0;
}

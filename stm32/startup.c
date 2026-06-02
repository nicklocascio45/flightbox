#include <stdint.h>

#define SRAM_START      0x20000000U
#define SRAM_SIZE       (128U * 1024U) // 128KB
#define SRAM_END        ((SRAM_START) + (SRAM_SIZE))
#define STACK_START     SRAM_END

extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _la_data;
extern uint32_t _sbss;
extern uint32_t _ebss;

void Reset_Handler(void);

uint32_t vectors[] __attribute__((section(".vectors"))) = {
	STACK_START,
	(uint32_t)Reset_Handler,
};

void Reset_Handler(void)
{
	// Determine size of data section
	uint32_t data_len = (uint32_t)&_edata - (uint32_t)&_sdata;
	// Pointer to data start address (in sram) for copy destination
	uint8_t *dst = (uint8_t *)&_sdata;
	// Pointer to data load address (in flash) for copy source
	uint8_t *src = (uint8_t *)&_la_data;
	// Copy loop to move item at src addr to dst addr
	for (uint32_t i = 0; i < data_len; i++) {
		*dst++ = *src++;
	}

	// Determine size of bss (uninitialized data) section
	uint32_t bss_len = (uint32_t)&_ebss - (uint32_t)&_sbss;
	// Only need pointer to start of bss for zeroing
	uint8_t *bss_addr = (uint8_t *)&_sbss;
	// Zero-out bss loop
	for (uint32_t i = 0; i < bss_len; i++) {
		*bss_addr++ = 0;
	}
}

void prvGpioISR(void)
{
	static const uint32_t base_addr = XGPIO + GPIO_SIZE * PORT_A;  //GPIOA base address
	static uint8_t pingpong = 1;  // ping-pong flag
	mmio_write_32(base_addr + GPIO_PORTA_EOI, 0xFFFFFFFF);	// Clear interrupt flag
	writePin(PORT_A, 22, pingpong);  // Toggle output pin every interrupt
	pingpong ^= 1;
}

void writePin( uint8_t port, uint8_t pin, uint8_t value )
{
	uint32_t base_addr = XGPIO + GPIO_SIZE * port; // GPIO base address
	uint32_t val = mmio_read_32( base_addr + GPIO_SWPORTA_DR); 
	val = (value == GPIO_HIGH ? ( val | BIT(pin) ) : ( val & (~BIT(pin))));
	mmio_write_32(base_addr + GPIO_SWPORTA_DR, val);
}
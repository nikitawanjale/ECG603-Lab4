/* 
Lab04_T02 Change the toggle of the GPIO at 50Hz and at 50% duty cycle.Include a GPIO Interrupt to Task 02 from switch SW2 to turn ON and the LED for 0.5
sec. The toggle of the GPIO is suspended when executing the interrupt. Normal operation of Task
02 should begin after the switch event.
*/

#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

// Added The following four header files to use sw2
//#include "enable_sw2.h"
#include "inc/hw_gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"

void PortFunctionInit(void);
void IntSwitch2Handler();

int main()
{
	uint32_t ui32Period;

	// Set up the clock
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

	// GPIO configuration
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // Takes 5 clock cycles for port to be available
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); // LEDs

    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x1;

	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0); // SW2 for input

	// Timer configuration
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); // enable clock to TIMER0
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); // Configure TIMER0 as 32 bit timer

	// Calculate and set delay
	ui32Period = (SysCtlClockGet() / 10) / 2; // set the period
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period - 1);

	// Enable interrupt
	IntEnable(INT_TIMER0A); // enable vector associated with TIMER0A
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT); // Enable event to generate interrupt
	IntMasterEnable(); // Master int enable for all interrupts

	// Enable the timer
	TimerEnable(TIMER0_BASE, TIMER_A);

	//GPIOIntRegister(GPIO_PORTF_BASE, IntSwitch2Handler);
	IntEnable(INT_GPIOF);
	GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE);
	GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0); // enable int on F0 activity

	IntMasterEnable();


	while(1);
}

void IntSwitch2Handler()
// Interrupt handler for user switch2 to keep LED on for 0.5 sec
{
	GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);
	//Read the current state of the GPIO pin and write back the opposite state

	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
	SysCtlDelay(7000000);
}

void Timer0IntHandler(void)
// Interrupt handler for user switch2 to keep LED on for 0.5 sec
{
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	//Read the current state of the GPIOpin and write back the opposite state
	if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2))
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);
	}
	else
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
	}
}

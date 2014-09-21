/*
 * main.c
 *
 *  Created on: Jul 3, 2014
 *      Author: James Dean
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdint.h>

#define BATT_LED		0x10
#define START_BUTTON	0x01
#define START_LED		0x80
#define DEBUG_PIN		0x10
#define MOTOR_LED		0x20
#define MOTOR_PIN		0x02

#define START_DELAY		2000
#define RUN_TIME		120000	// Run for 2 minutes
#define BATT_INT		1000	// Check battery every second
#define BATT_LED_BLINK	500		// blink every second
#define BATT_V_LOW		215
#define BATT_V_CUTOFF	205
#define ADC_START		0x40
#define ADC_ON			0x80


enum MOTOR_STATES {
	MOTOR_IDLE = 0,
	MOTOR_TIMING
};

enum BATT_STATE {
	BATT_OK,
	BATT_LOW,
	BATT_CUTOFF
};

enum BUTTON_STATE {
	BUTTON_PRESSED = 0,
	BUTTON_RELEASED
};



 uint8_t 	checkBattery(uint8_t);
 void		shutdown();

//Globals
static uint32_t		ticks;
static uint32_t		motorStartTime;
static uint8_t		motorState;
static uint8_t		btnState;
static uint8_t		battState;
static uint32_t		battLedTime;
static uint32_t		lastBatteryTime;
static uint8_t		battCutoffCount;

int main(void){

	// Setup GPIO
	DDRD = DEBUG_PIN | START_LED | BATT_LED;	// Sets DEBUG_PIN, START_LED and BATT_LED to output
	PORTD =  BATT_LED;				// BAT_LED off, START_LED off
	DDRB = MOTOR_PIN | MOTOR_PIN;				// Sets MOTOR_LED pin as output, every thing else input
	PORTB =  START_BUTTON;	// Turn off LED and set pullup for button

	//Setup Program Timer
	TCCR0A = 0x02;
	OCR0A = 250;
	TIMSK0 = 0x02;

	//Configure ADC
	ADMUX = 0x60;		// Vref = AVCC, left adjust
	ADCSRA = (0x07 | ADC_ON);		// Enable ADC, prescaler = 128

	// Initalize Global Variables
	ticks = 0;
	motorState = MOTOR_IDLE;
	btnState = BUTTON_RELEASED;
	lastBatteryTime = 0;
	battState = BATT_OK;
	battLedTime = 0;
	battCutoffCount = 0;

	// Start timers and begin program
	TCCR0B = 0x03;	// Select 64x prescaler

	sei();

	for(;;){

	}
	return 0;
}

/*
 * This is the main program
 */
ISR(TIMER0_COMPA_vect){
	uint8_t btnNow;
	//PORTD = PIND ^ DEBUG_PIN;
	ticks++;
	btnNow = PINB & START_BUTTON;

	if(btnNow == BUTTON_PRESSED && btnState == BUTTON_RELEASED){
		// Switch state on falling edge
		switch(motorState){
		case MOTOR_IDLE:
			motorStartTime = ticks;
			PORTD = (PORTD | START_LED);	// Turn START_LED off
			PORTB = (PORTB | MOTOR_LED);			// Turn motor LED on
			PORTB = (PORTB | MOTOR_PIN);
			motorState = MOTOR_TIMING;
			break;
		case MOTOR_TIMING:
			PORTD = (PORTD & ~START_LED); // Turn START_LED on
			PORTB = (PORTB & ~MOTOR_LED);		// Turn motor LED off
			PORTB = (PORTB & ~MOTOR_PIN);	// Turn motor off
			motorState = MOTOR_IDLE;
			break;
		}
	}
	btnState = btnNow;

	// If the motor is running, check for time out
	if(motorState == MOTOR_TIMING){
		if((ticks - motorStartTime) > RUN_TIME){
			motorState = MOTOR_IDLE;
			PORTD = (PORTD & ~START_LED); 	// Turn START_LED on
			PORTB = (PORTB & ~MOTOR_LED);	// Turn motor LED off
			PORTB = (PORTB & ~MOTOR_PIN);	// Turn motor off
		}
	}

	// If battery is low, blick LED
	if(battState == BATT_LOW){
		if((ticks - battLedTime) > BATT_LED_BLINK){
			battLedTime = ticks;
			PORTD = PIND ^ BATT_LED;
		}
	}

	//Check Battery level
	if((ticks - lastBatteryTime) > BATT_INT){
		uint8_t battLvl;
		lastBatteryTime = ticks;
		//PORTD = PIND ^ BATT_LED;
		battLvl = checkBattery(0);
		if(battLvl < BATT_V_LOW)
			battState = BATT_LOW;
		if(battLvl < BATT_V_CUTOFF){
			battCutoffCount++;
			if(battCutoffCount > 10){
				battState = BATT_CUTOFF;
				shutdown();
			}
		}
	}
}

uint8_t checkBattery(uint8_t chan){
	// PORTD = PIND ^ BATT_LED;
	// PORTD |= DEBUG_PIN;	// Set debug pin to measure function run time
	ADCSRA |= ADC_START;
	while(ADCSRA & ADC_START){
	}

	// PORTD &= ~DEBUG_PIN;
	return ADCH;
}

void shutdown(){
	cli();	//Turn off interrupts
	PORTD = (PORTD & ~BATT_LED)|START_LED;	// Turn off start led, turn on battery led
	PORTB = (PORTB & ~MOTOR_LED);			// Turn motor off
	PORTB = (PORTB & ~MOTOR_PIN);
	TCCR0B = 0x00;							// Turn off program timer

	while(1){
		// Wait to be shutoff
	}
}

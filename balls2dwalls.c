#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "C:\Users\Raymond Farias\Documents\Atmel Studio\7.0\lab_chip\includes\io.h"
#include "C:\Users\Raymond Farias\Documents\Atmel Studio\7.0\lab_chip\includes\io.c"

unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}

unsigned short SetBit2(unsigned short x, unsigned char k, unsigned char b) {
	return (b ? x | (0x0001 << k) : x & ~(0x0001 << k));
}
unsigned short GetBit2(unsigned short x, unsigned char k) {
	return ((x & (0x0001 << k)) != 0);
}

void shiftLeftLC(unsigned short* x, unsigned char* pos) {
	*x = SetBit2(*x, 15 - *pos, 1);
	if(*pos == 0) {
		*pos = 8;
	}
	*pos = *pos - 1;
	*x = SetBit2(*x, 15 - *pos, 0);
}

void shiftRightLC(unsigned short* x, unsigned char* pos) {
	*x = SetBit2(*x, 15 - *pos, 1);
	*pos = *pos + 1;
	if(*pos == 8) {
		*pos = 0;
	}
	*x = SetBit2(*x, 15 - *pos, 0);
}

void shiftLeftRC(unsigned short* x, unsigned char* pos) {
	*x = SetBit2(*x, *pos, 0);
	*pos = *pos + 1;
	if(*pos == 8) {
		*pos = 0;
	}
	*x = SetBit2(*x, *pos, 1);
}

void shiftRightRC(unsigned short* x, unsigned char* pos) {
	*x = SetBit2(*x, *pos, 0);
	if(*pos == 0) {
		*pos = 8;
	}
	*pos = *pos - 1;
	*x = SetBit2(*x, *pos, 1);
}

void setCol(unsigned short* x, unsigned char* col) {
	*x = SetBit2(*x, 15 - *col, 0);
}

void setRow(unsigned short* x, unsigned char* row) {
	*x = SetBit2(*x, *row, 1);
}
//RNG
unsigned char randomNum = 0;
//LEDMATRIX
unsigned short flipBit = 0xDF02;
unsigned short flipBit2 = 0x0000;
unsigned short flipBit3 = 0;
unsigned short flipBit4 = 0;
unsigned short flipBit5 = 0;
unsigned short data = 0xFFFF;
//WALLS
unsigned char wallPerm = 0;
unsigned char wallType = 0;
unsigned char WALL_SPEED = 5;
unsigned char UPDATE_GAME_BREAKPOINT = 23;
//COLLISION ARRAY
unsigned char collision[8][8];
unsigned char Pcollision[8][8];
//death boolean
unsigned char lives = 0;
//POSITION OF PLAYER
unsigned char LEDHpos = 2;
unsigned char LEDVpos = 2;
//INVULN
unsigned short invulnTimer = 0;
const unsigned short INVULN_ON_SPAWN = 150;
const unsigned short INVULN_ON_HIT = 100;
const unsigned short INVULN_ON_LIFE_PICK_UP = 150;
const unsigned short INVULN_ON_SHIELD_PICK_UP = 2200;
unsigned char IW = 0;
//POWERUP
unsigned char isPower = 0;
unsigned char powerPerm = 0;
unsigned char powerType = 0;
//LCD SM VARS
char displayLCD[30];
char scoreString[6];
char* livesString = "Lives: ";
//char* displayMenu = "BALL FONDLERS  ";
char* displayMenu = "BALLS 2D WALLS ";
//char* displayMenu = "3 WALLS 1 BALL ";
char* scoreDisplay = " Last Score: ";
unsigned char menuShown = 0;
unsigned char hasDied = 0;
unsigned short score = 0;
unsigned char scoreTimer = 0;
unsigned char deathWait = 0;
//LED SM VARS
unsigned char shownInvuln = 0;

void clearCollision() {
	for(unsigned char q = 0; q < 8; q++) {
		for(unsigned char r = 0; r < 8; r++) {
			collision[q][r] = 0;
		}
	}
}

void clearPCollision() {
	for(unsigned char q = 0; q < 8; q++) {
		for(unsigned char r = 0; r < 8; r++) {
			Pcollision[q][r] = 0;
		}
	}
}

void updateCollision3(unsigned char t) {
	clearPCollision();
	switch(t) {
		case 0:
		Pcollision[0][6] = 1;
		break;
		
		case 1:
		Pcollision[0][1] = 1;
		break;
		
		case 2:
		Pcollision[7][6] = 1;
		break;
		
		case 3:
		Pcollision[7][1] = 1;
		break;
		
	}
}

void powerUpGen(short* x, unsigned char *t) {
	switch(*t) {
		case 0:
		*x = 0xFD01;
		break;
		
		case 1:
		*x = 0xBF01;
		*t = *t + 1;
		break;
		
		case 2:
		*x = 0xFD80;
		break;
		
		case 3:
		*x = 0xBF80;
		break;
		
	}
	
}

void wallGen(unsigned short* x, unsigned short* y, unsigned char* t, unsigned char* p) {
	switch(*t) {
		case 0:
		switch(*p) {
			//left side open
			case 0:
			*x = 0xFEFF;
			*y = 0x0181;
			break;
			
			case 1:
			*x = 0xFD7E;
			*y = 0x8342;
			break;
			
			case 2:
			*x = 0xFB3C;
			*y = 0xC724;
			break;
			
			case 3:
			*x = 0xE718;
			*y = 0xE718;
			break;
			
			default:
			*p = 0;
			break;
			
		}
		break;
		
		//top side open
		case 1:
		switch(*p) {
			case 0:
			*x = 0x7EFF;
			*y = 0x0080;
			break;
			
			case 1:
			*x = 0xBD7E;
			*y = 0x8140;
			break;
			
			case 2:
			*x = 0xDB3C;
			*y = 0xE720;
			
			break;
			
			case 3:
			*x = 0xE718;
			*y = 0xE718;
			
			break;
			
			default:
			*p = 0;
			break;
		}
		break;
		
		//right side bb
		case 2:
		switch(*p) {
			case 0:
			*x = 0x7FFF;
			*y = 0x0081;
			break;
			
			case 1:
			*x = 0xBF7E;
			*y = 0x8142;
			break;
			
			case 2:
			*x = 0xDF3C;
			*y = 0xC324;
			break;
			
			case 3:
			*x = 0xE718;
			*y = 0xE718;
			break;
			
			default:
			*p = 0;
			break;
		}
		break;
		
		//bottom now dowg
		case 3:
		switch(*p) {
			case 0:
			*x = 0x7EFF;
			*y = 0x0001;
			break;
			
			case 1:
			*x = 0xBD7E;
			*y = 0x8102;
			break;
			
			case 2:
			*x = 0xDB3C;
			*y = 0xE704;
			break;
			
			case 3:
			*x = 0xE718;
			*y = 0xE718;
			break;
			
			default:
			*p = 0;
			break;
		}
		break;
		
		default:
		*t = 0;
		break;
	}
}

void updateCollision2(unsigned char t, unsigned char p) {
	clearCollision();
	switch(t) {
		case 0:
		//left
		switch(p) {
			case 0:
			for(unsigned char q = 0; q < 8; q+= 7) {
				for(unsigned char r = 0; r < 8; r++) {
					collision[q][r] = 1;
				}
			}
			for(unsigned char r = 0; r < 8; r++) {
				collision[r][7] = 1;
			}
			break;
			
			case 1:
			for(unsigned char q = 1; q < 7; q+= 5) {
				for(unsigned char r = 1; r < 7; r++) {
					collision[q][r] = 1;
				}
			}
			for(unsigned char r = 1; r < 7; r++) {
				collision[r][6] = 1;
			}
			break;
			
			case 2:
			for(unsigned char q = 2; q < 6; q+= 3) {
				for(unsigned char r = 2; r < 6; r++) {
					collision[q][r] = 1;
				}
			}
			for(unsigned char r = 2; r < 6; r++) {
				collision[r][5] = 1;
			}
			break;
			
			case 3:
			for(unsigned char q = 3; q < 5; q++) {
				for(unsigned char r = 3; r < 5; r++) {
					collision[q][r] = 1;
				}
			}
			break;
		}
		break;
		
		case 1:
		//up
		switch(p) {
			case 0:
			for(unsigned char q = 0; q < 8; q++) {
				for(unsigned char r = 0; r < 8; r+=7) {
					collision[q][r] = 1;
				}
			}
			for(unsigned char r = 0; r < 8; r++) {
				collision[7][r] = 1;
			}
			break;
			
			case 1:
			for(unsigned char q = 1; q < 7; q++) {
				for(unsigned char r = 1; r < 7; r+=5) {
					collision[q][r] = 1;
				}
			}
			for(unsigned char r = 1; r < 7; r++) {
				collision[6][r] = 1;
			}
			break;
			
			case 2:
			for(unsigned char q = 2; q < 6; q++) {
				for(unsigned char r = 2; r < 6; r+=3) {
					collision[q][r] = 1;
				}
			}
			for(unsigned char r = 2; r < 6; r++) {
				collision[5][r] = 1;
			}
			break;
			
			case 3:
			for(unsigned char q = 3; q < 5; q++) {
				for(unsigned char r = 3; r < 5; r++) {
					collision[q][r] = 1;
				}
			}
			break;
		}
		break;
		
		case 2:
		//right
		switch(p) {
			case 0:
			for(unsigned char q = 0; q < 8; q+= 7) {
				for(unsigned char r = 0; r < 8; r++) {
					collision[q][r] = 1;
				}
			}
			for(unsigned char r = 0; r < 8; r++) {
				collision[r][0] = 1;
			}
			break;
			
			case 1:
			for(unsigned char q = 1; q < 7; q+= 5) {
				for(unsigned char r = 1; r < 7; r++) {
					collision[q][r] = 1;
				}
			}
			for(unsigned char r = 1; r < 7; r++) {
				collision[r][1] = 1;
			}
			break;
			
			case 2:
			for(unsigned char q = 2; q < 6; q+= 3) {
				for(unsigned char r = 2; r < 6; r++) {
					collision[q][r] = 1;
				}
			}
			for(unsigned char r = 2; r < 6; r++) {
				collision[r][2] = 1;
			}
			break;
			
			case 3:
			for(unsigned char q = 3; q < 5; q++) {
				for(unsigned char r = 3; r < 5; r++) {
					collision[q][r] = 1;
				}
			}
			break;
		}
		break;
		
		case 3:
		//down
		switch(p) {
			case 0:
			for(unsigned char q = 0; q < 8; q++) {
				for(unsigned char r = 0; r < 8; r+=7) {
					collision[q][r] = 1;
				}
			}
			for(unsigned char r = 0; r < 8; r++) {
				collision[0][r] = 1;
			}
			break;
			
			case 1:
			for(unsigned char q = 1; q < 7; q++) {
				for(unsigned char r = 1; r < 7; r+=5) {
					collision[q][r] = 1;
				}
			}
			for(unsigned char r = 1; r < 7; r++) {
				collision[1][r] = 1;
			}
			break;
			
			case 2:
			for(unsigned char q = 2; q < 6; q++) {
				for(unsigned char r = 2; r < 6; r+=3) {
					collision[q][r] = 1;
				}
			}
			for(unsigned char r = 2; r < 6; r++) {
				collision[2][r] = 1;
			}
			break;
			
			case 3:
			for(unsigned char q = 3; q < 5; q++) {
				for(unsigned char r = 3; r < 5; r++) {
					collision[q][r] = 1;
				}
			}
			break;
		}
		break;
	}
}

unsigned char checkCollision() {
	if(collision[LEDVpos][LEDHpos] == 1) {
		return 1;
	}
	else {
		return 0;
	}
}

unsigned char checkPCollision() {
	if(Pcollision[LEDVpos][LEDHpos] == 1) {
		return 1;
	}
	else {
		return 0;
	}
}

void writeLives() {
	LCD_DisplayString(1, "   ");
	LCD_Cursor(1);
	LCD_WriteData('L');
	LCD_Cursor(2);
	LCD_WriteData('I');
	LCD_Cursor(3);
	LCD_WriteData('V');
	LCD_Cursor(4);
	LCD_WriteData('E');
	LCD_Cursor(5);
	LCD_WriteData('S');
	LCD_Cursor(6);
	LCD_WriteData(':');
}

//Timer code
volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if( _avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

//End Timer Code

void InitADC(void)
{
	ADMUX|=(1<<REFS0);
	ADCSRA|=(1<<ADEN)|(1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2);
}
unsigned char ADCRead(unsigned char pinToRead)
{
	pinToRead = pinToRead & 0x07;
	ADMUX = (ADMUX & 0xf8) | pinToRead;
	ADCSRA = ADCSRA | (1<<ADSC);
	while( (1<<ADSC) & (ADCSRA));
	return(ADC);
}

typedef struct task {
	int state;                  // Task's current state
	unsigned long period;       // Task period
	unsigned long elapsedTime;  // Time elapsed since last task tick
	int (*TickFct)(int);        // Task tick function
} task;


unsigned char xInput, yInput;
enum SM1_States { JoystickInput };
int TickFct_JoystickInput(int state) {
	switch(state) {
		case JoystickInput:
		state = JoystickInput;
		break;
		default:
		state = JoystickInput;
		break;
	}
	switch(state) {
		case JoystickInput:
		xInput = ADCRead(0);
		yInput = ADCRead(1);
		break;
	}
	return state;
}

enum SM2_States { readInput };
// none = 0, left / down = 1, right / up = 2
unsigned char horizontal = 0;
unsigned char vertical = 0;

//Holds threshold values to read if left/none/right and up/none/down
const unsigned char RIGHT_HORIZ_VAL = 165;
const unsigned char UP_VERT_VAL = 165;
const unsigned char LEFT_HORIZ_VAL = 115;
const unsigned char DOWN_VERT_VAL = 115;
int TickFct_ReadInput(int state) {
	switch(state) {
		case readInput:
		state = readInput;
		break;
		default:
		state = readInput;
		break;
	}
	switch(state) {
		case readInput:
		if(xInput > RIGHT_HORIZ_VAL) {
			horizontal = 2;
		}
		else if(xInput < LEFT_HORIZ_VAL) {
			horizontal = 1;
		}
		else {
			horizontal = 0;
		}
		
		if(yInput > UP_VERT_VAL) {
			vertical = 2;
		}
		else if(yInput < DOWN_VERT_VAL) {
			vertical = 1;
		}
		else {
			vertical = 0;
		}
		break;
	}
	return state;
}



void wallUpdate() {
	//PERM UPDATE
	wallPerm++;
	if(wallPerm == 4) {
		randomNum = rand() % 4;
		wallPerm = 0;
		wallType = randomNum;
	}
	updateCollision2(wallType, wallPerm);
}

void spawnPowerUp() {
	randomNum = rand() % 4;
	powerType = randomNum;
	randomNum = rand() % 2;
	powerPerm = randomNum;
	isPower = 1;
	updateCollision3(powerType);
	powerUpGen(&flipBit5, &powerType);
}
void clearPowerUp() {
	isPower = 0;
	clearPCollision();
	flipBit5 = 0xFF00;
}

void initializeGame() {
	WALL_SPEED = 5;
	wallType = 1;
	wallPerm = 0;
	updateCollision2(wallType, wallPerm);
	lives = 2;
	score = 0;
	invulnTimer = INVULN_ON_SPAWN;
	writeLives();
}

void updateGame() {
	menuShown = 0;
	scoreTimer++;
	if(scoreTimer % WALL_SPEED == 0) {
		//UPDATE WALL
		wallUpdate();
	}
	if(scoreTimer == UPDATE_GAME_BREAKPOINT) {
		score++;
		itoa(score, scoreString, 10);
		scoreTimer = 0;
		if(score % 5 == 0 && isPower == 0 && score != 0) {
			spawnPowerUp();
		}
		if(score % 10 == 0 && WALL_SPEED > 2) {
			WALL_SPEED--;
		}
	}
	LCD_Cursor(8);
	LCD_WriteData(lives + '0');
}

void showMenu() {
	if(deathWait > 0) {
		deathWait--;
	}
	if(!menuShown) {
		wallType = 1;
		wallPerm = 0;
		updateCollision2(wallType, wallPerm);
		itoa(score, scoreString, 10);
		itoa(lives, livesString, 10);
		LCD_DisplayString(1, displayMenu);
		menuShown = 1;
		flipBit == 0x0000;
		if(hasDied) {
			strcpy(displayLCD, displayMenu);
			strcat(displayLCD, scoreDisplay);
			strcat(displayLCD, scoreString);
			LCD_DisplayString(1, displayLCD);
		}
	}
}
enum SM3_States { out, menu };
int TickFct_OutputLCD(int state) {
	switch(state) {
		case out:
		if(lives == 0) {
			state = menu;
			hasDied = 1;
			clearPowerUp();
			deathWait = 10;
		}
		else {
			state = out;
		}
		break;
		
		case menu:
		switch(horizontal) {
			case 0: // N
			break;
			case 1: //L
			break;
			case 2:
			break;
			default: // R
			break;
		}
		switch(vertical) {
			case 0: //N
			break;
			case 1: // D
			break;
			case 2: // U
			if(deathWait == 0) {
				initializeGame();
				state = out;
			}
			break;
			default:
			break;
		}
		break;
		
		default:
		state = menu;
		break;
	}
	switch(state) {
		case out:
		switch(horizontal) {
			case 0: // N
			break;
			case 1: //L
			shiftLeftLC(&flipBit, &LEDHpos);
			break;
			case 2: //R
			shiftRightLC(&flipBit, &LEDHpos);
			break;
			default:
			break;
		}
		switch(vertical) {
			case 0: //N
			break;
			case 1: //D
			shiftLeftRC(&flipBit, &LEDVpos);
			break;
			case 2: // U
			shiftRightRC(&flipBit, &LEDVpos);
			break;
			default:
			break;
		}
		
		updateGame();
		break;
		
		case menu:
		showMenu();
		break;
	}
	return state;
}


void showShield() {
	IW = 17;
	LCD_Cursor(IW);
	LCD_WriteData('S');
	IW++;
	LCD_Cursor(IW);
	LCD_WriteData('H');
	IW++;
	LCD_Cursor(IW);
	LCD_WriteData('I');
	IW++;
	LCD_Cursor(IW);
	LCD_WriteData('E');
	IW++;
	LCD_Cursor(IW);
	LCD_WriteData('L');
	IW++;
	LCD_Cursor(IW);
	LCD_WriteData('D');
	IW++;
	LCD_Cursor(IW);
	LCD_WriteData('E');
	IW++;
	LCD_Cursor(IW);
	LCD_WriteData('D');
	IW++;
}

void clearShield() {
	IW = 17;
	LCD_Cursor(IW);
	LCD_WriteData(' ');
	IW++;
	LCD_Cursor(IW);
	LCD_WriteData(' ');
	IW++;
	LCD_Cursor(IW);
	LCD_WriteData(' ');
	IW++;
	LCD_Cursor(IW);
	LCD_WriteData(' ');
	IW++;
	LCD_Cursor(IW);
	LCD_WriteData(' ');
	IW++;
	LCD_Cursor(IW);
	LCD_WriteData(' ');
	IW++;
	LCD_Cursor(IW);
	LCD_WriteData(' ');
	IW++;
	LCD_Cursor(IW);
	LCD_WriteData(' ');
}

void checkAllCollisions() {
	if(checkCollision() == 1 && lives > 0 && invulnTimer == 0) {
		lives--;
		invulnTimer = INVULN_ON_HIT;
	}
	if(checkPCollision() == 1 && lives > 0 && isPower == 1) {
		if(powerPerm == 0) {
			lives++;
			invulnTimer = INVULN_ON_LIFE_PICK_UP;
		}
		else if(powerPerm == 1) {
			invulnTimer = INVULN_ON_SHIELD_PICK_UP;
		}
		clearPowerUp();
	}
	if(invulnTimer > 0) {
		if(shownInvuln == 0 && !menuShown) {
			showShield();
			shownInvuln = 1;
		}
		if(invulnTimer == 1 && !menuShown) {
			clearShield();
			shownInvuln = 0;
		}
		invulnTimer--;
	}
}

void transmitDude() {
	flipBit2 = ~flipBit;
	data = 0xFFFF;
	data = data & flipBit2;
	transmit_data(data);
}

void transmitWall1() {
	wallGen(&flipBit3, &flipBit4, &wallType, &wallPerm);
	flipBit2 = ~flipBit3;
	data = 0xFFFF;
	data = data & flipBit2;
	transmit_data(data);
}

void transmitWall2() {
	flipBit2 = ~flipBit4;
	data = 0xFFFF;
	data = data & flipBit2;
	transmit_data(data);
}

void transmitPower() {
	flipBit2 = ~flipBit5;
	data = 0xFFFF;
	data = data & flipBit2;
	transmit_data(data);
}
enum SM4States {dudeM, wallM, wallM2, powers};
int TickFct_MatrixOut(int state) {
	switch(state) {
		case dudeM:
		state = wallM;
		break;
		
		case wallM:
		state = wallM2;
		break;
		
		case wallM2:
		state = powers;
		break;
		
		case powers:
		state = dudeM;
		break;
		
		default:
		state = dudeM;
		break;
	}
	switch(state) {
		case dudeM:
		transmitDude();
		break;
		
		case wallM:
		transmitWall1();
		break;
		
		case wallM2:
		transmitWall2();
		break;
		
		case powers:
		transmitPower();
		break;
		
		default:
		break;
		
	}
	checkAllCollisions();
	return state;
}

void transmit_data(unsigned short data) {
	for (int i = 15; i >= 0; --i) {
		PORTB = 0x08;
		PORTB |= ((data >> i) & 0x01);
		PORTB |= 0x04;
	}
	PORTB |= 0x02;
	PORTB = 0x00;
}

void set_PWM(double frequency) {
	// Keeps track of the currently set frequency
	// Will only update the registers when the frequency
	// changes, plays music uninterrupted.
	static double current_frequency;
	if (frequency != current_frequency) {

		if (!frequency) TCCR3B &= 0x08; //stops timer/counter
		else TCCR3B |= 0x03; // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) OCR3A = 0xFFFF;
		
		// prevents OCR3A from underflowing, using prescaler 64					// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) OCR3A = 0x0000;
		
		// set OCR3A based on desired frequency
		else OCR3A = (short)(8000000 / (128 * frequency)) - 1;

		TCNT3 = 0; // resets counter
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	// COM3A0: Toggle PB6 on compare match between counter and OCR3A
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	// WGM32: When counter (TCNT3) matches OCR3A, reset counter
	// CS31 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

double arr[] = {329.63, 293.66, 261.63, 349.23, 329.63, 293.66, 392.00, 349.23, 329.63, 440.00, 392.00, 349.23, 493.88, 440.00, 392.00, 523.25, 493.88, 440.00, 440.00, 392.00, 349.23, 392.00, 349.23, 329.63, 293.66, 261.63, 349.23, 329.63, 293.66, 392.00, 349.23, 329.63, 440.00, 392.00, 349.23, 493.88, 440.00, 392.00, 523.25, 493.88, 440.00, 440.00, 392.00, 349.23, 392.00, 349.23, 349.23, 329.63, 440.00, 392.00, 349.23, 329.63, 440.00, 392.00, 349.23, 329.63, 440.00, 392.00, 349.23, 329.63 };
double soundtrack[] = {440.00, 440.00, 440.00, 440.00, 440.00, 440.00, 440.00, 440.00, 440.00, 440.00, 392.00, 392.00, 329.63, 329.63, 261.63, 261.63, 261.63, 261.63, 440.00, 440.00, 440.00, 440.00, 392.00, 392.00, 349.23, 349.23, 392.00, 392.00, 444.00, 444.00 };

unsigned char noteToPlay = 0;
enum SM5State { play };
int TickFct_Music(int state) {
	switch(state) {
		case play:
		break;
		
		default:
		state = play;
		break;
	}
	switch(state) {
		case play:
		if(menuShown == 1) {
			set_PWM(soundtrack[noteToPlay]);
		}
		else {
			set_PWM(arr[noteToPlay]);
		}
		if( noteToPlay == 27) { noteToPlay = 0 ;}
		noteToPlay++;
		break;
	}
	return state;
}

int main(void)
{
	//RAND
	srand(333);
	randomNum = rand();
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0xF0; PORTA = 0x0F;
	DDRC = 0xFF; PORTC = 0x00; // LCD data lines
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
	// Initializes the LCD display
	LCD_init();
	
	// Initialize the ADC
	InitADC();
	
	//INIT PWN
	PWM_on();
	
	//init coll arr
	for(unsigned char q = 0; q < 8; q++) {
		for(unsigned char r = 0; r < 8; r++) {
			collision[q][r] = 0;
		}
	}
	
	static task task1;
	static task task2;
	static task task3;
	static task task4;
	static task task5;
	
	const unsigned char tasksNum = 5;
	const unsigned long tasksPeriodGCD = 5;
	const unsigned long SM1Period = 30;
	const unsigned long SM2Period = 50;
	const unsigned long SM3Period = 100;
	const unsigned long SM4Period = 5;
	const unsigned long SM5Period = 80;
	
	task* tasks[5] = {&task1, &task2, &task3, &task4, &task5};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	task1.state = -1;
	task1.period = SM1Period;
	task1.elapsedTime = SM1Period;
	task1.TickFct = &TickFct_JoystickInput;
	
	task2.state = -1;
	task2.period = SM2Period;
	task2.elapsedTime = SM2Period;
	task2.TickFct = &TickFct_ReadInput;
	
	task3.state = -1;
	task3.period = SM3Period;
	task3.elapsedTime = SM3Period;
	task3.TickFct = &TickFct_OutputLCD;
	
	task4.state = -1;
	task4.period = SM4Period;
	task4.elapsedTime = SM4Period;
	task4.TickFct = &TickFct_MatrixOut;
	
	task5.state = -1;
	task5.period = SM5Period;
	task5.elapsedTime = SM5Period;
	task5.TickFct = &TickFct_Music;
	
	TimerSet(tasksPeriodGCD);
	TimerOn();
	
	unsigned short i;
	
	data = 0xFFFF;
	flipBit = 0xDF04;
	flipBit2 = ~flipBit;
	while(1) {
		for ( i = 0; i < numTasks; i++ ) {
			if ( tasks[i]->elapsedTime == tasks[i]->period ) {
				tasks[i]->state =
				tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += tasksPeriodGCD;
		}
		while (!TimerFlag);
		TimerFlag = 0;
	}
}
# 120BFinal
This custom lab project was the most fun I have ever had coding. My custom lab
project was called BALLS 2D WALLS; It is a game where you are a dot and you dodge
walls. There are three build upons: the joystick, the LED matrix, and the shift registers.
The joystick is an analog input and is implemented using the ADC on the ATMega chip.
The shift registers are used to communicate with the LED matrix using the example
code style. The LED matrix is used as an output to visualize to the user.

I had five state machines for my custom lab project. The first state machine
simply reads the ADC value from the joystick. The second state machine sets two
global variables : horizontal and vertical. These two are set based on the values from
the first state machine, telling us what direction is currently being input by the user. The
third state machine is used to output the lives to the LCD and the menu to the LCD;
additionally, the game’s walls, powerups, player position, and collision is updated during
this state. The fourth state machine simply outputs and time multiplexes the walls and
player position to the user; also, the shield and powerups invulnerability is calculated
during this state. Finally, the last state machine plays two soundtracks: one for the
menu and another for the gameplay.

User Guide:
To play, simply press up on the Joystick to begin. You are a ball that is dodging walls.
Powerups will spawn every so often, which will grant you lives or a shield.

Link to Video:
https://www.youtube.com/watch?v=f7KFHZzVbLo

Source Files:
rfari002_final.c : Has everything (All 5 SMs). Used ATmega, AVR Studio 7, and UCR
Lab example code
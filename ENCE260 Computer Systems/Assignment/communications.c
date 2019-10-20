/*
File:   communications.c
Author: Josie Smith (jcs162) and Yu Wen (ywe39)
Date:   11/10/2019
Descr:  Defines IR communication functions
*/
#include "system.h"
#include "pio.h"
#include "ir_uart.h"
#include "ball.h"
#include "communications.h"


/* Changes the current state and sends the new state */
void change_state(int changeto, states_t *state)
{
    char code = 0;
    if (changeto == READY) code = READY_CODE;
    else if (changeto == PLAYING) code = PLAYING_CODE;
    else if (changeto == START) code = START_CODE;
    else if (changeto == SCOREBOARD) code = SCOREBOARD_CODE;
    else if (changeto == SCORED) code = SCORED_CODE;
    else if (changeto == PAUSE) code = PAUSE_CODE;

    *state = changeto;
    ir_uart_putc (code);
}


/* Sends encoded values of the ball position and direction */
Ball send_ball(Ball ball)
{
    ball.visible = 0;
    char data = 33 + 3*ball.position[0] + ball.direction[0];
    ir_uart_putc (data);
    return ball;
}


/* Receives data and decodes the ball position and direction */
Ball get_ball(int data, Ball ball)
{
    data = data - START_NUMBERS;
    int direction = data % 3;
    if (direction > 1) direction = -1;
    data = data - direction;

    ball.position[0] = 6 - data / 3;
    ball.position[1] = 0;
    ball.direction[0] = (-1) * (direction);
    ball.direction[1] = 1;
    ball.visible = 1;
    return ball;
}

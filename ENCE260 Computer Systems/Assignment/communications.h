/*
File:   communications.h
Author: Josie Smith (jcs162) and Yu Wen (ywe39)
Date:   11/10/2019
Descr:  Defines IR communication functions
*/
#ifndef COMMUNICATIONS_H
#define COMMUNICATIONS_H

#include "system.h"
#include "pio.h"
#include "ir_uart.h"
#include "ball.h"

#define START_CODE 'X'
#define READY_CODE 'R'
#define PLAYING_CODE 'Y'
#define SCORED_CODE 'D'
#define SCOREBOARD_CODE 'S'
#define PAUSE_CODE 'P'

/* range is 32 - 53 which doesnn't overlap with the letters. This range is used
when encoding the ball position and direction. */
#define START_NUMBERS 33

typedef enum {START, READY, PLAYING, SCORED, SCOREBOARD, PAUSE} states_t;


/* Changes the current state and sends the new state */
void change_state(int changeto, states_t *state);

/* Sends encoded values of the ball position and direction */
Ball send_ball(Ball ball);

/* Receives data and decodes the ball position and direction */
Ball get_ball(int data, Ball ball);


#endif //COMMUNICATIONS_H

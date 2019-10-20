/*
File:   ball.h
Author: Josie Smith (jcs162) and Yu Wen (ywe39)
Date:   11/10/2019
Descr:  Controlls the position of the ball.
*/
#ifndef BALL_H
#define BALL_H

#include <time.h>
#include "system.h"
#include "pio.h"
#include "led_display.h"


typedef struct ball_s {
    int visible;
    int position[2];
    int direction[2];
} Ball;


/* Creates a ball at the position specified */
void display_ball(Ball ball);

/* Pushes the ball in a random direction, to start the game. */
Ball push_ball(Ball ball);

/* Updates the ball to its next position. */
Ball update_ball(Ball ball, int paddle_position);

/* Changes direction on the ball if it was hit by the paddle */
Ball check_if_hit(Ball ball, int paddle_position);

/* Initialises the ball on the paddle */
Ball ready_ball(Ball ball, int paddle_position);


#endif //BALL_H

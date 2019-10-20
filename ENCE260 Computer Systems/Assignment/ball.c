/*
File:   ball.c
Author: Josie Smith (jcs162) and Yu Wen (ywe39)
Date:   11/10/2019
Descr:  Controlls the position of the ball.
*/
#include "system.h"
#include "pio.h"
#include "led_display.h"
#include "ball.h"


/** Define PIO pins driving LED matrix rows.  */
static const pio_t rows[] =
{
    LEDMAT_ROW1_PIO, LEDMAT_ROW2_PIO, LEDMAT_ROW3_PIO,
    LEDMAT_ROW4_PIO, LEDMAT_ROW5_PIO, LEDMAT_ROW6_PIO,
    LEDMAT_ROW7_PIO
};

/** Define PIO pins driving LED matrix columns.  */
static const pio_t cols[] =
{
    LEDMAT_COL1_PIO, LEDMAT_COL2_PIO, LEDMAT_COL3_PIO,
    LEDMAT_COL4_PIO, LEDMAT_COL5_PIO
};


/* Creates a ball at the position specified */
void display_ball(Ball ball)
{
    for (int i = 0; i < 5; i++) {
        pio_output_high(cols[i]);
    }
    for (int i = 0; i < 7; i++) {
        pio_output_high(rows[i]);
    }

    pio_output_low(rows[ball.position[0]]);
    pio_output_low(cols[ball.position[1]]);
}


/* Pushes the ball in a random direction, to start the game. */
Ball push_ball(Ball ball)
{
    ball.direction[0] = 0;
    ball.direction[1] = -1;
    return ball;
}


/* Updates the ball to its next position. */
Ball update_ball(Ball ball, int paddle_position)
{
    int current_row = ball.position[0];
    int next_col = ball.position[1] + ball.direction[1];
    int next_row = ball.position[0] + ball.direction[0];
    // ball bounces off the paddle
    if (next_col == 4) {
        if (current_row - paddle_position == 0) {
            // bounce in the middle of the paddle, direction stays the same
            ball.direction[1] = (-1) * ball.direction[1];
        } else if (current_row - paddle_position == 1) {
            // bounce on the left of the paddle, direction goes left
            if (ball.direction[0] < 1) {
                ball.direction[0] = ball.direction[0] + 1;
            }
            ball.direction[1] = (-1) * ball.direction[1];
        } else if (current_row - paddle_position == -1) {
            // bounce on the right of the paddle, direction goes right
            if (ball.direction[0] > -1) {
                ball.direction[0] = ball.direction[0] - 1;
            }
            ball.direction[1] = (-1) * ball.direction[1];
        } else if ( next_row - paddle_position == -1 ||
                    next_row - paddle_position == 1) {
            // bounce on the corner of the paddle, direction swaps
            ball.direction[0] = (-1) * ball.direction[0];
            ball.direction[1] = (-1) * ball.direction[1];
        }
    }

    // bounce off the top and bottom
    if (next_row < 0 || next_row > 6) {
        ball.direction[0] = (-1) * ball.direction[0];
    }

    ball.position[0] = ball.position[0] + ball.direction[0];
    ball.position[1] = ball.position[1] + ball.direction[1];
    return ball;
}


/* Changes direction on the ball if it was hit by the paddle */
Ball check_if_hit(Ball ball, int paddle_position)
{
    if (ball.position[1] == 4) {
       if (ball.position[0] - paddle_position == 1 && ball.direction[0] < 1) {
           ball.direction[0] = ball.direction[0] + 1;
           ball.direction[1] = ball.direction[1] * -1;
       } else if (ball.position[0] - paddle_position == -1 && ball.direction[0] > -1) {
           ball.direction[0] = ball.direction[0] - 1;
           ball.direction[1] = ball.direction[1] * -1;
       }
   }
   return ball;
}


/* Initialises the ball on the paddle */
Ball ready_ball(Ball ball, int paddle_position)
{
    ball.visible = 1;
    ball.position[0] = paddle_position;
    ball.position[1] = 3;
    return ball;
}

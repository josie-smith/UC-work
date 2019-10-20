/*
File:   game.c
Author: Josie Smith (jcs162) and Yu Wen (ywe39)
Date:   11/10/2019
Descr:  Main game file for pong.
Included files excet those in this directory are not written by me - see the
individual .h files for information about the author.
*/

#include "system.h"
#include "pio.h"
#include "ir_uart.h"
#include "pacer.h"
#include "navswitch.h"
#include "tinygl.h"
#include "../fonts/font3x5_1.h"
#include "button.h"
#include "led_display.h"
#include "ball.h"
#include "communications.h"

#define PACER_RATE 1000
#define READ_IR_RATE 250
#define DISPLAY_TASK_RATE 1000
#define BALL_TASK_RATE 5
#define BUTTONS_TASK_RATE 250
#define TEXT_INIT 1000

#define MAX_SCORE 5


int paddle_position = 3;
int score = 0;
Ball ball = {0,{3,3},{0,0}};
states_t state = START;
int just_sent = 0;
states_t saved_state = START;


/** Reads the IR data and acts on it  */
static void read_ir_task(void)
{
    if (ir_uart_read_ready_p ()) {
        uint8_t data;
        data = ir_uart_getc ();

        if (START_NUMBERS - 1 <= data && data < START_NUMBERS + 20 && !just_sent) {
            state = PLAYING;
            ball = get_ball(data, ball);
        } else if (data == SCORED_CODE) {
            state = SCORED;
            score = score + 1;
            if (score >= MAX_SCORE) {
                change_state(SCOREBOARD, &state);
            }
        } else if (data == SCOREBOARD_CODE) {
            state = SCOREBOARD;
        } else if (data == PAUSE_CODE) {
            state = PAUSE;
        } else if (data == START_CODE) {
            score = 0;
            ball.visible = 0;
            state = START;
        } else if (data == PLAYING_CODE) {
            state = PLAYING;
        } else if (data == READY_CODE) {
            state = READY;
        }
    }
}


/** updates the location etc of the ball  */
static void ball_update_task(void)
{
    if (state == PLAYING) {
        if (just_sent > 0) {
            just_sent = just_sent - 1;;
        };
        if (ball.visible) {
            ball = update_ball(ball, paddle_position);
            if (ball.position[1] < 0) {
                // ball passes to the other screen
                ball = send_ball(ball);
                just_sent = 4;
            } else if (ball.position[1] > 4) {
                // ball scores
                ball.visible = 0;
                change_state(SCORED, &state);
                ball = ready_ball(ball, paddle_position);
            }
        }
    }
}


/** Reads the navswitch actions and acts on them  */
static void buttons_task (void)
{
    navswitch_update ();
    button_update ();

    if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
        if ( paddle_position < 5 ) {
            paddle_position++;
            ball = check_if_hit(ball, paddle_position);
        }
    }
    if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
        if ( paddle_position > 1 ) {
            paddle_position--;
            ball = check_if_hit(ball, paddle_position);
        }
    }

    if (state == START) {
        if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
            ball = ready_ball(ball, paddle_position);
            change_state(READY, &state);
        }
    } else if (state == READY || state == SCORED) {
        if (ball.visible == 1) {
            ball.position[0] = paddle_position;
            if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
                change_state(PLAYING, &state);
                ball = push_ball(ball);
            }
        }
    } else if (state == SCOREBOARD) {
        if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
            score = 0;
            ball.visible = 0;
            change_state(START, &state);
        }
    }

    if (button_push_event_p (BUTTON1) && state != START){
        if (state == PAUSE) {
            change_state(saved_state, &state);
        } else {
            saved_state = state;
            if (saved_state == SCORED) saved_state = READY;
            change_state(PAUSE, &state);
        }
    }
}


/** Uses a pacer to control the rates for reading from the IR, updating the ball, reading from the navswitch and updating the display. */
int main (void)
{
    system_init ();
    matrix_init ();
    pacer_init (PACER_RATE);
    navswitch_init ();
    ir_uart_init ();
    tinygl_init (TEXT_INIT);
    tinygl_font_set (&font3x5_1);
    tinygl_text_dir_set (TINYGL_TEXT_DIR_ROTATE);

    uint8_t read_ir_tick = 0;
    uint8_t buttons_tick = 0;
    uint8_t ball_update_tick = 0;
    uint8_t display_tick = 0;
    int display = 0;

    while (1)
    {
        pacer_wait ();

        read_ir_tick++;
        if (read_ir_tick >= PACER_RATE/READ_IR_RATE) {
            read_ir_tick = 0;
            read_ir_task();
        }

        buttons_tick++;
        if (buttons_tick >= PACER_RATE/BUTTONS_TASK_RATE) {
            buttons_tick = 0;
            buttons_task();
        }

        ball_update_tick++;
        if (ball_update_tick >= PACER_RATE/BALL_TASK_RATE) {
            ball_update_tick = 0;
            ball_update_task();
        }

        display_tick++;
        if (display_tick >= PACER_RATE/DISPLAY_TASK_RATE) {
            display_tick = 0;
            if (state == SCOREBOARD || state == PAUSE) {
                make_text(score);
                tinygl_update ();
            } else {
                if (display == 0) {
                    if (ball.visible) {
                        display_ball(ball);
                    }
                    display = 1;
                } else {
                    display_paddle (paddle_position);
                    display = 0;
                }
            }
        }
    }
}

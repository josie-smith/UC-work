/*
File:   led_display.h
Author: Josie Smith (jcs162) and Yu Wen (ywe39)
Date:   11/10/2019
Descr:  Displays a static paddle and also inits the LED matrix.
*/
#ifndef LED_DISPLAY_H
#define LED_DISPLAY_H

#include "system.h"
#include "pio.h"
#include "tinygl.h"


/* Initialises the LED matrix  */
void matrix_init(void);

/* Creates a static paddle around the position specified */
void display_paddle(int paddle_position);

/* Writes the score to tinygl */
void make_text(int score);


#endif //LED_DISPLAY_H

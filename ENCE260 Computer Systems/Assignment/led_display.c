/*
File:   led_display.c
Author: Josie Smith (jcs162) and Yu Wen (ywe39)
Date:   11/10/2019
Descr:  Displays a static paddle and also inits the LED matrix.
*/
#include "system.h"
#include "pio.h"
#include "tinygl.h"
#include "led_display.h"

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


/** Initialises the LED matrix  */
void matrix_init(void)
{
    for (int col=0; col<LEDMAT_COLS_NUM; col++) {
        pio_config_set(cols[col], PIO_OUTPUT_HIGH);
    }
    for (int row=0; row<LEDMAT_ROWS_NUM; row++) {
        pio_config_set(rows[row], PIO_OUTPUT_HIGH);
    }
}


/* Creates a static paddle around the position specified */
void display_paddle(int paddle_position)
{
    for (int i = 0; i < 5; i++) {
        pio_output_high(cols[i]);
    }
    for (int i = 0; i < 7; i++) {
        pio_output_high(rows[i]);
    }

    pio_output_low(cols[4]);
    pio_output_low(rows[paddle_position-1]);
    pio_output_low(rows[paddle_position]);
    pio_output_low(rows[paddle_position+1]);
}


/* Writes the score to tinygl */
void make_text(const int score)
{
    char str[3];
    str[0] = ((score / 10) % 10) + '0';
    str[1] = (score % 10) + '0';
    str[2] = 0;
    tinygl_text (str);
}

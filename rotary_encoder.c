/* Copyright 2021 Tim Siegel <siegeltr@gmail.com>
 * Licenced under the GNU GPL Version 3

 * Portions of this code derived from https://github.com/buxtronix/arduino/:
 * Copyright 2011 Ben Buxton. Licenced under the GNU GPL Version 3.
 */

#include "rotary_encoder.h"

#include <stdio.h>
#include <stdlib.h>

#define ROTENC_DIR_CW   (1 << 4)
#define ROTENC_DIR_CCW  (3 << 4)  /* AKA 1 << 4 | 1 << 5 */

/*
 * State table states
 */

/* Invalid state transitions might happen if reading pins isn't clean? */
#define ROTENC_INVALID 0
#define ROTENC_CCW_FINAL 1
#define ROTENC_CCW_NEXT 2
#define ROTENC_CCW_BEGIN 3
#define ROTENC_START _ROT_INITIAL_STATE_INDEX
#define ROTENC_CW_BEGIN 5
#define ROTENC_CW_NEXT 6
#define ROTENC_CW_FINAL 7

#if defined(ROTENC_FULL_STEP)

/*
    Encoder with 4 states:
    01  CCW_FINAL
    00  CCW_NEXT
    10  CCW_BEGIN
    11  START   <-- Detent
    01  CW_BEGIN
    00  CW_NEXT
    10  CW_FINAL
*/

_rotenc_ttable_t _rotenc_ttable_full = {
    /* INVALID */
    {ROTENC_START, ROTENC_CW_BEGIN, ROTENC_CCW_BEGIN, ROTENC_START},

    /* CCW_FINAL */
    {ROTENC_CCW_NEXT, ROTENC_CCW_FINAL, ROTENC_INVALID, ROTENC_START | ROTENC_DIR_CCW},

    /* CCW_NEXT */
    {ROTENC_CCW_NEXT, ROTENC_CCW_FINAL, ROTENC_CCW_BEGIN, ROTENC_INVALID},

    /* CCW_BEGIN */
    {ROTENC_CCW_NEXT, ROTENC_INVALID, ROTENC_CCW_BEGIN, ROTENC_START},

    /* START */
    {ROTENC_INVALID, ROTENC_CW_BEGIN, ROTENC_CCW_BEGIN, ROTENC_START},

    /* CW_BEGIN */
    {ROTENC_CW_NEXT, ROTENC_CW_BEGIN, ROTENC_INVALID, ROTENC_START},

    /* CW_NEXT */
    {ROTENC_CW_NEXT, ROTENC_CW_BEGIN, ROTENC_CW_FINAL, ROTENC_INVALID},

    /* CW_FINAL */
    {ROTENC_CW_NEXT, ROTENC_INVALID, ROTENC_CW_FINAL, ROTENC_START | ROTENC_DIR_CW}

};

#endif

#if defined(ROTENC_HALF_STEP)

/*
    Encoder with 4 states, which fires on both 00 and 11:
    01  CCW_FINAL
    00  CCW_NEXT
    10  CCW_BEGIN
    11  START   <-- Detent
    01  CW_BEGIN
    00  CW_NEXT
    10  CW_FINAL
*/

_rotenc_ttable_t _rotenc_ttable_half = {
    /* INVALID */
    {ROTENC_START, ROTENC_CW_BEGIN, ROTENC_CCW_BEGIN, ROTENC_START},

    /* CCW_FINAL */
    {ROTENC_CCW_NEXT, ROTENC_CCW_FINAL, ROTENC_INVALID, ROTENC_START | ROTENC_DIR_CCW},

    /* CCW_NEXT */
    {ROTENC_CCW_NEXT, ROTENC_CCW_FINAL, ROTENC_CW_FINAL, ROTENC_INVALID},

    /* CCW_BEGIN */
    {ROTENC_CCW_NEXT | ROTENC_DIR_CCW, ROTENC_INVALID, ROTENC_CCW_BEGIN, ROTENC_START},

    /* START */
    {ROTENC_INVALID, ROTENC_CW_BEGIN, ROTENC_CCW_BEGIN, ROTENC_START},

    /* CW_BEGIN */
    {ROTENC_CW_NEXT | ROTENC_DIR_CW, ROTENC_CW_BEGIN, ROTENC_INVALID, ROTENC_START},

    /* CW_NEXT */
    {ROTENC_CW_NEXT, ROTENC_CCW_FINAL, ROTENC_CW_FINAL, ROTENC_INVALID},

    /* CW_FINAL */
    {ROTENC_CW_NEXT, ROTENC_INVALID, ROTENC_CW_FINAL, ROTENC_START | ROTENC_DIR_CW}
};

#endif

#if defined(ROTENC_TRISTATE)

/*
    Encoder with three states: 00, 10, 11
    00 CCW_FINAL
    10 CCW_BEGIN
    11 START <-- Detent
    00 CW_BEGIN
    10 CW_FINAL
*/

_rotenc_ttable_t _rotenc_ttable_tristate = {
    /* INVALID */
    {ROTENC_START, ROTENC_CW_BEGIN, ROTENC_CCW_BEGIN, ROTENC_START},

    /* CCW_FINAL */
    {ROTENC_CCW_FINAL, ROTENC_START, ROTENC_CCW_BEGIN, ROTENC_START | ROTENC_DIR_CCW},

    /* CCW_NEXT */
    {ROTENC_INVALID, ROTENC_INVALID, ROTENC_INVALID, ROTENC_INVALID},

    /* CCW_BEGIN */
    {ROTENC_CCW_FINAL, ROTENC_START, ROTENC_CCW_BEGIN, ROTENC_START},

    /* START */
    {ROTENC_CW_BEGIN, ROTENC_START, ROTENC_CCW_BEGIN, ROTENC_START },

    /* CW_BEGIN */
    {ROTENC_CW_BEGIN, ROTENC_START, ROTENC_CW_FINAL, ROTENC_START},

    /* CW_NEXT */
    {ROTENC_INVALID, ROTENC_INVALID, ROTENC_INVALID, ROTENC_INVALID},

    /* CW_FINAL */
    {ROTENC_CW_BEGIN, ROTENC_START, ROTENC_CW_FINAL, ROTENC_START | ROTENC_DIR_CW}
};

#endif


signed char rotenc_process_pins(rotenc_encoder_t *encoder, unsigned char pins) {
    unsigned char dir;
    encoder->_state = (*((_rotenc_ttable_t *)encoder->_ttable))[encoder->_state & 0x0f][pins];

    /* If CW is set, dir = 1; if CCW, dir = 3 */
    dir = (encoder->_state & (ROTENC_DIR_CW | ROTENC_DIR_CCW)) >> 4;

    /* 2-1 == 1; 2-3 == -1 */
    return (dir ? 2 - dir : 0);
}

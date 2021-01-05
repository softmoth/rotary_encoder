/* Copyright 2021 Tim Siegel <siegeltr@gmail.com>
 * Licenced under the GNU GPL Version 3

 * Portions of this code derived from https://github.com/buxtronix/arduino/:
 * Copyright 2011 Ben Buxton. Licenced under the GNU GPL Version 3.
 */

#ifndef _rotary_encoder_h
#define _rotary_encoder_h

/**
 * Each encoder knows what type of encoder it is (full-step, half-step, or
 * tristate), and what state it is in. Treat this as an opaque handle.
 *
 * Example:
 * in setup(), or static global scope, etc. {
 *     rotenc_encoder_t fullstep_encoder = ROTENC_FULL_STEP_INIT;
 *     rotenc_encoder_t halfstep_encoder = ROTENC_HALF_STEP_INIT;
 *     rotenc_encoder_t tristate_encoder = ROTENC_TRISTATE_INIT;
 * }
 */
typedef struct {
    void *_ttable;
    unsigned char _state;
} rotenc_encoder_t;

/**
 * Process pins to track the state of rotation
 *
 * Parameters:
 * enc: Opaque representation, see ROTINC_FULL_STEP_INIT, etc., below
 * pins: Integer in range [0..3]; one bit set from each pin of the encoder
 *
 * Returns -1 on counter-clockwise move, +1 on clockwise move, else 0
 *
 * Example:
 * in loop (polling) or ISR (interrupt service routine) {
 *     unsigned char pins = (digitalRead(pin2) << 1) | digitalRead(pin1);
 *     signed char rotation = rotenc_process_pins(&fullstep_encoder, pins);
 *
 *     switch (rotation) {
 *     case -1: my_counter -= 1; break;  // Counter-clockwise
 *     case  1: my_counter += 1; break;  // Clockwise
 *     default:
 *     }
 * }
 */
signed char rotenc_process_pins(rotenc_encoder_t *enc, unsigned char pins);

/** If not using all three types of encoders, you may save a bit of
 * space by defining only the types of encoders you want.
 */
#if !defined(ROTENC_FULL_STEP) && !defined(ROTENC_HALF_STEP) && !defined(ROTENC_TRISTATE)
#   define ROTENC_FULL_STEP
#   define ROTENC_HALF_STEP
#   define ROTENC_TRISTATE
#endif


/*
 * Implementation details that have to be lifted up here for the
 * initialization macros to work
 */

/* The linker should be able to put the state tables in the read-only data
 * section so it doesn't use up RAM. If you need to make any modifications
 * to this library to achieve that, please open an issue or a pull request.
 */
typedef const unsigned char (_rotenc_ttable_t)[8][4];

/* Implementation detail of the state machine */
#define _ROT_INITIAL_STATE_INDEX 4


#ifdef ROTENC_FULL_STEP
    extern _rotenc_ttable_t _rotenc_ttable_full;
#   define ROTENC_FULL_STEP_INIT \
            { (void *)&_rotenc_ttable_full, _ROT_INITIAL_STATE_INDEX }
#endif

#ifdef ROTENC_HALF_STEP
    extern _rotenc_ttable_t _rotenc_ttable_half;
#   define ROTENC_HALF_STEP_INIT \
            { (void *)&_rotenc_ttable_half, _ROT_INITIAL_STATE_INDEX }
#endif

#ifdef ROTENC_TRISTATE
    extern _rotenc_ttable_t _rotenc_ttable_tristate;
#   define ROTENC_TRISTATE_INIT \
            { (void *)&_rotenc_ttable_tristate, _ROT_INITIAL_STATE_INDEX }
#endif

#endif /* _rotary_encoder_h */

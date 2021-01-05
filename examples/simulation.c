/* Copyright 2021 Tim Siegel <siegeltr@gmail.com>
 * Licenced under the GNU GPL Version 3
 *
 * Test program that models 3 different encoder types, and simulates turning
 * them in either direction and switching between them.
 *
 * Keys:
 * j, l     Rotate left (counter-clockwise)
 * k, r     Rotate right (clockwise)
 * f        Switch to FULL_STEP encoder (if defined)
 * h        Switch to HALF_STEP encoder (if defined)
 * t        Switch to TRISTATE encoder (if defined)
 * q        Quit
 */

#include "rotary_encoder.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <curses.h>

#if defined(ROTENC_FULL_STEP) || defined(ROTENC_HALF_STEP)
static const unsigned char Pin_Values[] = {
    /* Gray code sequence for rotary encoder */
    3,  /* 11 */
    1,  /* 01 */
    0,  /* 00 */
    2   /* 10 */
};
#endif

#if defined(ROTENC_TRISTATE)
static const unsigned char Pin_Values_3[] = {
    /* Tri-state encoder */
    3,  /* 11 */
    0,  /* 00 */
    2   /* 10 */
};
#endif

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

struct EncType {
    const char *name;
    const unsigned char *vals;
    const unsigned char size;
};

#if defined(ROTENC_FULL_STEP)
static struct EncType Fullstep_Type = { "Full", Pin_Values, ARRAY_SIZE(Pin_Values) };
#endif
#if defined(ROTENC_HALF_STEP)
static struct EncType Halfstep_Type = { "Half", Pin_Values, ARRAY_SIZE(Pin_Values) };
#endif
#if defined(ROTENC_TRISTATE)
static struct EncType Tristate_Type = { "Tri ", Pin_Values_3, ARRAY_SIZE(Pin_Values_3) };
#endif

struct Encoder {
    struct EncType *type;
    rotenc_encoder_t enc;
    unsigned char index;
};


int main(void)
{
#if defined(ROTENC_FULL_STEP)
    struct Encoder encoder_f = { &Fullstep_Type, ROTENC_FULL_STEP_INIT, 0 };
#endif
#if defined(ROTENC_HALF_STEP)
    struct Encoder encoder_h = { &Halfstep_Type, ROTENC_HALF_STEP_INIT, 0 };
#endif
#if defined(ROTENC_TRISTATE)
    struct Encoder encoder_t = { &Tristate_Type, ROTENC_TRISTATE_INIT, 0 };
#endif

    struct Encoder *encoder =
#if defined(ROTENC_FULL_STEP)
        &encoder_f
#elif defined(ROTENC_HALF_STEP)
        &encoder_h
#elif defined(ROTENC_TRISTATE)
        &encoder_t
#endif
        ;

    int val = 0;

    /* Init screen */
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    nonl();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    /* Scroll off the bottom of the window */
    idlok(stdscr, TRUE);
    scrollok(stdscr, TRUE);

    while (1) {
        char buf[80];

        snprintf(buf, sizeof(buf),
                "%s: %d; ",
                encoder->type->name,
                /* XXX: DEBUGGING ONLY: Peek into internal state! */
                encoder->enc._state & 0x0f);
        addstr(buf);

        int ch = getch();
        switch (ch) {
            case ERR:
            case 27: /* ESC */
            case 'q':
                goto ENDLOOP;
            case ',':
            case 'j':
            case 'l':
                ch = 'L';
                encoder->index = (encoder->index + encoder->type->size - 1) % encoder->type->size;
                break;
            case '.':
            case 'k':
            case 'r':
                ch = 'R';
                encoder->index = (encoder->index + 1) % encoder->type->size;
                break;
#ifdef ROTENC_FULL_STEP
            case 'f':
                encoder = &encoder_f;
                break;
#endif
#ifdef ROTENC_HALF_STEP
            case 'h':
                encoder = &encoder_h;
                break;
#endif
#ifdef ROTENC_TRISTATE
            case 't':
                encoder = &encoder_t;
                break;
#endif
            default:
                snprintf(buf, sizeof(buf), "Input '%c' [%d]; use 'q' to quit\n", ch, ch);
                addstr(buf);
                ;
        }
        assert(encoder->index < encoder->type->size);

        val += rotenc_process_pins(&encoder->enc, encoder->type->vals[encoder->index]);

        snprintf(buf, sizeof(buf),
                "Input: %c, pins: %d, State: %d\t\tVal: %d\n",
                ch, encoder->type->vals[encoder->index],
                /* XXX: DEBUGGING ONLY: Peek into internal state! */
                encoder->enc._state & 0x0f,
                val);
        addstr(buf);
        refresh();

        continue;

    ENDLOOP:
        break;
    }

    endwin();
    exit(EXIT_SUCCESS);
}

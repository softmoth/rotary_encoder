/* This code is in the public domain */

#include "rotary_encoder.h"

#include <stdlib.h>

/* Fix these based on your environment */
#define digitalRead(pin) 0

#define READ_ENCODER_PINS(pin1, pin2) \
            (digitalRead(pin1) | (digitalRead(pin2) << 1))


/* Simple counter to track the number of turns */
int Counter = 0;

/* Define an encoder */
#define ENCODER_PIN_A 8
#define ENCODER_PIN_B 9
static rotenc_encoder_t Encoder = ROTENC_FULL_STEP_INIT;

void process_encoder(void) {
    Counter += rotenc_process_pins(&Encoder,
                    READ_ENCODER_PINS(ENCODER_PIN_A, ENCODER_PIN_B));
}

int main(void)
{
    /* Setup */

    /* Depends on your environment */
    /* register_ISR(ENCODER_PIN_A, process_encoder) */
    /* register_ISR(ENCODER_PIN_B, process_encoder) */

    /* Loop */
    while (1) {
        /* Or, just poll periodically */
        process_encoder();

        /* Now, do something with Counter */

        exit(0);
    }
}

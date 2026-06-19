#include "gpio.h"
#include "stm32f103.h"

#define DOT_DURATION_MS 200U
#define DASH_DURATION_MS (DOT_DURATION_MS * 3U)
#define ELEMENT_SPACE_MS DOT_DURATION_MS
#define LETTER_SPACE_MS (DOT_DURATION_MS * 3U)
#define WORD_SPACE_MS (DOT_DURATION_MS * 7U)

static const char* MORSE_ALPHABET[26] = {".-",   "-...", "-.-.", "-..",  ".",    "..-.", "--.",
                                         "....", "..",   ".---", "-.-",  ".-..", "--",   "-.",
                                         "---",  ".--.", "--.-", ".-.",  "...",  "-",    "..-",
                                         "...-", ".--",  "-..-", "-.--", "--.."};

void delay_ms(uint32_t ms)
{
    volatile uint32_t count = ms * 800U;
    while (count--)
    {
        __asm__("nop");
    }
}

void transmit_symbol(uint32_t duration_ms)
{
    gpio_set_pin(GPIOA, 0);
    delay_ms(duration_ms);
    gpio_clear_pin(GPIOA, 0);
    delay_ms(ELEMENT_SPACE_MS);
}

void morse_transmit_string(const char* str)
{
    for (uint32_t i = 0; str[i] != '\0'; i++)
    {
        char c = str[i];

        if (c == ' ')
        {
            delay_ms(WORD_SPACE_MS - ELEMENT_SPACE_MS);
            continue;
        }

        if (c >= 'a' && c <= 'z')
        {
            c -= 32;
        }

        if (c >= 'A' && c <= 'Z')
        {
            const char* m = MORSE_ALPHABET[c - 'A'];

            for (uint32_t j = 0; m[j] != '\0'; j++)
            {
                transmit_symbol(m[j] == '.' ? DOT_DURATION_MS : DASH_DURATION_MS);
            }

            delay_ms(LETTER_SPACE_MS - ELEMENT_SPACE_MS);
        }
    }
}

int main(void)
{
    RCC_APB2ENR |= RCC_APB2ENR_IOPAEN;

    gpio_setup(GPIOA, 0, GPIO_MODE_OUTPUT_2MHz, GPIO_CNF_PUSHPULL);
    gpio_clear_pin(GPIOA, 0);

    while (1)
    {
        morse_transmit_string("SOS");
        delay_ms(3000);
    }
}

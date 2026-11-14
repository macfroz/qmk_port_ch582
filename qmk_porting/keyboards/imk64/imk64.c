/*
Copyright 2023 SharpSnail01 <https://github.com/SharpSnail01>
Copyright 2023 Jactry Zeng

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H

#ifdef RGB_MATRIX_ENABLE

/* clang-format off */
led_config_t g_led_config = {
#if RGBLED_NUM == 68
    {
        {0,  1,  2,  3,  4,      5,      6,      7,  8,  9,  10, 11, 12, 13},
        {27, 26, 25, 24, 23,     22,     21,     20, 19, 18, 17, 16, 15, 14},
        {28, 29, 30, 31, 32,     33,     34,     35, 36, 37, 38, 39, 40, 41},
        {55, 54, 53, 52, 51,     50,     49,     48, 47, 46, 45, 44, 43, 42},
        {56, 57, 58, 59, NO_LED, NO_LED, NO_LED, 61, 62, 63, 64, 65, 66, 67},
    }, {
        {0,0},    {17,0},   {34,0},   {52,0},   {69,0},   {86,0},   {103,0},  {121,0},  {138,0},  {155,0},  {172,0},  {190,0},  {207,0},  {224,0},
        {224,16}, {207,16}, {190,16}, {172,16}, {155,16}, {138,16}, {121,16}, {103,16}, {86,16},  {69,16},  {52,16},  {34,16},  {17,16},  {0,16},
        {0,32},   {17,32},  {34,32},  {52,32},  {69,32},  {86,32},  {103,32}, {121,32}, {138,32}, {155,32}, {172,32}, {190,32}, {207,32}, {224,32},
        {224,48}, {207,48}, {190,48}, {172,48}, {155,48}, {138,48}, {121,48}, {103,48}, {86,48},  {69,48},  {52,48},  {34,48},  {17,48},  {0,48},
        {0,64},   {17,64},  {34,64},  {69,64},                                {121,64}, {138,64}, {155,64}, {172,64}, {190,64}, {207,64}, {224,64},
    }, {
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    }
#else
{
        {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,  11,  12,  13}, 
        {27,  26,  25,  24,  23,  22,  21,  20,  19,  18,  17,  16,  15,  14}, 
        {28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,   NO_LED}, 
        {54,  53,  52,  51,  50,  49,  48,  47,  46,  45,  44,  43,  42,  41}, 
        {55,  56,  57,  58,   NO_LED, NO_LED,   NO_LED,  NO_LED, NO_LED, 59, 60,   61,  62,  63},
	}, {
        {0,0},{17,0},{34,0},{52,0},{69,0},{86,0},{103,0},{121,0},{138,0},{155,0},{172,0},{190,0},{207,0},{224,0},
        {224,16},{207,16},{190,16},{172,16},{155,16},{138,16},{121,16},{103,16},{86,16},{69,16},{52,16},{34,16},{17,16},{0,16},
        {0,32},{17,32},{34,32},{52,32},{69,32},{86,32},{103,32},{121,32},{138,32},{155,32},{172,32},{190,32},{207,32},       
        {224,48},{207,48},{190,48},{172,48},{155,48},{138,48},{121,48},{103,48},{86,48},{69,48},{52,48},{34,48},{17,48},{0,48},
        {0,64},{17,64},{34,64},{52,64},                      {155,64},  {172,64},  {190,64},{207,64},{224,64},
	}, {
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 
		4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 
	}
#endif
};
/* clang-format on */

static bool     embers_initialized = false;
static uint8_t  ember_phase[RGB_MATRIX_LED_COUNT];
static uint8_t  ember_speed[RGB_MATRIX_LED_COUNT];
static uint8_t  ember_peak[RGB_MATRIX_LED_COUNT];
static uint16_t last_ember_tick = 0;
static uint16_t ember_rng       = 0xACE1u;

static uint8_t next_ember_rand(void) {
    ember_rng = (ember_rng >> 1) ^ (-(ember_rng & 1u) & 0xB400u);
    return (uint8_t)(ember_rng & 0xFFu);
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max)
{
    uint8_t mode = rgb_matrix_get_mode();

    // Включаем эффект "Белые тлеющие угли" на первом режиме (Solid Color)
    // и вместо всех реактивных режимов с клиньями (начиная с 30-го)
    if (mode == 1 || mode >= 30) {
        if (!embers_initialized) {
            for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
                ember_phase[i] = next_ember_rand();
                // ~55% клавиш тлеют одновременно, остальные в темноте
                ember_speed[i] = (next_ember_rand() < 140) ? ((next_ember_rand() % 3) + 1) : 0;
                // Разная максимальная яркость для каждого уголька (от 100 до 255)
                ember_peak[i]  = 100 + (next_ember_rand() % 156);
            }
            last_ember_tick    = timer_read();
            embers_initialized = true;
        }

        uint16_t delay = ((uint16_t)(255 - rgb_matrix_config.speed) / 6) + 12;
        if (timer_elapsed(last_ember_tick) > delay) {
            last_ember_tick = timer_read();

            for (uint8_t k = 0; k < RGB_MATRIX_LED_COUNT; k++) {
                if (ember_speed[k] > 0) {
                    uint8_t old_p = ember_phase[k];
                    ember_phase[k] += ember_speed[k];

                    // Когда клавиша плавно дотлела до нуля
                    if (ember_phase[k] < old_p) {
                        ember_phase[k] = 0;
                        if (next_ember_rand() < 128) {
                            ember_speed[k] = (next_ember_rand() % 3) + 1;
                            ember_peak[k]  = 100 + (next_ember_rand() % 156);
                        } else {
                            ember_speed[k] = 0;
                        }
                    }
                } else {
                    // Потухшая клавиша с небольшим шансом начинает плавно разгораться
                    if (next_ember_rand() < 8) {
                        ember_phase[k] = 0;
                        ember_speed[k] = (next_ember_rand() % 3) + 1;
                        ember_peak[k]  = 100 + (next_ember_rand() % 156);
                    }
                }
            }
        }

        uint8_t max_val = rgb_matrix_config.hsv.v;

        for (uint8_t i = led_min; i < led_max; i++) {
            uint8_t white = 0;
            if (ember_speed[i] > 0) {
                uint8_t p = ember_phase[i];
                // Треугольная волна 0 -> 254 -> 0
                uint8_t tri = (p & 0x80) ? (255 - ((p & 0x7F) << 1)) : ((p & 0x7F) << 1);
                // Квадратичное сглаживание (гамма-кривая 2.0 для идеально мягкого тления)
                uint8_t smooth = (uint8_t)(((uint16_t)tri * tri) >> 8);
                uint8_t scaled = (uint8_t)(((uint16_t)smooth * ember_peak[i]) >> 8);
                white          = (uint8_t)(((uint16_t)scaled * max_val) >> 8);
            }
            rgb_matrix_set_color(i, white, white, white);
        }
    }

    if (led_min <= 28 && led_max > 28 && host_keyboard_led_state().caps_lock) {
        RGB_MATRIX_INDICATOR_SET_COLOR(28, 0xFF, 0x00, 0x00);
    }

    return true;
}

#endif

int main()
{
    extern void protocol_setup();
    extern void protocol_pre_init();
    extern void protocol_post_init();
    extern void platform_run();

    platform_setup();

    protocol_setup();
#if !defined ESB_ENABLE || ESB_ENABLE != 2
    keyboard_setup();
#endif

    protocol_pre_init();
    keyboard_init();
    protocol_post_init();

    /* Main loop */
    for (;;) {
        platform_run();
        //! housekeeping_task() is handled by platform
    }
}

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
static uint16_t ignite_delay[RGB_MATRIX_LED_COUNT];
static uint16_t last_ember_tick = 0;
static uint32_t boot_start_time = 0;
static uint16_t ember_rng       = 0xACE1u;
static uint8_t  boot_anim_type  = 1;
static uint8_t  last_seen_mode  = 0xFF;

static uint8_t next_ember_rand(void) {
    ember_rng = (ember_rng >> 1) ^ (-(ember_rng & 1u) & 0xB400u);
    return (uint8_t)(ember_rng & 0xFFu);
}

static void init_ignite_delays(uint8_t anim_type) {
    for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        if (anim_type == 1) {
            // 1. Каскадный розжиг (Cascade Ignition)
            uint8_t r       = next_ember_rand();
            ignite_delay[i] = ((uint32_t)r * r * 1700UL) / 65025UL;
        } else if (anim_type == 2) {
            // 2. Световая волна слева направо (Scanner Wave)
            uint8_t x       = (i < 64) ? g_led_config.point[i].x : ((i - 64) * 12);
            ignite_delay[i] = ((uint32_t)x * 1600UL) / 224UL;
        } else if (anim_type == 3) {
            // 3. Сверхновая из центра (Center Supernova)
            int16_t dx      = (i < 64) ? ((int16_t)g_led_config.point[i].x - 112) : 112;
            int16_t dy      = (i < 64) ? (((int16_t)g_led_config.point[i].y - 32) * 2) : 64;
            if (dx < 0) dx = -dx;
            if (dy < 0) dy = -dy;
            uint16_t dist   = (uint16_t)(dx + dy);
            if (dist > 180) dist = 180;
            ignite_delay[i] = ((uint32_t)dist * 1600UL) / 180UL;
        } else if (anim_type == 4) {
            // 4. Заполнение по рядам сверху вниз (Matrix Fill)
            uint8_t x       = (i < 64) ? g_led_config.point[i].x : 224;
            uint8_t y       = (i < 64) ? g_led_config.point[i].y : 64;
            uint16_t pos    = ((uint16_t)(y / 16) * 240u) + x;
            ignite_delay[i] = ((uint32_t)pos * 1700UL) / 1200UL;
        } else {
            ignite_delay[i] = 0;
        }
    }
    if (anim_type == 1) {
        ignite_delay[33] = 0;
    }
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max)
{
    uint8_t mode = rgb_matrix_get_mode();

    if (mode == 1 || mode >= 30) {
        // Определяем тип анимации загрузки по выбранному пункту в VIA:
        // 40 = Без анимации загрузки
        // 1 или 41 = Каскадный розжиг
        // 42 = Световая волна слева направо
        // 43 = Сверхновая из центра
        // 44 = Заполнение матрицы по рядам
        uint8_t target_boot = 1;
        if (mode == 40) target_boot = 0;
        else if (mode == 41) target_boot = 1;
        else if (mode == 42) target_boot = 2;
        else if (mode == 43) target_boot = 3;
        else if (mode == 44) target_boot = 4;

        if (!embers_initialized) {
            for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
                ember_phase[i] = next_ember_rand();
                // Скорость каждого уголька 1 или 2 (плавное, неспешное тление)
                ember_speed[i] = (next_ember_rand() < 145) ? ((next_ember_rand() & 1u) + 1u) : 0;
                ember_peak[i]  = 110 + (next_ember_rand() % 146);
            }
            boot_anim_type     = target_boot;
            init_ignite_delays(boot_anim_type);
            last_ember_tick    = timer_read();
            boot_start_time    = timer_read32();
            last_seen_mode     = mode;
            embers_initialized = true;
        } else if (mode != last_seen_mode) {
            // Если вы переключили вариант загрузки в VIA — сразу показываем его!
            last_seen_mode  = mode;
            boot_anim_type  = target_boot;
            init_ignite_delays(boot_anim_type);
            boot_start_time = timer_read32();
        }

        uint32_t boot_elapsed = timer_elapsed32(boot_start_time);

        // Замедлили минимальный шаг таймера:
        // При speed = 0 задержка = 75 мс (очень медленное, плавное дыхание углей)
        // При speed = 255 задержка = 12 мс (быстрое мерцание)
        uint16_t delay = ((uint16_t)(255 - rgb_matrix_config.speed) / 4) + 12;
        if (timer_elapsed(last_ember_tick) > delay) {
            last_ember_tick = timer_read();

            for (uint8_t k = 0; k < RGB_MATRIX_LED_COUNT; k++) {
                if (ember_speed[k] > 0) {
                    uint8_t old_p = ember_phase[k];
                    ember_phase[k] += ember_speed[k];

                    if (ember_phase[k] < old_p) {
                        ember_phase[k] = 0;
                        if (next_ember_rand() < 135) {
                            ember_speed[k] = (next_ember_rand() & 1u) + 1u;
                            ember_peak[k]  = 110 + (next_ember_rand() % 146);
                        } else {
                            ember_speed[k] = 0;
                        }
                    }
                } else {
                    if (next_ember_rand() < 6) {
                        ember_phase[k] = 0;
                        ember_speed[k] = (next_ember_rand() & 1u) + 1u;
                        ember_peak[k]  = 110 + (next_ember_rand() % 146);
                    }
                }
            }
        }

        uint8_t max_val = rgb_matrix_config.hsv.v;

        // Вычисляем чистый цвет на полной точности 255!
        // Если точка в палитре VIA опущена в белую зону (s < 32, как ваш #ffefee),
        // принудительно делаем s = 0, чтобы на низкой яркости НИКОГДА не вылезал красный цвет!
        HSV full_hsv = rgb_matrix_config.hsv;
        if (full_hsv.s < 32) {
            full_hsv.s = 0;
        }
        full_hsv.v     = 255;
        RGB pure_color = hsv_to_rgb(full_hsv);

        for (uint8_t i = led_min; i < led_max; i++) {
            uint8_t ember_val = 0;
            if (ember_speed[i] > 0) {
                uint8_t p      = ember_phase[i];
                uint8_t tri    = (p & 0x80) ? (255 - ((p & 0x7F) << 1)) : ((p & 0x7F) << 1);
                // Та самая плавная кривая, которая вам понравилась!
                uint8_t smooth = (uint8_t)(((uint16_t)tri * tri) >> 8);
                ember_val      = (uint8_t)(((uint16_t)smooth * ember_peak[i]) >> 8);
            }

            uint8_t final_intensity = ember_val;

            if (boot_anim_type > 0 && boot_elapsed < 3200) {
                if (boot_elapsed < 2300) {
                    if (boot_elapsed < ignite_delay[i]) {
                        final_intensity = 0;
                    } else {
                        uint32_t t_on = boot_elapsed - ignite_delay[i];
                        if (t_on < 480) {
                            uint8_t ramp    = (uint8_t)((t_on * 255UL) / 480UL);
                            final_intensity = (uint8_t)(((uint16_t)ramp * ramp) >> 8);
                        } else {
                            final_intensity = 255;
                        }
                    }
                } else {
                    uint32_t fade_t  = boot_elapsed - 2300; // 0..900 мс
                    uint8_t  ember_w = (uint8_t)((fade_t * 255UL) / 900UL);
                    uint8_t  boot_w  = 255 - ember_w;
                    final_intensity  = (uint8_t)(((uint16_t)255 * boot_w + (uint16_t)ember_val * ember_w) >> 8);
                }
            }

            // Масштабируем общую яркость один раз в самом конце:
            uint8_t level = (uint8_t)(((uint16_t)final_intensity * max_val) >> 8);

            if (full_hsv.s == 0) {
                // Идеально чистый белый цвет (без малейшей примеси красного на минимальной яркости!)
                rgb_matrix_set_color(i, level, level, level);
            } else {
                uint8_t r = (uint8_t)(((uint16_t)pure_color.r * level) >> 8);
                uint8_t g = (uint8_t)(((uint16_t)pure_color.g * level) >> 8);
                uint8_t b = (uint8_t)(((uint16_t)pure_color.b * level) >> 8);
                rgb_matrix_set_color(i, r, g, b);
            }
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

RGB_MATRIX_EFFECT(STARRY_NIGHT)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static uint8_t star_phase[RGB_MATRIX_LED_COUNT];
static uint8_t star_speed[RGB_MATRIX_LED_COUNT];
static uint16_t last_star_tick = 0;

static void STARRY_NIGHT_init(void) {
    for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        star_phase[i] = random8();
        star_speed[i] = (random8() < 90) ? random8_min_max(1, 4) : 0;
    }
    last_star_tick = timer_read();
}

static bool STARRY_NIGHT(effect_params_t* params) {
    if (params->init) {
        STARRY_NIGHT_init();
    }

    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    uint16_t delay = scale16by8(40, 255 - rgb_matrix_config.speed) + 5;
    bool update_stars = false;

    if (led_min == 0 && timer_elapsed(last_star_tick) > delay) {
        last_star_tick = timer_read();
        update_stars = true;
    }

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();

        if (update_stars) {
            if (star_speed[i] > 0) {
                uint8_t old_phase = star_phase[i];
                star_phase[i] += star_speed[i];

                // Когда звезда прошла полный цикл и погасла
                if (star_phase[i] < old_phase) {
                    star_phase[i] = 0;
                    star_speed[i] = (random8() < 80) ? random8_min_max(1, 4) : 0;
                }
            } else {
                // Шанс зажечь новую звезду на потухшей клавише
                if (random8() < 5) {
                    star_phase[i] = 0;
                    star_speed[i] = random8_min_max(1, 4);
                }
            }
        }

        uint8_t brightness = 0;
        if (star_speed[i] > 0) {
            brightness = sin8(star_phase[i] - 64);
        }

        HSV hsv = rgb_matrix_config.hsv;
        hsv.v = scale8(brightness, rgb_matrix_config.hsv.v);

        RGB rgb = rgb_matrix_hsv_to_rgb(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }

    return rgb_matrix_check_finished_leds(led_max);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS

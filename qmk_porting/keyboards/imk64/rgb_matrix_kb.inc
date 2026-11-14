RGB_MATRIX_EFFECT(STARRY_NIGHT)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static uint8_t ember_phase[RGB_MATRIX_LED_COUNT];
static uint8_t ember_speed[RGB_MATRIX_LED_COUNT];
static uint8_t ember_max_val[RGB_MATRIX_LED_COUNT];
static uint16_t last_ember_tick = 0;

static void STARRY_NIGHT_init(void) {
    for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        ember_phase[i]   = random8();
        // Около 55% клавиш плавно тлеют одновременно, остальные в темноте
        ember_speed[i]   = (random8() < 140) ? random8_min_max(1, 3) : 0;
        // У каждой клавиши своя пиковая яркость (одни светятся тускло-серым/белым, другие ярко-белым)
        ember_max_val[i] = random8_min_max(100, 255);
    }
    last_ember_tick = timer_read();
}

static bool STARRY_NIGHT(effect_params_t* params) {
    if (params->init) {
        STARRY_NIGHT_init();
    }

    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    // Плавный шаг анимации (скорость регулируется ползунком Effect Speed в VIA)
    uint16_t delay = scale16by8(45, 255 - rgb_matrix_config.speed) + 10;
    if (timer_elapsed(last_ember_tick) > delay) {
        last_ember_tick = timer_read();

        for (uint8_t k = 0; k < RGB_MATRIX_LED_COUNT; k++) {
            if (ember_speed[k] > 0) {
                uint8_t old_p = ember_phase[k];
                ember_phase[k] += ember_speed[k];

                // Когда клавиша полностью потухла
                if (ember_phase[k] < old_p) {
                    ember_phase[k] = 0;
                    if (random8() < 128) {
                        ember_speed[k]   = random8_min_max(1, 3);
                        ember_max_val[k] = random8_min_max(100, 255);
                    } else {
                        ember_speed[k] = 0;
                    }
                }
            } else {
                // Потухшая клавиша с небольшим шансом начинает плавно разгораться
                if (random8() < 8) {
                    ember_phase[k]   = 0;
                    ember_speed[k]   = random8_min_max(1, 3);
                    ember_max_val[k] = random8_min_max(100, 255);
                }
            }
        }
    }

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();

        uint8_t brightness = 0;
        if (ember_speed[i] > 0) {
            // Плавная кривая разгорания и затухания
            uint8_t wave = sin8(ember_phase[i] - 64);
            brightness   = scale8(wave, ember_max_val[i]);
        }

        // Масштабируем под ползунок общей яркости (Brightness) в VIA
        uint8_t white = scale8(brightness, rgb_matrix_config.hsv.v);

        // Чистый белый цвет: R = G = B = white
        rgb_matrix_set_color(i, white, white, white);
    }

    return rgb_matrix_check_finished_leds(led_max);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS

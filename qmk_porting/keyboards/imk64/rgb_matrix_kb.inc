RGB_MATRIX_EFFECT(STARRY_NIGHT)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static uint8_t star_offset[RGB_MATRIX_LED_COUNT];
static uint8_t star_speed[RGB_MATRIX_LED_COUNT];

static void STARRY_NIGHT_init(void) {
    for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        star_offset[i] = random8();
        star_speed[i]  = random8_min_max(1, 5);
    }
}

static bool STARRY_NIGHT(effect_params_t* params) {
    if (params->init) {
        STARRY_NIGHT_init();
    }

    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    // Непрерывное время анимации (работает всегда, без нажатий клавиш)
    uint16_t time = scale16by8(g_rgb_timer, qadd8(rgb_matrix_config.speed / 4, 8));

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();

        // Индивидуальная фаза каждой звезды
        uint8_t phase = (time * star_speed[i]) + star_offset[i];

        // Когда звезда полностью погасла, иногда меняем её смещение для случайности
        if (phase < 4 && (random8() < 15)) {
            star_offset[i] = random8();
        }

        // Делаем так, чтобы звезда плавно зажигалась и половину цикла отдыхала в темноте
        uint8_t wave = sin8(phase);
        uint8_t brightness = (wave > 100) ? scale8(wave - 100, 255) : 0;

        HSV hsv = rgb_matrix_config.hsv;
        hsv.v = scale8(brightness, rgb_matrix_config.hsv.v);

        RGB rgb = rgb_matrix_hsv_to_rgb(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }

    return rgb_matrix_check_finished_leds(led_max);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS

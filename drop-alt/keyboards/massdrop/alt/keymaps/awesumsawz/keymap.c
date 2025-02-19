#include QMK_KEYBOARD_H
#include "led.h"
#include "print.h"

// If you need these aliases, ensure they're defined before usage.
#define NK_TOGG MAGIC_TOGGLE_NKRO
#define RGB_MO RGB_MOD

// Global flag to track caps lock state: false = default (green), true = red.
static bool caps_backlight_on = false;

/**
 * update_rgb_state - Set the backlight color based on the given layer state and caps lock state.
 *
 * Priority:
 *   1. If layer 1 is active, show blue.
 *   2. Otherwise, if caps lock is active, show red.
 *   3. Otherwise, show green.
 */
void update_rgb_state(layer_state_t state) {
    if (state & (1 << 1)) {
        dprintf("update_rgb_state: Layer 1 active: setting blue\n");
        rgblight_sethsv_noeeprom(170, 255, 255);  // Blue for layer 1
    } else {
        if (caps_backlight_on) {
            dprintf("update_rgb_state: Caps lock active: setting red\n");
            rgblight_sethsv_noeeprom(0, 255, 255);    // Red when caps lock is active
        } else {
            dprintf("update_rgb_state: Default: setting green\n");
            rgblight_sethsv_noeeprom(85, 255, 255);   // Green by default
        }
    }
}

/**
 * keyboard_post_init_user - Runs after the keyboard has initialized.
 */
void keyboard_post_init_user(void) {
    rgblight_enable();                              // Enable RGB lighting
    rgblight_mode(RGBLIGHT_MODE_STATIC_LIGHT);      // Static mode
    rgblight_sethsv_noeeprom(85, 255, 255);           // Default to green

    debug_enable   = true;
    debug_matrix   = true;
    debug_keyboard = true;

    // Use the current layer_state for initialization
    update_rgb_state(layer_state);
}

/**
 * process_record_user - Intercepts key events.
 *
 * Here we toggle the caps lock flag on KC_CAPS.
 */
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_CAPS:
            if (record->event.pressed) {
                caps_backlight_on = !caps_backlight_on;
                dprintf("process_record_user: KC_CAPS toggled. New state: %d\n", caps_backlight_on);
                update_rgb_state(layer_state);
            }
            break;
        default:
            break;
    }
    return true;
}

/**
 * layer_state_set_user - Called when the layer state changes.
 */
layer_state_t layer_state_set_user(layer_state_t state) {
    dprintf("layer_state_set_user: New layer state: %lu\n", state);
    update_rgb_state(state);
    return state;
}

/*
   ---------------------------
   Keymap Definitions
   ---------------------------
   (Include your full keymap as needed. Note that we now use RGB_MOD in place of RGB_MO.)
*/
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_65_ansi_blocker(
        KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_DEL, 
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_HOME, 
        KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,  KC_END,  
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT, KC_UP,   KC_RGUI, 
        KC_CAPS, KC_LGUI, KC_LALT,                   KC_SPC,                             KC_RALT, MO(1),   KC_LEFT, KC_DOWN, KC_RGHT  
    ),
    [1] = LAYOUT_65_ansi_blocker(
        KC_GRV,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______, KC_INS, 
        _______,  RGB_SPI, RGB_SPD, RGB_M_P, RGB_M_B, RGB_M_R, _______, _______, KC_UP,   _______, KC_PSCR, _______, _______, _______, _______, 
        _______,  BL_STEP, BL_BRTG, RGB_M_SW, RGB_M_SN, RGB_M_K, _______, KC_LEFT, KC_DOWN, KC_RGHT, _______, _______,          _______, _______, 
        _______,  _______, RGB_M_X, RGB_M_G, RGB_M_T, _______, NK_TOGG, _______, _______, _______, _______,          _______, RGB_VAI, _______, 
        _______,  _______, _______,                  RGB_TOG,                           _______, _______, RGB_RMOD, RGB_VAD, RGB_MOD
    )
};

#include QMK_KEYBOARD_H

bool is_alt_tab_active = false;

// clang-format off
enum layers{
  MAC_BASE,
  MAC_FN,
  WIN_BASE,
  WIN_FN,
  WIN_CAPS,
  COD,
  // NUM,
  MSE,
};

// Tap Dance declarations
enum {
    TD_RSFT_CAPS,
    TD_DEL_LINE,
    TD_INSERT,
    TD_YANK,
    TD_SCOL_ENT,
};

enum custom_keycodes {
     DELNWRD = SAFE_RANGE, // delete next word
     NEXTWIN,
     POWERWSL,
     DAND,
     DOR,
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
     switch(keycode) {
          case DELNWRD:
               if(record->event.pressed) {
                    SEND_STRING(SS_DOWN(X_RCTL) SS_TAP(X_RIGHT) SS_TAP(X_BSPC));
               } else {
                    unregister_code(KC_RCTL);
               }
               break;
          case NEXTWIN:
               if(record->event.pressed) {
                    if(!is_alt_tab_active) {
                         is_alt_tab_active = true;
                         register_code(KC_LALT);
                    }
                    register_code(KC_TAB);
               } else {
                    unregister_code(KC_TAB);
               }
               break;
          case DAND:
               if(record->event.pressed) {
                    tap_code16(S(KC_7));
                    tap_code16(S(KC_7));
               }
               break;
          case DOR:
               if(record->event.pressed) {
                    tap_code16(S(KC_BSLS));
                    tap_code16(S(KC_BSLS));
               }
               break;
     }
     return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
     if(is_alt_tab_active && IS_LAYER_OFF_STATE(state, COD)) {
          unregister_code(KC_LALT);
          is_alt_tab_active = false;
     }
     return state;
}

void dance_d(tap_dance_state_t *state, void *user_data) {
     switch(state->count) {
          case 1:
               tap_code(KC_PGDN);
               break;
          case 2:
               SEND_STRING(SS_TAP(X_HOME) SS_LSFT(SS_TAP(X_DOWN)) SS_TAP(X_BSPC));
               break;
          case 3:
               SEND_STRING(SS_TAP(X_HOME) SS_RCTL(SS_LSFT(SS_TAP(X_DOWN))) SS_TAP(X_BSPC));
               break;
     }
}

// Tap Dance definitions
tap_dance_action_t tap_dance_actions[] = {
    [TD_RSFT_CAPS] = ACTION_TAP_DANCE_DOUBLE(KC_RSFT, KC_CAPS),
    [TD_INSERT] = ACTION_TAP_DANCE_DOUBLE(C(KC_V), C(S(KC_V))),
    [TD_YANK] = ACTION_TAP_DANCE_DOUBLE(C(KC_C), C(S(KC_C))),
    [TD_SCOL_ENT] = ACTION_TAP_DANCE_DOUBLE(KC_SEMICOLON, KC_ENT),
    [TD_DEL_LINE] = ACTION_TAP_DANCE_FN(dance_d),
};

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case TD(TD_RSFT_CAPS):
          return 250;
        case KC_LSFT:
          return 265;
        case CTL_T(KC_J):
        case CTL_T(KC_F):
          return TAPPING_TERM - 25;
        case ALT_T(KC_S):
          return TAPPING_TERM + 150;
        case ALT_T(KC_L):
          return TAPPING_TERM + 25;
        case WIN_T(KC_A):
          return TAPPING_TERM + 150;
        case WIN_T(KC_SCLN):
          return TAPPING_TERM + 75;
        case TT(COD):
          return TAPPING_TERM + 50;
        case TD(TD_SCOL_ENT):
          return TAPPING_TERM + 100;
        default:
          return TAPPING_TERM;
    }
}

/*
void leader_start_user(void) {
    // Do something when the leader key is pressed
}
*/

void leader_end_user(void) {
     if (leader_sequence_three_keys(KC_R, KC_E, KC_S)) {
         soft_reset_keyboard();
    } else if (leader_sequence_three_keys(KC_D, KC_F, KC_U)) {
        soft_reset_keyboard(); tap_code16(QK_BOOT);
    } else if (leader_sequence_two_keys(KC_A, KC_S)) {
        SEND_STRING("leader hri");
    } else if (leader_sequence_one_key(KC_L)) {
        tap_code16(KC_F15);
    }
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    uint8_t current_layer = get_highest_layer(layer_state);
    switch (current_layer) {
        case COD:
            rgb_matrix_set_color_all(0x00, 0x00, 0xFF);
            break;
        case WIN_CAPS:
            rgb_matrix_set_color_all(0x00, 0x00, 0x00);  // RGB blank

            int indices[] = {20, 26, 33, 34, 35, 37, 38, 39, 41, 46, 47, 49, 52, 53, 54, 55, 62, 65, 66, 67};
            //               4,  0,  w,  e,  r,  y,  u,  i,  p,  cp, a,  d,  h,  j,  k,  l,  x,  b,  n,  m
            int indices_len = sizeof(indices) / sizeof(indices[0]);

            for(int i = 0; i < indices_len; i++) {
               rgb_matrix_set_color(indices[i], 0xFF, 0x00, 0xFF);
            }
            break;
        /* case NUM:
            rgb_matrix_set_color_all(0x00, 0x00, 0x00);
            int inds[] = {38, 39, 40, 41, 53, 54, 55, 56, 65, 66, 67, 68, 69, 47, 48, 49, 50, 2};
            //            u,  i,  o,  p,  j,  k,  l,  ;,  b,  n,  m,  ,,  .,  a,  s,  d,  f,  f1
            int inds_len = sizeof(inds) / sizeof(inds[0]);

            for(int i = 0; i < inds_len; i++) {
               rgb_matrix_set_color(inds[i], 0xFF, 0xFF, 0x00);
            }
            break; */
        default:
            break;
    }
    if (host_keyboard_led_state().caps_lock) {
        rgb_matrix_set_color_all(0xFF, 0x00, 0x00);  // RGB red --- capslock color
    }
    else if(is_caps_word_on()) {
       rgb_matrix_set_color_all(0xFF, 0xFF, 0x00);
    }
    return false;
}

bool caps_word_press_user(uint16_t keycode) {
    switch (keycode) {
        // Keycodes that continue Caps Word, with shift applied.
        case KC_A ... KC_Z:
        case KC_MINS:
            add_weak_mods(MOD_BIT(KC_LSFT));  // Apply shift to next key.
            return true;

        // Keycodes that continue Caps Word, without shifting.
        case KC_1 ... KC_0:
        case KC_BSPC:
        case KC_DEL:
        case KC_UNDS:
            return true;

        default:
            return false;  // Deactivate Caps Word.
    }
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[MAC_BASE] = LAYOUT_ansi_84(
     KC_ESC,   KC_BRID,  KC_BRIU,  KC_MCTL,  KC_LPAD,  RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  KC_SNAP,  KC_DEL,   RGB_MOD,
     KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,            KC_PGUP,
     KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,            KC_PGDN,
     KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,             KC_HOME,
     KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,  KC_UP,    KC_END,
     KC_LCTL,  KC_LOPTN, KC_LCMMD,                               KC_SPC,                                 KC_RCMMD,MO(MAC_FN),KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT),

[MAC_FN] = LAYOUT_ansi_84(
     KC_TRNS,  KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_TRNS,  KC_TRNS,  RGB_TOG,
     KC_TRNS,  BT_HST1,  BT_HST2,  BT_HST3,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,            KC_TRNS,
     RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,            KC_TRNS,
     KC_TRNS,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,            KC_TRNS,            KC_TRNS,
     KC_TRNS,            KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  BAT_LVL,  NK_TOGG,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,            KC_TRNS,  KC_TRNS,  KC_TRNS,
     KC_TRNS,  KC_TRNS,  KC_TRNS,                                KC_TRNS,                                KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS),

[WIN_FN] = LAYOUT_ansi_84(
     KC_TRNS,  KC_BRID,  KC_BRIU,  KC_TASK,  KC_FILE,  RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  KC_TRNS,  KC_TRNS,  RGB_TOG,
     KC_TRNS,  BT_HST1,  BT_HST2,  BT_HST3,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,            KC_TRNS,
     RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,            KC_TRNS,
     KC_TRNS,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,            KC_TRNS,            KC_TRNS,
     KC_TRNS,            KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  BAT_LVL,  NK_TOGG,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,            KC_TRNS,  KC_TRNS,  KC_TRNS,
     KC_TRNS,  KC_TRNS,  KC_TRNS,                                KC_TRNS,                                KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS),

[WIN_BASE] = LAYOUT_ansi_84(
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
     KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,    KC_PSCR,   KC_DEL, G(KC_W),
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
     KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,    KC_BSPC,             KC_PGUP,
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
     KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,   KC_BSLS,             KC_PGDN,
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
        TT(WIN_CAPS),   WIN_T(KC_A), ALT_T(KC_S),     SFT_T(KC_D), CTL_T(KC_F),     KC_G,     KC_H,     CTL_T(KC_J),     SFT_T(KC_K),   ALT_T(KC_L), WIN_T(KC_SCLN),   KC_QUOT,   KC_ENT,   KC_HOME,
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
     TD(TD_RSFT_CAPS),  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,              KC_LSFT,   KC_UP,    KC_END,
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
     KC_LCTL,  KC_LGUI,  TT(COD),                                KC_SPC,                                 TT(MSE),  KC_RCTL,  MO(WIN_FN),KC_LEFT,   KC_DOWN,  KC_RGHT),
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|),

[WIN_CAPS] = LAYOUT_ansi_84(
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
     KC_TRNS,  KC_BRID,  KC_BRIU,  KC_TASK, KC_FILE,  KC_TRNS,  KC_TRNS,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,   KC_VOLD,  KC_VOLU,   KC_TRNS,   KC_TRNS,  KC_TRNS,
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
     KC_TRNS,  BT_HST1,  BT_HST2,  BT_HST3, KC_END,   KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_HOME,   KC_TRNS,  KC_TRNS,   KC_TRNS,             KC_TRNS,
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|---------|----------|
     KC_TRNS,  KC_TRNS,  C(KC_RGHT),     C(S(KC_RGHT)),    DELNWRD,      KC_TRNS,  TD(TD_YANK),  KC_PGUP,   TG(WIN_CAPS),  KC_TRNS,   TD(TD_INSERT),   KC_TRNS,  KC_TRNS,   KC_TRNS,   KC_TRNS,
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
     KC_TRNS,      C(KC_A),     KC_TRNS,  TD(TD_DEL_LINE), KC_TRNS,  KC_BSPC,  KC_LEFT,  KC_DOWN,  KC_UP,    KC_RIGHT,  KC_TRNS,  KC_TRNS,   KC_TRNS,   KC_TRNS,
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
     KC_TRNS,            KC_TRNS,  C(KC_X), KC_TRNS,  KC_TRNS,    C(KC_LEFT),    C(S(KC_LEFT)),  C(KC_BSPC),  KC_TRNS,   KC_TRNS,  KC_TRNS,   KC_TRNS,   KC_TRNS,  KC_TRNS,
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
     KC_TRNS,  KC_TRNS,  KC_TRNS,                               KC_TRNS,                                KC_TRNS,   KC_TRNS,  KC_TRNS,   KC_TRNS,   KC_TRNS,  KC_TRNS),
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|),

[COD] = LAYOUT_ansi_84(
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
     KC_TRNS,  KC_TRNS,  DT_DOWN,  DT_UP,  A(KC_F4),  DT_PRNT,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  KC_TRNS,  KC_TRNS,  KC_TRNS,
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
     S(KC_GRV),   S(KC_1),     S(KC_2),     S(KC_3),     S(KC_4),     S(KC_5),     S(KC_6),     S(KC_7),     S(KC_8),     S(KC_9),     S(KC_0),    KC_MINS, KC_EQL,    KC_BSPC,             KC_PGUP,
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
     MO(WIN_FN),  S(KC_1),  S(KC_3),  S(KC_GRV),  S(KC_5),  DOR,  DAND,  S(KC_9),  S(KC_0),  S(KC_LBRC),  S(KC_RBRC),  KC_TRNS,  KC_TRNS,  KC_TRNS,            KC_TRNS,
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
     KC_TRNS,  S(KC_EQL), KC_MINS,  S(KC_8),  KC_SLSH,  KC_EQL,  KC_BSPC,  C(G(KC_LEFT)),  C(G(KC_RGHT)),  S(KC_MINS),  TD(TD_SCOL_ENT),  S(KC_QUOT),            KC_TRNS,            KC_TRNS,
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
     KC_TRNS,            KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_F15,  KC_ESC,  KC_F8,  KC_F9,  KC_F10,  KC_F11,  KC_F12,            KC_TRNS,  KC_TRNS,  KC_TRNS,
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
     KC_TRNS,  KC_TRNS,  KC_TRNS,                            A(KC_SPC),                                KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS),
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|),

// [NUM] = LAYOUT_ansi_84(
// //-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
//      KC_TRNS,  TG(NUM),  KC_BRIU,  KC_TASK,  KC_FILE,  KC_TRNS,  KC_TRNS,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  KC_TRNS,  KC_TRNS,  KC_TRNS,
// //-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
//      KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,            KC_TRNS,
// //-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
//      KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_P7,    KC_P8,    KC_P9,  KC_P0,  KC_TRNS,  KC_TRNS,  KC_TRNS,            KC_TRNS,
// //-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
//      KC_TRNS,  KC_PPLS, KC_PMNS,  KC_PAST,  KC_PSLS,  KC_TRNS,  KC_TRNS,   KC_P4,    KC_P5,    KC_P6,  KC_PENT,  KC_TRNS,            KC_TRNS,            KC_TRNS,
// //-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
//      KC_TRNS,            KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_COMM,  KC_P1,    KC_P2,    KC_P3,  KC_TRNS,  KC_TRNS,            KC_TRNS,  KC_TRNS,  KC_TRNS,
// //-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
//      KC_TRNS,  KC_TRNS,  KC_TRNS,                                KC_TRNS,                                KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS),
// //-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|),

[MSE] = LAYOUT_ansi_84(
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
     KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
     KC_TRNS,  C(A(KC_1)), C(A(KC_2)),  C(A(KC_3)),  C(A(KC_4)),  C(A(KC_5)),  C(A(KC_6)),  C(A(KC_7)),  C(A(KC_8)),  C(A(KC_9)),  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,            KC_TRNS,
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
     KC_TRNS,  KC_TRNS,  KC_BTN1,  KC_BTN2,  KC_TRNS,  C(A(KC_T)),  KC_TRNS,  KC_TRNS,  KC_BTN1,  KC_BTN2,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,            KC_TRNS,
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
     KC_TRNS,  KC_WH_L, KC_WH_D,  KC_WH_U,  KC_WH_R,  KC_TRNS,   KC_TRNS,  KC_MS_L,  KC_MS_D,  KC_MS_U,  KC_MS_R,  KC_TRNS,            MO(WIN_FN),            KC_TRNS,
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
          KC_TRNS,       KC_1,  KC_2,  KC_3,  KC_4,  KC_5,  KC_6,  KC_7,  KC_8,  KC_9,  KC_0,       KC_TRNS,       KC_TRNS,  KC_TRNS,
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|
     KC_TRNS,  KC_TRNS,  KC_TRNS,                                A(KC_SPC),                                KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS)
//-|---------|---------|--------|---------|---------|---------|---------|---------|---------|---------|----------|---------|----------|----------|--------|-----------|),
};

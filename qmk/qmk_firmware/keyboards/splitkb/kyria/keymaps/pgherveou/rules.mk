OLED_ENABLE = yes
OLED_DRIVER = SSD1306
ENCODER_ENABLE = yes
RGBLIGHT_ENABLE = yes
TAP_DANCE_ENABLE = no
COMBO_ENABLE = yes
MOUSEKEY_ENABLE = no
RAW_ENABLE = no
WPM_ENABLE = no
CONSOLE_ENABLE = no
DEBOUNCE_TYPE = sym_eager_pk
# https://github.com/qmk/qmk_firmware/issues/5585
NO_USB_STARTUP_CHECK = yes
SRC += encoder.c

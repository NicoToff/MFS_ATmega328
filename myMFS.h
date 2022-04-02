/** Définition des bits utilisés dans le registre C pour les boutons */
#define BTN_1 0b10   // pinA1
#define BTN_2 0b100  // pinA2
#define BTN_3 0b1000 // pinA3

/** Opérations booléennes pour vérification de l'appui sur les boutons */
#define BTN_1_PRESSED (PINC & BTN_1) == 0
#define BTN_2_PRESSED (PINC & BTN_2) == 0
#define BTN_3_PRESSED (PINC & BTN_3) == 0

/** Définition des bits utilisés dans le registre B */
#define LED_1 0b100000 // pin13
#define LED_2 0b10000  // pin12
#define LED_3 0b1000   // pin11
#define LED_4 0b100    // pin10
#define ALL_LEDS_ON   0b0
#define ALL_LEDS_OFF  0b111100

/** Constantes pour les états des LEDs */
#define LED_OFF      0
#define LED_ON       1
#define LED_BLINKING 2

/** Définition des pins utilisées pour le display 7 segments */
#define LATCH 0b10000    // pin4 (Registre C)
#define CLK   0b10000000 // pin7 (Registre C)
#define DATA  0b1        // pin8 (Registre B)
#define LATCH_PIN 4
#define CLK_PIN   7
#define DATA_PIN  8

/** Définition des symboles à afficher */
// 1 = éteint ; 0 = allumé
//       SEG = GFEDCBA
//             |||||||
#define __g 0b10010000 // === 9
#define __N 0b11001000
#define __o 0b10100011
#define __P 0b10001100
#define __r 0b10101111
#define __U 0b11000001
#define __space 0b11111111

// Non utilisés
/*
#define __1 0b11111001 // === I
#define __F 0b10001110
#define __S 0b10010010 // === 5
#define ___ 0b11110111
#define __O 0b11000000 // === 0
*/
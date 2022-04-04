#include <Arduino.h>

/* Définition des bits utilisés dans le registre C pour les boutons */
#define BTN_1 0b10   // pinA1
#define BTN_2 0b100  // pinA2
#define BTN_3 0b1000 // pinA3

/* Opérations booléennes pour vérification de l'appui sur les boutons */
#define BTN_1_PRESSED (PINC & BTN_1) == 0
#define BTN_2_PRESSED (PINC & BTN_2) == 0
#define BTN_3_PRESSED (PINC & BTN_3) == 0

/* Définition des bits utilisés dans le registre B */
#define LED_1 0b100000 // pin13
#define LED_2 0b10000  // pin12
#define LED_3 0b1000   // pin11
#define LED_4 0b100    // pin10
#define ALL_LEDS_ON 0b0
#define ALL_LEDS_OFF 0b111100

/* Constantes pour les états des LEDs */
#define LED_OFF 0
#define LED_ON 1
#define LED_BLINKING 2

/* Définition des pins utilisées pour le display 7 segments */
#define LATCH 0b10000  // pin4 (Registre C)
#define CLK 0b10000000 // pin7 (Registre C)
#define DATA 0b1       // pin8 (Registre B)
#define LATCH_PIN 4
#define CLK_PIN 7
#define DATA_PIN 8

/* Définition de caractères spéciaux */
#define __underscore 0b11110111
#define __space 0b11111111

/** DÉPRÉCIÉ ! Utiliser DIGIT[] ou ALPHA[] pour les caractères.
 * Définition des symboles à afficher
// 1 = éteint ; 0 = allumé
//       SEG = GFEDCBA
//             |||||||
#define __g 0b10010000 // === 9
#define __N 0b11001000
#define __o 0b10100011
#define __P 0b10001100
#define __r 0b10101111
#define __U 0b11000001
#define __1 0b11111001 // === I
#define __F 0b10001110
#define __S 0b10010010 // === 5
#define __O 0b11000000 // === 0
*/

const byte DIGIT[] = {
    // 1 = éteint ; 0 = allumé
    // SEG = GFEDCBA
    0b11000000, // 0
    0b11111001, // 1
    0b10100100, // 2
    0b10110000, // 3
    0b10011001, // 4
    0b10010010, // 5
    0b10000010, // 6
    0b11111000, // 7
    0b10000000, // 8
    0b10010000  // 9
};

/** Pour l'affichage des lettres */
const byte ALPHA[] = {136, 131, 198, 161, 134, 142, 144, 139, 207, 241, 138, 199, 170, 200, 163, 140, 152, 175, 146, 135, 227, 227, 129, 137, 145, 164};

/** Tableau contenant l'accès aux segments */
const byte SEGMENT_SELECT[] = {
    0b11110001, // cadran 1 (gauche)
    0b11110010, // cadran 2
    0b11110100, // cadran 3
    0b11111000  // cadran 4 (droite)
};

class MyMFS
{
public:
    //----------------------------------------------------------------------------------------------------
    // INITIALISATION DES INPUTS/OUTPUTS
    //----------------------------------------------------------------------------------------------------
    /**
     * @brief Setup des inputs/outputs et extinction des LEDs et du display.
     */
    void init()
    {
        /* Registre D (PORTD) :
            pin4 = LATCH <- bit4
            pin7 =  CLK  <- bit7
        */
        DDRD |= (LATCH | CLK); // Mise à 1 des bits pour "OUTPUT"
        /* Registre B (PORTB) :
            pin13 = LED_1 <- bit5
            pin12 = LED_2 <- bit4
            pin11 = LED_3 <- bit3
            pin10 = LED_4 <- bit2
            pin8  = DATA  <- bit0
        */
        DDRB |= (LED_1 | LED_2 | LED_3 | LED_4 | DATA); // Mise à 1 des bits pour "OUTPUT"
        setLEDs(ALL_LEDS_OFF);
        clear();

        /* Registre C (PORTC) === "Groupe 1" :
            pinA1 = BTN_1 <- bit1
            pinA2 = BTN_2 <- bit2
            pinA3 = BTN_3 <- bit3
        */
        DDRC &= (byte) ~(BTN_1 | BTN_2 | BTN_3); // Mise à 0 des bits pour "INPUT"
    };

    //----------------------------------------------------------------------------------------------------
    // GESTION DES LEDS
    //----------------------------------------------------------------------------------------------------
    /**
     * @brief Manipule directement le registre PORTB. Utilisez les constantes pour plus de facilité.
     *
     * @param state Ex. arg. valides : (ALL_LEDS_ON) (~LED_1) (~(LED_2 | LED_4)) ...
     * @return L'état de PORTB, qui devrait être égal au byte passé
     */
    byte setLEDs(byte state)
    {
        return PORTB = state;
    }

    //----------------------------------------------------------------------------------------------------
    // GESTION DES INTERRUPTIONS
    //----------------------------------------------------------------------------------------------------
    /**
     * @brief Set les boutons qui pourront faire des interrupts avec ISR(PCINT1_vect)
     * @param button Ex. arg. valides : (BTN_1) (BTN_2 + BTN_3) (BTN_1 | BTN_3) ...
     */
    void attachButtonInterrupt(byte button)
    {
        PCICR |= 0b10;    // Active la possibilité d'interruptions pour le registre C (PCINT8 -> PCINT13 = GROUPE 1)
        PCMSK1 |= button; // Les pins A1(BTN_1), A2(BTN_2), et A3(BTN_3) peuvent déclencher des interruptions.
    }

    /**
     * @brief Désactive toutes les possibilités d'interruptions avec les boutons du MFS.
     */
    void detachButtonInterrupts()
    {
        PCICR &= (byte) ~(0b10); // Désactive toutes les interruptions
        PCMSK1 &= (byte) ~(BTN_1 | BTN_2 | BTN_3);
    }

    //----------------------------------------------------------------------------------------------------
    // AFFICHAGE SUR LE DISPLAY 7 SEGMENTS
    //----------------------------------------------------------------------------------------------------
    /**
     * @brief Affiche les 4 symboles passés en argument. Utilisez DIGIT[] et ALPHA[] au besoin.
     */
    void write(byte sym1, byte sym2, byte sym3, byte sym4)
    {
        byte toPrint[4] = {sym1, sym2, sym3, sym4};
        for (unsigned int segment = 0; segment < 4; segment++)
        {
            PORTD &= (byte)~LATCH;                                          // Prépare à la réception de données === digitalWrite(LATCH_PIN, LOW)
            shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, toPrint[segment]);        // Envoi du caractère à afficher
            shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, SEGMENT_SELECT[segment]); // Sélection du segment
            PORTD |= LATCH;                                                 // Fin de la lecture === digitalWrite(LATCH_PIN, HIGH);
            delay(5);                                                       // Delai pour affichage ~50 Hz
        }
    }
    /**
     * @brief Affiche un tableau de 4 bytes passés en argument.
     */
    void write(byte toPrint[])
    {
        for (unsigned int segment = 0; segment < 4; segment++)
        {
            PORTD &= (byte)~LATCH;                                          // Prépare à la réception de données === digitalWrite(LATCH_PIN, LOW)
            shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, toPrint[segment]);        // Envoi du caractère à afficher
            shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, SEGMENT_SELECT[segment]); // Sélection du segment
            PORTD |= LATCH;                                                 // Fin de la lecture === digitalWrite(LATCH_PIN, HIGH);
            delay(5);                                                       // Delai pour affichage ~50 Hz
        }
        clear();
    }
    /**
     * @brief Affiche l'entier passé en argument. Un nombre négatif sera converti en positif.
     */
    void write(long value)
    {
        byte segment = 3;
        if (value < 0)
            value *= -1; // Un négatif devient positif
        if (value > 9999)
            value = 9999; // Le max est 9999

        while (value && segment >= 0)
        {
            PORTD &= (byte)~LATCH;                                            // Prépare à la réception de données === digitalWrite(LATCH_PIN, LOW)
            shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, DIGIT[value % 10]);         // %10 pour avoir le dernier chiffre du nombre
            shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, SEGMENT_SELECT[segment--]); // sélection du segment puis incrémentation
            PORTD |= LATCH;                                                   // Fin de la lecture === digitalWrite(LATCH_PIN, HIGH);
            value /= 10;                                                      // On passe au nombre suivant
            delay(5);                                                         // Delai pour affichage ~50 Hz
        }
        clear();
    }

    //----------------------------------------------------------------------------------------------------
    // FONCTIONS POUR LOW POWER // Plus d'infos : https://youtu.be/urLSDi7SD8M
    //----------------------------------------------------------------------------------------------------
    /**
     * @brief Arrête l'Analog Digital Converter
     */
    void disableADC()
    {
        ADCSRA &= 0b01111111;
    }
    /**
     * @brief Active l'Analog Digital Converter
     */
    void enableADC()
    {
        ADCSRA |= 0b10000000;
    }
    /**
     * @brief Passe en "Power-down mode" et arrête l'ADC (Analog-Digital Converter) et BOD (Brown-out Detection)
     */
    void deepSleep()
    {
        // Analog Digital Convert Control and Status (Register A) = ADCSRA
        disableADC();
        SMCR |= 0b100; // Active le "Power-down" sleep mode
        SMCR |= 1;     // Active la possiblité de faire des sleep mode

        // MicroController Unit Control Register (MCUCR)
        MCUCR |= 0b01100000;                         // BOD "Sleep" + "Sleep Enable" à 1
        MCUCR = ((MCUCR & 0b11011111) | 0b01000000); // BODS à 1, BODSE à 0
        __asm__ __volatile__("sleep");               // Exécute l'instruction assembleur "sleep"
    }

    /**
     * @brief Fonction à appeler pour bonne pratique après un deepSleep() ; idéal dans l'ISR(PCINT1_vect)
     */
    void wakeUp()
    {
        SMCR = 0;
        /* ATmega328P [DATASHEET] p38 :
            "...it is recommended to clear [the sleep enable (SE)] immediately after waking up."
        */
    }

private:
    /**
     * @brief Utilisé en interne ; éteint tout le display 7-segment.
     */
    void clear()
    {
        for (unsigned int segment = 0; segment < 4; segment++)
        {
            PORTD &= (byte)~LATCH;                                          // Prépare à la réception de données === digitalWrite(LATCH_PIN, LOW)
            shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, 0xFF);                    // Eteint tout
            shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, SEGMENT_SELECT[segment]); // Sélection du segment
            PORTD |= LATCH;                                                 // Fin de la lecture === digitalWrite(LATCH_PIN, HIGH);
        }
    }
};
//----------------------------------------------------------------------------------------------------
// OBJET A UTILISER POUR ACCÈS AUX MÉTHODES DE LA BIBLIOTHÈQUE
//----------------------------------------------------------------------------------------------------
MyMFS myMFS;
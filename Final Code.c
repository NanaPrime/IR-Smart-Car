#include <xc.h>

// CONFIGURATION BITS
#pragma config FOSC = INTOSC    
#pragma config WDTE = OFF       
#pragma config PWRTE = ON       
#pragma config MCLRE = ON       
#pragma config CP = OFF         
#pragma config BOREN = ON       
#pragma config CLKOUTEN = OFF   
#pragma config IESO = OFF       
#pragma config FCMEN = OFF      

#define _XTAL_FREQ 4000000  // 4 MHz internal oscillator

// === PWM Setup for ENA (RC5) and ENB (RC1) ===
void setup_pwm(void) {
    // ENA (Motor A) on RC5 using CCP1
    TRISC5 = 0;
    CCP1CON = 0b00001100;  // CCP1 in PWM mode
    CCPR1L = 128;          // ~50% duty cycle

    // ENB (Motor B) on RC1 using CCP2
    TRISC1 = 0;  // used as PWM output (not as sensor in this setup)
    CCP2CON = 0b00001100;  // CCP2 in PWM mode
    CCPR2L = 128;

    T2CON = 0b00000101;  // Timer2 on, prescaler 1:4
    PR2 = 255;

    __delay_ms(10);  // allow PWM to stabilize
}

// === Motor control functions ===
void motor_forward(void) {
    LATCbits.LATC0 = 1; LATCbits.LATC2 = 0;  // Motor A forward (IN1=1, IN2=0)
    LATCbits.LATC3 = 1; LATCbits.LATC4 = 0;  // Motor B forward (IN3=1, IN4=0)
}

void motor_left(void) {
    LATCbits.LATC0 = 0; LATCbits.LATC2 = 0;  // Motor A stop
    LATCbits.LATC3 = 1; LATCbits.LATC4 = 0;  // Motor B forward
}

void motor_right(void) {
    LATCbits.LATC0 = 1; LATCbits.LATC2 = 0;  // Motor A forward
    LATCbits.LATC3 = 0; LATCbits.LATC4 = 0;  // Motor B stop
}

void motor_stop(void) {
    LATCbits.LATC0 = 0; LATCbits.LATC2 = 0;
    LATCbits.LATC3 = 0; LATCbits.LATC4 = 0;
}

// === Setup all peripherals ===
void setup(void) {
    // IR Sensors
    TRISC7 = 1;  // Right IR sensor (RC7)
    TRISB0 = 1;  // Left IR sensor on RB0 instead of RC1 due to RC1 used for PWM

    // Motor direction outputs
    TRISC0 = 0; // IN1
    TRISC2 = 0; // IN2
    TRISC3 = 0; // IN3
    TRISC4 = 0; // IN4

    // All digital
    ANSELC = 0x00;
    ANSELB = 0x00;

    OSCCON = 0b01101010;  // 4 MHz internal oscillator
    LATC = 0x00;

    setup_pwm();
}

// === Main program loop ===
void main(void) {
    setup();

    while (1) {
        unsigned char leftIR = PORTBbits.RB0;
        unsigned char rightIR = PORTCbits.RC7;

        if (leftIR == 0 && rightIR == 0) {
            motor_forward();
        }
        else if (leftIR == 1 && rightIR == 0) {
            motor_left();
        }
        else if (leftIR == 0 && rightIR == 1) {
            motor_right();
        }
        else {
            motor_stop();
        }
    }
}

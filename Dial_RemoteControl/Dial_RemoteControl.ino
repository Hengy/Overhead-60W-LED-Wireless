// -------------------------------------------
// Library includes
// -------------------------------------------
#include <RotaryEncoder.h>

#include <nRF24.h>    // nRF24L01+

#include <SPI.h>      // SPI
// -------------------------------------------

// -------------------------------------------
// Definitions
// -------------------------------------------
// Setup a RoraryEncoder for pins A2 and A3:
RotaryEncoder encoder(A2, A3);

int pos = 0;  // encoder postion

// nRF24L01+ pins
#define nRF_CE   8    // chip enable
#define nRF_CSN  9    // chip select
#define nRF_IRQ  10    // irq

uint8_t btnPressed = 0; // btn pressed flag

uint32_t btnPTime;  // time of last btn press
uint32_t btnCTime;  // current time
// -------------------------------------------

// -------------------------------------------
// Arduino setup
// -------------------------------------------
void setup()
{
  Serial.begin(115200);

  // You may have to modify the next 2 lines if using other pins than A2 and A3
  PCICR |= (1 << PCIE1);    // This enables Pin Change Interrupt 1 that covers the Analog input pins or Port C.
  PCMSK1 |= (1 << PCINT10) | (1 << PCINT11);  // This enables the interrupt for pin 2 and 3 of Port C.

  attachInterrupt(0, btnISR, RISING); // encoder middle btn interrupt
}


// -------------------------------------------
// ISR for encoder pin change
// -------------------------------------------
ISR(PCINT1_vect) {
  encoder.tick();
}


// -------------------------------------------
// ISR for encoder switch
// -------------------------------------------
void btnISR() {
  btnPressed = 1;
}


// -------------------------------------------
// Arduino main loop
// -------------------------------------------
void loop()
{
  // encoder rotation
  int newPos = encoder.getPosition();
  if (pos != newPos) {  // position change
    if (newPos > pos) { // positive (CW) change
      
    } else if (newPos < pos) {  // negative (CCW) change
      
    }
    pos = newPos;
  }

  // encoder middle btn
  btnPTime = millis(); // get current
  if (btnPressed && (btnPTime - btnCTime > 250)) {  // if btn was pressed, not within 250ms of last press
    btnPressed = 0;
    btnCTime = millis();
  } else if (btnPressed) {  // ignore btn presses within 250ms of last press (software debounce)
    btnPressed = 0;
  }
}

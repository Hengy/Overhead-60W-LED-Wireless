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
// nRF24L01+ pins
#define nRF_CE   8    // chip enable
#define nRF_CSN  9    // chip select
#define nRF_IRQ  10    // irq
// -------------------------------------------

// -------------------------------------------
// Global variables
// -------------------------------------------
// rotary encoder
RotaryEncoder encoder(A2, A3);
int pos = 0;  // encoder postion

uint8_t btnPressed = 0; // btn pressed flag
uint32_t btnPTime;  // time of last btn press
uint32_t btnCTime;  // current time

// nRF24L01+
uint8_t recvAddr[4] = {0xD5,0xD5,0xD5,0xD5};
uint8_t *buf = (uint8_t*)calloc(4, sizeof(uint8_t));
// -------------------------------------------

// -------------------------------------------
// Arduino setup
// -------------------------------------------
void setup()
{
  Serial.begin(115200);

  // rotary encoder
  PCICR |= (1 << PCIE1);    // Pin Change Interrupt 1 for pins on Port C.
  PCMSK1 |= (1 << PCINT10) | (1 << PCINT11);  // Enables interrupt for pins 2, 3 of Port C.

  attachInterrupt(0, btnISR, RISING); // encoder middle btn interrupt

  // nRF24L01+
  nRF24.init(SPI_CLOCK_DIV4,nRF_CE,nRF_CSN,nRF_IRQ);   // Initialize: SPI clock divider, CE pin, CSN pin, IRQ pin.
                                                       // IRQ pin must be capable of interrupts!
  
  nRF24.setTXMode();                  // Set to TX mode
  
  nRF24.transfer('n',FLUSH_RX,0);
  nRF24.transfer('n',FLUSH_TX,0);
  nRF24.clearInt(0x70);
  
  nRF24.setTXAddr(TX_ADDR_P0, recvAddr, 4);
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

  // nRF24L01+
  // if data sent
  if (nRFSN.TXInt) {
    nRFSN.clearInt(TX_DS); // Clear data sent interrupt
    nRFSN.TXInt = 0;
  }

  // If max retransmits
  if (nRFSN.MAXInt) {
    nRFSN.clearInt(MAX_RT); // Clear max retransmits interrupt
    nRFSN.MAXInt = 0;
  }
}

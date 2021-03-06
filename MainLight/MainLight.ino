// -------------------------------------------
// 60W Overhead LED Light with Wireless Remote
//  - MainLight V1.0
//
//  This is the code for the uController in the
//  light. It receives commands from the "Dial"
//  remote control, and switches the LEDs ON/OFF,
//  or PWMs them.
// -------------------------------------------

// -------------------------------------------
// Library includes
// -------------------------------------------
#include <nRF24.h>    // nRF24L01+

#include <SPI.h>      // SPI

#include <avr/io.h>
// -------------------------------------------

// -------------------------------------------
// Definitions
// -------------------------------------------
// nRF24L01+ pins
#define nRF_CE   18    // chip enable
#define nRF_CSN  19    // chip select
#define nRF_IRQ  20    // irq

// LED
#define LEDpin  10 // LED

// commands
#define ENCUP   B11001100   // encoder increment
#define ENCDWN  B11000011   // encoder deccrement
#define ENCBTN  B11111010   // encoder btn press
// -------------------------------------------

// -------------------------------------------
// Global variables
// -------------------------------------------
// nRF24L01+
uint8_t recvAddr[4] = {0xE4,0xE4,0xE4,0xE4};
uint8_t *buf = (uint8_t*)calloc(4, sizeof(uint8_t));

// LED
uint8_t ledON = 0;
int brightness = 64;
// -------------------------------------------


void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  }
}


// -------------------------------------------
// Arduino setup
// -------------------------------------------
void setup()
{
  //Serial.begin(115200);

  pinMode(LEDpin, OUTPUT);
  analogWrite(LEDpin, 0);
  setPwmFrequency(LEDpin, 8);

  // nRF24L01+
  nRF24.init(SPI_CLOCK_DIV4,nRF_CE,nRF_CSN,nRF_IRQ);   // Initialize: SPI clock divider, CE pin, CSN pin, IRQ pin.
                                                       // IRQ pin must be capable of interrupts!
  
  nRF24.setRXMode();                  // Set to RX mode

  nRF24.setReg(EN_AA, 0);             // Disable auto ack
  
  nRF24.transfer('n',FLUSH_RX,0);
  nRF24.transfer('n',FLUSH_TX,0);
  nRF24.clearInt(0x70);
  
  nRF24.setRXAddr(RX_ADDR_P0, recvAddr, 4);
}

// -------------------------------------------
// Arduino main loop
// -------------------------------------------
void loop()
{
  delay(2);

  if ( nRF24.updateStatus() & 0b01000000 ) {
    
    nRF24.getPayload(1);
    
    buf = nRF24.getBufIn(1);
    
    if (buf[0] == ENCBTN) {
      if (ledON) {
        ledON = 0;
        uint8_t i = brightness;
        uint8_t decr = (brightness/64) + 1;
        while (i > decr) {
          i -= decr;
          analogWrite(LEDpin, i);
          delay(6);
        }
        analogWrite(LEDpin, 0);
      } else {
        ledON = 1;
        for (int i=0; i<=128; i++) {
          analogWrite(LEDpin, i);
          delay(10);
        }
        brightness = 128;
      }
    } else if (buf[0] == ENCUP) {
      brightness += 2;
      if (brightness > 254) {
        brightness = 254;
      }
      analogWrite(LEDpin, brightness);
    } else if (buf[0] == ENCDWN) {
      brightness -= 2;
      if (brightness < 0) {
        brightness = 0;
      }
      analogWrite(LEDpin, brightness);
    }

    buf[0] = 0;
    nRF24.transfer('n',FLUSH_RX,0);
    nRF24.clearInt(0x70);
  }
}

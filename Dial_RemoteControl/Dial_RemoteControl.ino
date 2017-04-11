// Hardware setup:
// Attach a rotary encoder with output pins to A2 and A3.
// The common contact should be attached to ground.

#include <RotaryEncoder.h>

// Setup a RoraryEncoder for pins A2 and A3:
RotaryEncoder encoder(A2, A3);

const uint8_t ledPin = 6;

uint8_t ledON = 0;
int brightness = 1024;
uint16_t inc = 128;

uint8_t btnPressed = 0;

uint32_t sTime;
uint32_t cTime;

void setup()
{
  Serial.begin(115200);
  Serial.println("SimplePollRotator example for the RotaryEncoder library.");

  pinMode(ledPin, OUTPUT);
  analogWrite(ledPin, 0);

  // You may have to modify the next 2 lines if using other pins than A2 and A3
  PCICR |= (1 << PCIE1);    // This enables Pin Change Interrupt 1 that covers the Analog input pins or Port C.
  PCMSK1 |= (1 << PCINT10) | (1 << PCINT11);  // This enables the interrupt for pin 2 and 3 of Port C.

  attachInterrupt(0, btnISR, RISING);
}

// ISR for encoder pin change
ISR(PCINT1_vect) {
  encoder.tick(); // just call tick() to check the state.
}

// ISR for encoder switch
void btnISR() {
  btnPressed = 1;
}

// Read the current position of the encoder and print out when changed.
void loop()
{
  static int pos = 0;

  int newPos = encoder.getPosition();
  if (pos != newPos) {
    if (newPos > pos) {
      brightness += inc;
    } else if (newPos < pos) {
      brightness -= inc;
    }
    if (brightness > 4095) {
      brightness = 4095;
    } else if (brightness < 0) {
      brightness = 0;
    }
    Serial.println(brightness/16);
    pos = newPos;
  }

  sTime = millis();
  if (btnPressed && (sTime - cTime > 250)) {
    if (ledON) {
      ledON = 0;
      int i = brightness / 16;
      uint8_t offSpeed = (i / 64) + 1;
      while (i >= 0) {
        analogWrite(ledPin, i);
        i -= offSpeed;
        delay(3);
      }
    } else {
      ledON = 1;
      brightness = 1024;
      for (int i=0; i < 64; i++) {
        analogWrite(ledPin, i);
        delay(8);
      }
    }
    btnPressed = 0;
    cTime = millis();
  } else if (btnPressed) {
    btnPressed = 0;
  }

  if (ledON) {
    analogWrite(ledPin, brightness/16);
  } else {
    analogWrite(ledPin, 0);
  }
}

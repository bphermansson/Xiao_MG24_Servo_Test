#include "ServoDriver.h"

ServoDriver::ServoDriver(int pin)
  : _pin(pin),
    _pulseWidthUs((MIN_PULSE_WIDTH + MAX_PULSE_WIDTH) / 2),
    _lastPulseTimeUs(0) {}

void ServoDriver::begin() {
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
}

void ServoDriver::writeMicroseconds(int pulseWidthUs) {
  // Clamp pulse width to a safe range (500us to 2500us) to prevent servo damage
  if (pulseWidthUs < 500) {
    pulseWidthUs = 500;
  } else if (pulseWidthUs > 2500) {
    pulseWidthUs = 2500;
  }
  _pulseWidthUs = pulseWidthUs;
}

void ServoDriver::write(int angle) {
  // Clamp angle to 0 - 180 degrees
  if (angle < 0) {
    angle = 0;
  } else if (angle > 180) {
    angle = 180;
  }

  // Map 0-180 degrees to MIN_PULSE_WIDTH (544us) and MAX_PULSE_WIDTH (2400us)
  int pulse = map(angle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  writeMicroseconds(pulse);
}

int ServoDriver::read() const {
  return map(_pulseWidthUs, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH, 0, 180);
}

int ServoDriver::readMicroseconds() const {
  return _pulseWidthUs;
}

void ServoDriver::update() {
  unsigned long currentMicros = micros();

  // Generate a pulse every 20,000 microseconds (50Hz frequency)
  // Unsigned subtraction naturally handles micros() overflow safely
  if (currentMicros - _lastPulseTimeUs >= REFRESH_INTERVAL_US) {
    _lastPulseTimeUs = currentMicros;

    // High phase of the pulse (blocking only for the pulse duration, which is max 2.5ms)
    // We temporarily disable interrupts during the active pulse to prevent timing jitter.
    noInterrupts();
    digitalWrite(_pin, HIGH);
    delayMicroseconds(_pulseWidthUs);
    digitalWrite(_pin, LOW);
    interrupts();
  }
}

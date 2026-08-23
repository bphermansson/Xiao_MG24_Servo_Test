#ifndef SERVO_DRIVER_H
#define SERVO_DRIVER_H

#include <Arduino.h>

class ServoDriver {
public:
  // Constructor: sets the pin to use for the servo.
  ServoDriver(int pin);

  // Initializes the GPIO pin. Should be called in setup().
  void begin();

  // Writes a pulse width directly in microseconds (typically 544 to 2400).
  void writeMicroseconds(int pulseWidthUs);

  // Writes an angle in degrees (0 to 180). Maps to standard servo microseconds.
  void write(int angle);

  // Reads the current angle (0 to 180).
  int read() const;

  // Reads the current pulse width in microseconds.
  int readMicroseconds() const;

  // Must be called continuously in the loop() function to generate pulses.
  void update();

private:
  int _pin;
  int _pulseWidthUs;
  unsigned long _lastPulseTimeUs;

  // Standard pulse width mappings (matching Arduino Servo library defaults)
  static const int MIN_PULSE_WIDTH = 544;  // microseconds, corresponds to 0 degrees
  static const int MAX_PULSE_WIDTH = 2400; // microseconds, corresponds to 180 degrees
  static const unsigned long REFRESH_INTERVAL_US = 20000; // 20ms (50Hz)
};

#endif // SERVO_DRIVER_H

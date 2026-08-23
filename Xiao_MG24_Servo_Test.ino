#include "ServoDriver.h"

// Define the pin connected to the servo signal wire.
// Pin D0 is a standard digital pin on the Seeed Studio XIAO MG24.
// We use preprocessor check for D0 fallback.
#ifdef D0
const int SERVO_PIN = D7;
#else
const int SERVO_PIN = 2; // Default to GPIO 2 (common fallback)
#endif

ServoDriver myServo(SERVO_PIN);

// State variables for servo control modes
enum Mode {
  MODE_MANUAL,
  MODE_SWEEP
};

Mode currentMode = MODE_MANUAL;
int currentAngle = 90;
bool sweepDirectionUp = true;
unsigned long lastSweepUpdateMs = 0;
const unsigned long SWEEP_INTERVAL_MS = 25; // Time in milliseconds between each 1-degree step of the sweep

// Function declarations
void printHelp();
void printStatus();
void handleSerial();
void updateSweep();

void setup() {
  // Initialize Serial interface at 115200 bps
  Serial.begin(115200);

  // Wait up to 3 seconds for Serial Monitor to initialize (optional, prevents missing initial prints)
  unsigned long start = millis();
  while (!Serial && (millis() - start < 3000)) {
    delay(10);
  }

  // Initialize the servo driver (sets pin mode to OUTPUT)
  myServo.begin();
  
  // Set servo to middle position (90 degrees) initially
  myServo.write(currentAngle);

  // Display the help menu and initial status
  printHelp();
  printStatus();
}

void loop() {
  // CRITICAL: Must call update() continuously in the loop.
  // This generates the 50Hz pulses (20ms interval) required for servo control.
  myServo.update();

  // Read and execute serial commands
  handleSerial();

  // Advance the sweep angle if we are in automatic sweep mode
  updateSweep();
}

void printHelp() {
  Serial.println(F("\n============================================="));
  Serial.println(F("     Seeed Studio XIAO MG24 Servo Tester      "));
  Serial.println(F("============================================="));
  Serial.println(F("Commands:"));
  Serial.println(F("  [0-180]      - Go to specific angle in degrees (e.g. '90')"));
  Serial.println(F("  u[500-2500]  - Set pulse width directly in microseconds (e.g. 'u1500')"));
  Serial.println(F("  sweep        - Start automatic continuous sweep (0 -> 180 -> 0)"));
  Serial.println(F("  stop         - Stop automatic sweep and enter manual mode"));
  Serial.println(F("  status       - Display current servo settings"));
  Serial.println(F("  help         - Display this menu"));
  Serial.println(F("=============================================\n"));
}

void printStatus() {
  Serial.println(F("--- Current Status ---"));
  Serial.print(F("Mode:        "));
  Serial.println(currentMode == MODE_SWEEP ? F("AUTOMATIC SWEEP") : F("MANUAL CONTROL"));
  Serial.print(F("Angle:       "));
  Serial.print(myServo.read());
  Serial.println(F(" degrees"));
  Serial.print(F("Pulse Width: "));
  Serial.print(myServo.readMicroseconds());
  Serial.println(F(" us"));
  Serial.print(F("Control Pin: D"));
  Serial.println(SERVO_PIN);
  Serial.println(F("----------------------"));
}

void handleSerial() {
  static String inputBuffer = "";

  while (Serial.available() > 0) {
    char c = (char)Serial.read();

    // Check for line end characters
    if (c == '\n' || c == '\r') {
      inputBuffer.trim();
      if (inputBuffer.length() == 0) {
        inputBuffer = "";
        continue;
      }

      if (inputBuffer.equalsIgnoreCase("sweep")) {
        currentMode = MODE_SWEEP;
        Serial.println(F(">> Entering AUTOMATIC SWEEP mode."));
      }
      else if (inputBuffer.equalsIgnoreCase("stop") || inputBuffer.equalsIgnoreCase("manual")) {
        currentMode = MODE_MANUAL;
        Serial.println(F(">> Stopped sweep. Entering MANUAL mode."));
        printStatus();
      }
      else if (inputBuffer.equalsIgnoreCase("status")) {
        printStatus();
      }
      else if (inputBuffer.equalsIgnoreCase("help")) {
        printHelp();
      }
      else if (inputBuffer.startsWith("u")) {
        // Direct microsecond command (e.g., "u1500")
        currentMode = MODE_MANUAL;
        String pulseStr = inputBuffer.substring(1);
        int pulseVal = pulseStr.toInt();

        if (pulseVal >= 500 && pulseVal <= 2500) {
          myServo.writeMicroseconds(pulseVal);
          Serial.print(F(">> Set pulse width to: "));
          Serial.print(pulseVal);
          Serial.println(F(" us"));
        } else {
          Serial.println(F(">> Error: Pulse width must be between 500 and 2500 microseconds."));
        }
      }
      else {
        // Check if the input is a valid positive integer
        bool isNumeric = true;
        for (unsigned int i = 0; i < inputBuffer.length(); i++) {
          if (!isdigit(inputBuffer[i])) {
            isNumeric = false;
            break;
          }
        }

        if (isNumeric && inputBuffer.length() > 0) {
          int angleVal = inputBuffer.toInt();
          if (angleVal >= 0 && angleVal <= 180) {
            currentMode = MODE_MANUAL;
            currentAngle = angleVal;
            myServo.write(currentAngle);
            Serial.print(F(">> Set angle to: "));
            Serial.print(currentAngle);
            Serial.println(F(" degrees"));
          } else {
            Serial.println(F(">> Error: Angle must be between 0 and 180 degrees."));
          }
        } else {
          Serial.print(F(">> Unknown command: '"));
          Serial.print(inputBuffer);
          Serial.println(F("'. Type 'help' to see list of valid commands."));
        }
      }

      // Clear the buffer for next command
      inputBuffer = "";
    } else {
      // Append character to the buffer if it is printable
      if (isprint(c)) {
        inputBuffer += c;
      }
      // Safety limit to prevent memory bloat/hangs
      if (inputBuffer.length() > 32) {
        inputBuffer = "";
      }
    }
  }
}

void updateSweep() {
  if (currentMode == MODE_SWEEP) {
    unsigned long currentMs = millis();
    if (currentMs - lastSweepUpdateMs >= SWEEP_INTERVAL_MS) {
      lastSweepUpdateMs = currentMs;

      // Increment or decrement the angle based on direction
      if (sweepDirectionUp) {
        currentAngle++;
        if (currentAngle >= 180) {
          currentAngle = 180;
          sweepDirectionUp = false;
        }
      } else {
        currentAngle--;
        if (currentAngle <= 0) {
          currentAngle = 0;
          sweepDirectionUp = true;
        }
      }

      // Write the updated angle to the servo
      myServo.write(currentAngle);
    }
  }
}

    #include <Wire.h>
    #include <SFE_BMP180.h>
    #include <LiquidCrystal_I2C.h>
    // #include "pitches.h"  // Only needed if you're using defined note values elsewhere
 

    SFE_BMP180 bmp180;
    LiquidCrystal_I2C lcd(0x27, 20, 4);  // Adjust the I2C address if needed

    // Pin definitions
    const int greenLED   = 3;   // Safe zone indicator
    const int yellowLED  = 5;   // Warning zone indicator
    const int redLED     = 4;   // Danger zone indicator
    const int speakerPin = 8;   // Speaker for danger siren

    // ------------------------------------------------------------------
    // Function: playDangerSiren()
    // Description: Alternates the red LED with two frequencies to create a
    //              dangerous, siren-like sound for the specified duration.
    // ------------------------------------------------------------------
    void playDangerSiren(unsigned int duration) {
      unsigned long startTime = millis();
      while (millis() - startTime < duration) {
        // Flash red LED and play high tone
        digitalWrite(redLED, HIGH);
        tone(speakerPin, 2200);  // High frequency
        delay(100);
        
        // Turn red LED off and play lower tone
        digitalWrite(redLED, LOW);
        tone(speakerPin, 1800);  // Lower frequency
        delay(100);
      }
      noTone(speakerPin);  // Ensure speaker is off at the end.
    }

    void setup() {
      Serial.begin(9600);
      
      // Initialize LED and speaker pins
      pinMode(greenLED, OUTPUT);
      pinMode(yellowLED, OUTPUT);
      pinMode(redLED, OUTPUT);
      pinMode(speakerPin, OUTPUT);
      
      lcd.begin(20, 4);
      lcd.backlight();

      Wire.begin();  

      // Start the BMP180 sensor
      bool ok = bmp180.begin();
      if (ok) {
        Serial.println("BMP180 initialized successfully!");
        lcd.setCursor(0, 0);
        lcd.print("BMP180 initialized successfully!");
      } else {
        Serial.println("BMP180 initialization failed!");
        lcd.setCursor(0, 0);
        lcd.print("BMP180 initialization failed!");
        while (1);  
      }

    }

    void loop() {
      // Make sure the speaker is silent at the start of each cycle.
      noTone(speakerPin);
      
      char status;
      double temp, pressure;

      
      // ----- Pressure Measurement (Oversampling mode = 3) -----
      status = bmp180.startPressure(3);
      if (status == 0) {
        Serial.println("Pressure measurement error.");
        delay(1000);
        return;
      }
      delay(status);  // Wait for sensor conversion (ms)
      status = bmp180.getPressure(pressure, temp);
      if (status == 0) {
        Serial.println("Pressure read error.");
        delay(1000);
        return;
      }

      
      
      // Print the raw pressure (in Pascals)
      Serial.print("Pressure: ");
      Serial.print(pressure);
      Serial.println(" Pa");
      
      // ----- Zone Determination and Display Updates -----
      // lcd.clear();
      if (pressure >= 750) {
        // SAFE ZONE
        lcd.setCursor(0, 0);
        lcd.print("Pressure: ");
        lcd.print(pressure);
        lcd.print(" hPa ");
        lcd.setCursor(0, 1);
        lcd.print("Safe Zone");
        lcd.setCursor(0, 2);
        lcd.print("Oxygen OK             ");
        
        // Blink green LED
        digitalWrite(greenLED, HIGH);
        digitalWrite(yellowLED, LOW);
        digitalWrite(redLED, LOW);
        noTone(speakerPin);  // Ensure speaker is off
        delay(250);
        digitalWrite(greenLED, LOW);
        delay(250);
        
      } else if ((pressure >= 550) && (pressure < 750)) {
        // WARNING ZONE
        lcd.setCursor(0, 0);
        lcd.print("Pressure: ");
        lcd.print(pressure);
        lcd.print(" hPa ");
        lcd.setCursor(0, 1);
        lcd.print("Warning!");
        lcd.setCursor(0, 2);
        lcd.print("Oxygen dropping");
        
        // Blink yellow LED
        digitalWrite(greenLED, LOW);
        digitalWrite(yellowLED, HIGH);
        digitalWrite(redLED, LOW);
        noTone(speakerPin);  // Ensure speaker is off
        delay(250);
        digitalWrite(yellowLED, LOW);
        delay(250);
        
      } else {
        // DANGER ZONE
        lcd.setCursor(0, 0);
        lcd.print("Pressure: ");
        lcd.print(pressure);
        lcd.print(" hPa ");
        lcd.setCursor(0, 1);
        lcd.print("Danger!");
        lcd.setCursor(0, 2);
        lcd.print("Oxygen CRITICAL          ");
        
        // In danger, play the siren sound and flash red LED.
        digitalWrite(greenLED, LOW);
        digitalWrite(yellowLED, LOW);
        playDangerSiren(1000);  // Play the siren for 2 seconds.
      }
    }

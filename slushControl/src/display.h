#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define LCD_UPDATE_INTERVAL 250

LiquidCrystal_I2C lcd(0x20, 4, 5, 6);

void initDisplay() {
    lcd.begin(20, 4);
    lcd.print("  Slush Machinery");
}

uint32_t lastDisplay = 0;

void loopDisplay(SlushMachine sms[]) {
    if (lastDisplay + LCD_UPDATE_INTERVAL < millis()) {
        lastDisplay = millis();
        for (uint8_t i = 0; i < 2; i++) {
            lcd.setCursor(i * 10, 1);
            lcd.printf("%6.1f \xB2""C", sms[i].getTemperature());
            lcd.setCursor(i * 10, 2);
            lcd.printf("%6u RPM", sms[i].getMotorRevsPerMin());
            lcd.setCursor(i*10, 3);
            lcd.printf(" M:%i C:%i", sms[i].getMotorState(), sms[i].getValveState());
        }
        lcd.setCursor(18, 0);
        lcd.print(digitalRead(PIN_SML_ENCODER));
        lcd.print(digitalRead(PIN_SMR_ENCODER));
    }
}

// TODO: show values on display
// TODO: implement controls (Motor/Valve per side)
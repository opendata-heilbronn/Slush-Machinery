#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x20);

void initDisplay() {
    lcd.begin(20,4);
    lcd.print("Slush Machinery");
}
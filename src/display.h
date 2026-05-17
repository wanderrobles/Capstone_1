#pragma once
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "config.h"

class display_manager {
private:
    LiquidCrystal_I2C _lcd;

public:
    display_manager();
    void begin();
    void showReadings(int moisture, float tempC);
    void showMessage(String line1, String line2 = "");
    void clear();
};

display_manager::display_manager()
    : _lcd(LCD_I2C_ADDR, LCD_COLS, LCD_ROWS) {}

void display_manager::begin() {
    _lcd.init();
    _lcd.backlight();
}

void display_manager::showReadings(int moisture, float tempC) {
    float tempF = tempC * 9.0 / 5.0 + 32.0;
    _lcd.clear();
    _lcd.setCursor(0, 0);
    _lcd.print("Temp: ");
    _lcd.print(tempF, 1);
    _lcd.print(" F");
    _lcd.setCursor(0, 1);
    _lcd.print("Moist: ");
    _lcd.print(moisture);
    _lcd.print("%");
}

void display_manager::showMessage(String line1, String line2) {
    _lcd.clear();
    _lcd.setCursor(0, 0);
    _lcd.print(line1.substring(0, 16));
    if (line2 != "") {
        _lcd.setCursor(0, 1);
        _lcd.print(line2.substring(0, 16));
    }
}

void display_manager::clear() {
    _lcd.clear();
}

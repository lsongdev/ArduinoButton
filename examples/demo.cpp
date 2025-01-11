#include <ArduinoButton.h>

#define BUTTON_PIN 0

KButton button(BUTTON_PIN);

void setup() {
  Serial.begin(115200);
  button.begin();
  button.attachClick([]() {
    Serial.println("Click");
  });
  button.attachDoubleClick([]() {
    Serial.println("DoubleClick");
  });
  button.attachLongPressStart([]() {
    Serial.println("LongPressStart");
  });
  button.attachLongPressStop([]() {
    Serial.println("LongPressStop");
  });
  button.attachDuringLongPress([]() {
    Serial.println("DuringLongPress");
  });
  button.attachMultiClick([](uint8_t count) {
    Serial.print("MultiClick ");
    Serial.println(count);
  });
}

void loop() {
  button.tick();
}
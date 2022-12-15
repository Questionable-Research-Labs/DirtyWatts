#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include <main.h>
// Node ID
// #define NODE_ID 0xa

#define VFD_A 2
#define VFD_B 3
#define VFD_C 4
#define VFD_D 5
#define VFD_E 6
#define VFD_F 7
#define VFD_G 8
#define VFD_DP 9

const int VFD_PINS[] = {VFD_A, VFD_B, VFD_C, VFD_D, VFD_E, VFD_F, VFD_G, VFD_DP};

// Hashmap mapping chars to a list of 7-segment display segments
// A B C D E F G DP
// 0 1 2 3 4 5 6 7
const int VFD_CHARS[][2] = {
  {0x30, 0b11111100}, // 0
  {0x31, 0b01100000}, // 1
  {0x32, 0b11011010}, // 2
  {0x33, 0b11110010}, // 3
  {0x34, 0b01100110}, // 4
  {0x35, 0b10110110}, // 5
  {0x36, 0b10111110}, // 6
  {0x37, 0b11100000}, // 7
  {0x38, 0b11111110}, // 8
  {0x39, 0b11110110}, // 9
  {0x41, 0b11101110}, // A
  {0x42, 0b00111110}, // B
  {0x43, 0b10011100}, // C
  {0x44, 0b01111010}, // D
  {0x45, 0b10011110}, // E
  {0x46, 0b10001110}, // F
  {0x20, 0b00000000}, // Blank
};
// When a char is not found, display three horizontal lines
#define CHAR_NOT_FOUND 0b10010010

void receiveEvent(int howMany) {
  Serial.println("YES");
  while (Wire.available() < 2) {
    delay(10);
  }
  char letterData = Wire.read(); // Contains the letter to display
  const unsigned int controlData = Wire.read(); // Contains weather to display the point or not
  Serial.print("Letter: ");
  Serial.print(letterData, BIN);
  Serial.print((char)letterData);
  Serial.print(" Control: ");
  Serial.println(controlData, BIN);


  // Find the letter in the hashmap
  char outputData = CHAR_NOT_FOUND;
  for (int i = 0; i < sizeof(VFD_CHARS) / sizeof(VFD_CHARS[0]); i++) {
    if (VFD_CHARS[i][0] == letterData) {
      outputData = VFD_CHARS[i][1];
      break;
    }
  }
  outputData = outputData | controlData;

  // Debug output to serial
  Serial.print("Letter: ");
  Serial.println(letterData, BIN);
  Serial.print("As Char: ");
  Serial.println((char)letterData);
  Serial.print(" Control: ");
  Serial.print(controlData, BIN);
  Serial.print(" Output: ");
  Serial.println(outputData, BIN);

  // Write the data to the VFD
  // for (int i = 0; i < 8; i++) {
  //   digitalWrite(VFD_PINS[i], outputData & 0x1);
  //   outputData = outputData >> 1;
  // }

  for (int i = 7; i >= 0; i--) {
    // Serial.print("Data In: ");
    // Serial.print(outputData, BIN);
    // Serial.print(" i: ");
    // Serial.print(i);
    // Serial.print(" writing: ");
    // Serial.print(outputData & 0x1);
    // Serial.print(" to pin: ");
    // Serial.println(VFD_PINS[i]);

    digitalWrite(VFD_PINS[i], outputData & 0x1);
    outputData = outputData >> 1;
  }

}

void setup() {
  Serial.begin(9600);
  #ifdef NODE_ID
    EEPROM.write(0, NODE_ID);
  #endif
  Serial.print("Starting up, node ID: ");
  const int node_id = EEPROM.read(0);
  Serial.println(node_id);
  Wire.begin(node_id);
  Wire.onReceive(receiveEvent); /* register receive event */

  for (int i = 0; i < 8; i++) {
    pinMode(VFD_PINS[i], OUTPUT);
    digitalWrite(VFD_PINS[i], LOW);
  }
  Serial.println("Waiting for data...");
}



void loop() {

  delay(100);
  // char outputData = CHAR_NOT_FOUND;

  // for (int i = 7; i >= 0; i--) {
  //   // Serial.print("Data In: ");
  //   // Serial.print(outputData, BIN);
  //   // Serial.print(" i: ");
  //   // Serial.print(i);
  //   // Serial.print(" writing: ");
  //   // Serial.print(outputData & 0x1);
  //   // Serial.print(" to pin: ");
  //   // Serial.println(VFD_PINS[i]);

  //   digitalWrite(VFD_PINS[i], outputData & 0x1);
  //   outputData = outputData >> 1;
  // }
  // while (true) {delay(10);}

#ifdef DEBUG
  // Run test pattern
  for (int i = 0; i < 8; i++) {
    digitalWrite(VFD_PINS[i], HIGH);
    delay(100);
    digitalWrite(VFD_PINS[i], LOW);
  }
#endif
}
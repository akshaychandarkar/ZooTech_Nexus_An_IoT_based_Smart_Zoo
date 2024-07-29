#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN1 10  // Slave Select Pins for each MFRC522 module
#define RST_PIN1 9  // Reset Pin
#define SS_PIN2 8
#define RST_PIN2 7
#define SS_PIN3 6
#define RST_PIN3 5

#define BUZZER_PIN 4 // Pin for the buzzer
#define WATER_SENSOR_PIN A0 // Pin for the water level sensor

// Water level sensor threshold value
#define WATER_LEVEL_THRESHOLD 500 // Adjust this value based on your sensor and setup

MFRC522 mfrc522_1(SS_PIN1, RST_PIN1); // Create MFRC522 instance for each module
MFRC522 mfrc522_2(SS_PIN2, RST_PIN2);
MFRC522 mfrc522_3(SS_PIN3, RST_PIN3);

// Define allowed card IDs for each RFID reader
byte allowedCards_1[][4] = {{0x33, 0xAC, 0x94, 0xDD}}; // Allowed card for RFID 1
byte deniedCards_1[][4] = {{0xD3, 0x42, 0xBA, 0x0D}, {0xF3, 0xBC, 0x1A, 0xF5}, {0x73, 0x2F, 0xA2, 0xDD}}; // Denied cards for RFID 1

byte allowedCards_2[][4] = {{0xD3, 0x42, 0xBA, 0x0D}}; // Allowed card for RFID 2
byte deniedCards_2[][4] = {{0x33, 0xAC, 0x94, 0xDD}, {0xF3, 0xBC, 0x1A, 0xF5}, {0x73, 0x2F, 0xA2, 0xDD}}; // Denied cards for RFID 2

byte deniedCards_3[][4] = {{0x33, 0xAC, 0x94, 0xDD}, {0xD3, 0x42, 0xBA, 0x0D}, {0xF3, 0xBC, 0x1A, 0xF5}, {0x73, 0x2F, 0xA2, 0xDD}}; // Denied cards for RFID 3

void setup() {
  Serial.begin(9600); // Initialize serial communication
  pinMode(BUZZER_PIN, OUTPUT); // Set buzzer pin as output
  pinMode(WATER_SENSOR_PIN, INPUT); // Set water level sensor pin as input
  
  SPI.begin(); // Init SPI bus
  mfrc522_1.PCD_Init(); // Init MFRC522
  mfrc522_2.PCD_Init();
  mfrc522_3.PCD_Init();

  Serial.println("WELCOME TO ZOO TECH NEXUS");
}

void loop() {
  // Look for new cards
  if (mfrc522_1.PICC_IsNewCardPresent() || mfrc522_2.PICC_IsNewCardPresent() || mfrc522_3.PICC_IsNewCardPresent()) {
    if (mfrc522_1.PICC_ReadCardSerial()) { // Check if the card is present in the first reader
      Serial.print("CAGE 1 : ");
      if (isCardAllowed(mfrc522_1.uid, allowedCards_1)) {
        Serial.println("Leopard Detected. Access granted !");
      } else if (isCardDenied(mfrc522_1.uid, deniedCards_1)) {
        Serial.println("Access denied ! Monkey is from another cage!");
        activateBuzzer();
      } else {
        Serial.println("Access denied ! Animal is from another cage!");
      }
      mfrc522_1.PICC_HaltA(); // Halt PICC
      delay(1000); // Delay to prevent multiple detections
    }
    else if (mfrc522_2.PICC_ReadCardSerial()) { // Check if the card is present in the second reader
      Serial.print("CAGE 2 : ");
      if (isCardAllowed(mfrc522_2.uid, allowedCards_2)) {
        Serial.println("Monkey Detected. Access granted !");
      } else if (isCardDenied(mfrc522_2.uid, deniedCards_2)) {
        Serial.println("Access denied ! Leopard is from another cage!");
        activateBuzzer();
      } else {
        Serial.println("Access denied ! Animal is from another cage!");
      }
      mfrc522_2.PICC_HaltA(); // Halt PICC
      delay(1000); // Delay to prevent multiple detections
    }
    else if (mfrc522_3.PICC_ReadCardSerial()) { // Check if the card is present in the third reader
      Serial.print("ALERT: ");
      if (isCardDenied(mfrc522_3.uid, deniedCards_3)) {
        Serial.println("Animal detected near the fence !");
        activateBuzzer();
      } else {
        Serial.println("Animal detected near the fence !");
      }
      delay(1000); // Delay to prevent multiple detections
    }
  }
  
  // Check water level
  if (digitalRead(WATER_SENSOR_PIN) == LOW) { // If water level is low
    Serial.println("WATER LEVEL ALERT:");
    Serial.println("Tank Water level is low!");
    activateBuzzer();
    delay(1000); // Delay to prevent continuous buzzer activation
  }
}

bool isCardAllowed(MFRC522::Uid uid, byte allowedCards[][4]) {
  for (int i = 0; i < sizeof(allowedCards); i++) {
    bool match = true;
    for (byte j = 0; j < uid.size; j++) {
      if (uid.uidByte[j] != allowedCards[i][j]) {
        match = false;
        break;
      }
    }
    if (match) {
      return true;
    }
  }
  return false;
}

bool isCardDenied(MFRC522::Uid uid, byte deniedCards[][4]) {
  for (int i = 0; i < sizeof(deniedCards); i++) {
    bool match = true;
    for (byte j = 0; j < uid.size; j++) {
      if (uid.uidByte[j] != deniedCards[i][j]) {
        match = false;
        break;
      }
    }
    if (match) {
      return true;
    }
  }
  return false;
}

void activateBuzzer() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(500); // Buzzer ON for 0.5 second
  digitalWrite(BUZZER_PIN, LOW);
}

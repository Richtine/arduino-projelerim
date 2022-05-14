#include <Keypad.h>
#include <MFRC522Extended.h>
#include <MFRC522.h>
#include <Servo.h>
#include <SPI.h>
#include <LcdI2cTUR.h> // türkçe lcd kütüphanemiz
LcdI2cTUR lcd(0x27);

byte heart[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};

byte armsUp[8] = {
  0b00100,
  0b01010,
  0b00100,
  0b10101,
  0b01110,
  0b00100,
  0b00100,
  0b01010
};


#define ROW_NUM    4  // four rows
#define COLUMN_NUM 4  // four columns
#define SERVO_PIN  7 // // the Arduino pin, which connects to the servo motor

#define RST_PIN 8
#define SS_PIN 10

#define LEDA 6
#define LEDB A4
#define LEDC 1

Servo motor;
MFRC522 rfid(SS_PIN, RST_PIN);
byte ID[4] = {195, 25, 19, 9};


Servo servo; // servo motor

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte pin_rows[ROW_NUM] = {A3, A2, A1, A0}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

const String password_1 = "ABC1234"; // change your password here
const String password_2 = "1337";   // change your password here
const String password_3 = "B2016";    // change your password here
String input_password;

int angle = 0; //servo değişkeni
unsigned long lastTime;

void setup() {
  lcd.begin();
  lcd.addChar(6, heart); // 6 cgram bölgesine kalp şeklindeki karakteri atar
  // 7 cgram bölgesine eller havada şeklindeki karakteri atar
  // üzerine yaz işaretini kapatır.
  lcd.setCursor(4, 0);   // ışıkları kapatır
  lcd.setCursor(4, 1);
  lcd.print("! NİCKİNİZ !");
  delay(3000);
  
  lcd.backlightsON();     // ışıkları açar
  delay(500);
  lcd.setCursor(2, 1);
  lcd.write(6);           // cgRam'in 6 hanesine yazılan kalp simgesini ekrana yazar.
  delay(500);
  lcd.setCursor(13, 1);
  lcd.write(7);
  for (char a = 1; a < 3; a++)
  {
    lcd.backlightsOFF();
    delay(200);
    lcd.backlightsON();
    delay(200);
  }
  lcd.goHome();           //ddram adres sayacını sıfırlar, imleci 0,0 a adresler
  lcd.displayClear();
  digitalWrite(LEDB, HIGH);
  Serial.begin(9600);
  input_password.reserve(32); // maximum ŞİFRE HANESİ

  servo.attach(SERVO_PIN);
  servo.write(0); // rotate servo motor to 0°

  SPI.begin();
  rfid.PCD_Init();
  lastTime = millis();
  pinMode(LEDA, OUTPUT);
}

void loop() {


  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    char key = keypad.getKey();
    lcd.setCursor(0, 0);
    lcd.print("ŞİFRE GİRİNİZ:");
    
    if (key) {
      digitalWrite(LEDA, HIGH);
      delay(100);
      digitalWrite(LEDA, LOW);
      
      if (key == '*') {
        input_password = ""; //RESET PASSWORD KEYİ ATAYIN.
      } else if (key == '#') { // ENTER TUŞU
        if (input_password == password_1 || input_password == password_2 || input_password == password_3) {
          lcd.displayClear();
          Serial.println("The password is correct, rotating Servo Motor to 90°");
          angle = 180;
          lcd.setCursor(0, 0);
          lcd.print("KASA AÇILIYOR");
          servo.write(angle);
          lastTime = millis();
          digitalWrite(LEDA, HIGH);
          delay(100);
          digitalWrite(LEDA, LOW);
          delay(2000);
          lcd.displayClear();
          servo.write(0);
          lcd.print("KASA KİLİTLİ");
          lcd.displayClear();
        } else {
          lcd.displayClear();
          digitalWrite(LEDA, HIGH);
          Serial.println("ŞİFRE YANLIŞ");
          lcd.setCursor(0, 0);
          lcd.print("TEKRAR DENEYİN");
          delay(2000);
          digitalWrite(LEDA, LOW);
          lcd.displayClear();
        }
        
        input_password = "";
      } else {
        lcd.setCursor(0, 1);
        input_password += key;
        lcd.print(input_password);
      }
    }
  } else {
    lcd.setCursor(0, 1);
    lcd.print("RFID OKUNUYOR");
    if (rfid.uid.uidByte[0] == ID[0] &&
        rfid.uid.uidByte[1] == ID[1] &&
        rfid.uid.uidByte[2] == ID[2] &&
        rfid.uid.uidByte[3] == ID[3] ) {
      lcd.displayClear();
      lcd.print("KAPI ACILDI");
      Serial.println("Kapi acildi");
      servo.write(180);
      digitalWrite(LEDA, HIGH);
      delay(100);
      digitalWrite(LEDA, LOW);
      delay(3000);
      lcd.displayClear();
      lcd.print("KAPI KILITLI");
      servo.write(0);
      delay(1000);
    } else {
      lcd.displayClear();
      lcd.print("YETKISIZ KART");
      Serial.println("Yetkisiz Kart");
      digitalWrite(LEDA, HIGH);
      delay(1000);
      digitalWrite(LEDA, LOW);
      delay(2000);
      lcd.displayClear();
    }
    rfid.PICC_HaltA();
  }

  if (angle == 90 && (millis() - lastTime) > 5000) { // 5 sANİYE
    angle = 0;
    servo.write(angle);
    Serial.println("Rotating Servo Motor to 0°");
  }
}

#include <SPI.h>                          
#include <MFRC522.h>                     
#include <Servo.h>                        

int RST_PIN = 7;                          //RC522 modülü reset pinini tanımlıyoruz.
int SS_PIN = 10;                          //RC522 modülü chip select pinini tanımlıyoruz.
int servoPin = 8;                         //Servo motor pinini tanımlıyoruz.
//int buzzerPin=6;                        //Buzzer pinini tanımlıyoruz.Buzzeriniz var ise baştaki // ları silin.

Servo motor;                              //Servo motor için değişken oluşturuyoruz.
MFRC522 rfid(SS_PIN, RST_PIN);            //RC522 modülü ayarlarını yapıyoruz.
byte ID[4] = {252,109,23,74};             //Yetkili kart ID'sini tanımlıyoruz. 
void setup() { 
  motor.attach(servoPin);                 //Servo motor pinini motor değişkeni ile ilişkilendiriyor.
  Serial.begin(9600);                     //Seri haberleşmeyi başlatıyor.
  SPI.begin();                            //SPI iletişimini başlatıyor.
  rfid.PCD_Init();                        //RC522 modülünü başlatıyor.
 // pinMode(buzzerPin,OUTPUT);            //Buzzer modulünü başlatıyor.Buzzeriniz var ise baştaki // ları silin.
  motor.write(0);                         //Kap
}

 

void loop() {
  
  if ( ! rfid.PICC_IsNewCardPresent())    //Yeni kartın okunmasını bekliyoruz.
    return;

  if ( ! rfid.PICC_ReadCardSerial())      //Kart okunmadığı zaman bekliyoruz.
    return;


  if (rfid.uid.uidByte[0] == ID[0] &&     //Okunan kart ID'si ile ID değişkenini karşılaştırıyoruz.
    rfid.uid.uidByte[1] == ID[1] && 
    rfid.uid.uidByte[2] == ID[2] && 
    rfid.uid.uidByte[3] == ID[3] ) {
        Serial.println("Kapi acildi");
        ekranaYazdir();
        motor.write(180);                 //Servo motoru 180 dereceye getiriyoruz.
        delay(3000);
        motor.write(0);                   //Servo motoru 0 dereceye getiriyoruz.
        delay(1000);
    }
    else{                                 //Yetkisiz girişte içerideki komutlar çalıştırılır.
      Serial.println("Yetkisiz Kart");
      ekranaYazdir();
      //digitalWrite(buzzerPin,HIGH);     //Ses çıkarması için buzzeri uyarır.
      //delay(1000);                      //Buzzeriniz var ise baştaki // ları silin.
      //digitalWrite(buzzerPin,LOW);      //Buzzeriniz var ise baştaki // ları silin.
      //delay(1000);                      //Buzzeriniz var ise baştaki // ları silin.
    }
  rfid.PICC_HaltA();
}
void ekranaYazdir(){
  Serial.print("ID Numarasi: ");
  for(int sayac = 0; sayac < 4; sayac++){
    Serial.print(rfid.uid.uidByte[sayac]);
    Serial.print(" ");
  }
  Serial.println("");
}

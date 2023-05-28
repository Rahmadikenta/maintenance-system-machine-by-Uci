/* menambah library program
list componenet
-sensor current SCT013 + adding wiring resistor ad capacitor
-sensor waterlevel REES52
- RFID reader RC522
- relay
- arduino uno
- LCD I2C
-buzzer
*/
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "EmonLib.h"

// inialisasi pin RFID, buzzer, dan relay
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);
int pinBuzzer = 8;
int pinRelay = 7;
LiquidCrystal_I2C lcd(0x27, 16, 2);// spesifikasi LCD

EnergyMonitor emon1;
int tegangan = 220.0;

//pin yang digunakan sensor SCT------------------------------
int pin_sct = A0;

// inialisasi variabel System---------------------------------

int System =false;
unsigned long millisPrevious =0;
unsigned long millisSebelum =0;
unsigned long millisSekarang;
unsigned long millisState;
int WaktuStandby;
int waktuMenitStandby;
int detik =1000;
long interval =1000;
int WaktuPause;

int pinLevel= A3;
int valueLevel=0;
int statlevel;
long waktuPakai =0;
long waktuPause =0;
int waktustbLCD;
int waktuPakaiLCD;
int WaktuGantiOli =5; //define according you need(minutes)--------------------
int waktuMenit =0; 

double Irms;
int kondisi;

// ===================== PROGRAM PENGATURAN AWAL ======================= //

void setup()
{

Serial.begin(9600);
Serial.println("CLEARDATA");
Serial.println("LABEL,Time,Arus,Using T,Standby T, Oil Level, Stat level");
//calib - Cur Const= Ratio/BurdenR. 2000/33 = 60
emon1.current(pin_sct, 60);

pinMode(pinBuzzer, OUTPUT);
statlevel =0;

SPI.begin(); // Initiate SPI bus
mfrc522.PCD_Init(); // Initiate MFRC522
// lcd-------------------------------------------------------------
lcd.init();
lcd.backlight();
lcd.setCursor(0,0);
lcd.print("Silahkan tab");
lcd.setCursor(0,1);
lcd.print("kartu");
  
// inialisasi status I/O pin
pinMode(pinBuzzer, OUTPUT);
pinMode(pinRelay, OUTPUT);

// mematkan buzzer dan relay di awal program
digitalWrite(pinRelay, HIGH);

// System awal = 0
 System = 0;

// mematkan buzzer dan relay di awal program
digitalWrite(pinBuzzer, LOW);
}

//----------------------------------------MAIN PROGRAM----------------------------------------
void loop()
{
//start to rean current sensors---------------------------
Irms = emon1.calcIrms(1480);
valueLevel = analogRead(pinLevel);
//program when login System--------------------------------
if (System==1){
 SystemProgram();
 }
 
//Serial.print(System);
//Serial.print("//");
//Serial.println(WaktuStandby);
//delay(1000);

// new RFID check----------------------------------------
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }

  // memilih kartu RFID
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }

  
  //Inialisasi jumlah baris dan kolom lcd
  lcd.begin(16, 2);
  
  // menampilkan ID kartu RFID pada Serial Monitor
  // Serial.print("UID tag :");
  
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    lcd.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    lcd.print(mfrc522.uid.uidByte[i], HEX);

    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  content.toUpperCase();

  

  // * PROGRAM JIKA KARTU RFID SESUAI DENGAN YANG TERDAFTAR * //
  // ** ubah ID katu RFID yang ingin didaftarkan di sini ** //
  if (content.substring(1) == "9B 1B CD 23")
  {


    // PROGRAM "ON" alat

    // jika System = 0
    if (System == 0)
    {
      // relay dinyalakan
      // alat dalam System "ON"
      // buzzer menyala
      digitalWrite(pinRelay, LOW);
      digitalWrite(pinBuzzer, LOW);
      delay(1000);
      
      //Menghapus tulisan lcd
      lcd.clear();
      //Menuliskan kata pada lcd
      lcd.setCursor(0, 0);
      lcd.print("Selamat Datang");
      lcd.setCursor(0, 1);
      lcd.print("Uci");
      delay(1000);
      lcd.clear();

 
      // buzzer dimatikan
      digitalWrite(pinBuzzer, HIGH);
      delay(1000);
      
     // System menjadi = 1
      System = 1;
                  
    }

    // PROGRAM "OFF" alat

    // jika System = 1
    else if (System == 1)
    {
      // relay dimatikan
      // alat dalam System "ON"
      // buzzer menyala
      digitalWrite(pinRelay, HIGH);
      digitalWrite(pinBuzzer, LOW);
      delay(1000);
      
     //Menghapus tulisan lcd
      lcd.clear();
     //Menuliskan kata pada lcd
     lcd.setCursor(0, 0);
     lcd.print("Selamat Tinggal");
     lcd.setCursor(0, 1);
     lcd.print("Uci");
     delay(1000);
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Silahkan tab");
     lcd.setCursor(0,1);
     lcd.print("kartu");
      
      // buzzer dimatikan
      digitalWrite(pinBuzzer, HIGH);
      delay(1000);
      // System menjadi = 0
      System = 0;
      
      //reset all counter value
      WaktuStandby = WaktuStandby *0;
      waktuMenitStandby = waktuMenitStandby *0;
      waktuPakai = waktuPakai*0;
      waktuMenit = waktuMenit*0;
     }
  }


  // PROGRAM JIKA KARTU RFID YANG DIGUNAKAN SALAH ATAU TIDAK TERDAFTAR

  else {
    // buzzer berbunyi pendek 3 kali
    digitalWrite(pinBuzzer, LOW);
    delay(300);
    digitalWrite(pinBuzzer, HIGH);
    delay(300);
    digitalWrite(pinBuzzer, LOW);
    delay(300);
    digitalWrite(pinBuzzer, HIGH);
    delay(300);
    digitalWrite(pinBuzzer, LOW);
    delay(300);
    digitalWrite(pinBuzzer, HIGH);
    delay(300);
   
     //Menghapus tulisan lcd
      lcd.clear();
     //Menuliskan kata pada lcd
     lcd.setCursor(0, 0);
     lcd.print("Akses ditolak");
     lcd.setCursor(0, 1);
     lcd.print("Maaf Siapa Ya?");
     delay(3000);
     lcd.clear();   

  }
}

void SystemProgram(){
 
  //menampilkan di serial monitor
  //Serial.print("Arus yang terbaca : ");
  Serial.print("DATA,TIME,");
  Serial.print(Irms);
  Serial.print(",");
  Serial.print(waktuPakai);
  Serial.print("s");
  Serial.print(waktuMenit);
  Serial.print("m");
  Serial.print(",");
  Serial.print(WaktuStandby);
  Serial.print("s");
  Serial.print(waktuMenitStandby);
  Serial.print("m");
  Serial.print(",");
  Serial.print(valueLevel);
  Serial.print(",");
  Serial.println(statlevel);
  
  
  
  //--------LCD------------------------
  lcd.setCursor(0,0);
  lcd.print("Stby=");
  lcd.setCursor(6,0);
  lcd.print(WaktuStandby);
  lcd.setCursor(8,0);
  lcd.print("/");
  lcd.setCursor(9,0);
  lcd.print(waktuMenitStandby);  

  lcd.setCursor(12,0);
  lcd.print("Oil");
  lcd.setCursor(15,0);
  lcd.print(statlevel);
  
  lcd.setCursor(0,1);
  lcd.print("U Time=");
  lcd.setCursor(7,1);
  lcd.print(waktuPakai);
  lcd.setCursor(9,1);
  lcd.print("/");
  lcd.setCursor(10,1);
  lcd.print(waktuMenit);
  delay(650);
  
  
  
 // waktu standby
  millisSekarang = millis();
    if (millisSekarang - millisSebelum >= detik){
      millisSebelum=millisSekarang;
      WaktuStandby= WaktuStandby+1;
      if (Irms>=0.2){
        waktuPakai= waktuPakai+1;
      }
      if (WaktuStandby== 60){
        waktuMenitStandby= waktuMenitStandby+1;
        WaktuStandby= WaktuStandby*0;
        lcd.setCursor(6,0);
        lcd.print("  ");
      }
       if (waktuPakai== 60){
        waktuMenit= waktuMenit+1;
        waktuPakai= waktuPakai*0;
        lcd.setCursor(7,1);
        lcd.print("  ");
        
      }
    }
  
  /*if (Irms>=0.2){
    kondisi= 1;
  }
  if (Irms<=0.2){
    kondisi = 0;
  }
  if (kondisi==1){
    millisState = millis();
    if (millisState - millisPrevious >= interval){
      millisPrevious=millisState;
      waktuPakai= waktuPakai+1;
      if (waktuPakai== 60){
        waktuMenit= waktuMenit+1;
        waktuPakai= waktuPakai*0;
        lcd.setCursor(7,1);
        lcd.print("  ");
        
      }
    }
  }*/
  if (kondisi==0){
    waktuPause= waktuPakai;
  }
  if (waktuMenit>=WaktuGantiOli){
    waktuMenit=waktuMenit*0;
    // nyalakan buzzer notif oli habis
 
  digitalWrite(pinBuzzer, HIGH);
  delay(1000);
  digitalWrite(pinBuzzer, LOW);
  delay(600);
  digitalWrite(pinBuzzer, HIGH);
  delay(1000);
  digitalWrite(pinBuzzer, LOW);
  delay(600);
  digitalWrite(pinBuzzer, HIGH);
  delay(1000);
  digitalWrite(pinBuzzer, LOW);
  delay(600);
  digitalWrite(pinBuzzer, HIGH);
  delay(1000);
  digitalWrite(pinBuzzer, LOW);
  delay(600);
  digitalWrite(pinBuzzer, HIGH);
  delay(1000);
  digitalWrite(pinBuzzer, LOW);
  delay(600);       
  }
  
 if (valueLevel >1 && valueLevel<=500) {
   digitalWrite(pinBuzzer, HIGH);
  delay(1000);
  digitalWrite(pinBuzzer, LOW);
  delay(600);
  digitalWrite(pinBuzzer, HIGH);
  delay(1000);
  digitalWrite(pinBuzzer, LOW);
  delay(600);
  digitalWrite(pinBuzzer, HIGH);
  delay(1000);
  digitalWrite(pinBuzzer, LOW);
  delay(600);
    statlevel = 0;
  } 
  
  if (valueLevel > 501) {
    statlevel = 1;
  }
}
  

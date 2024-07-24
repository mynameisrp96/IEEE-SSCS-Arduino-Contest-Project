#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//I2C pins declaration
LiquidCrystal_I2C lcd(0x27,16,2);
int dist(void);
#define DHTPIN A0 // Pin connected to DHT22 sensor
#define DHTTYPE DHT22 // DHT 22
#define RELAYPIN 12 // Pin connected to relay control
#define trigPin  9
#define echoPin  10
#define LDRPin A1 // Pin connected to LDR
#define LEDPin 2 // Pin connected to LED

void run_fan(int ,int );
void disp_serial(int,int);
DHT dht(DHTPIN, DHTTYPE);

int temp;
int distance;
int duration;

int count=0;
int LDRValue = 0;
void led_on(int);
void disp_lcd(void);//to display temp and humidity on LCD screen
void lcd_disp_intr2(String,String);


void setup() {
  Serial.begin(9600);
  
  pinMode(RELAYPIN, OUTPUT); // Set relay control pin as output
  digitalWrite(RELAYPIN,HIGH);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  dht.begin();
  pinMode(LEDPin, OUTPUT); // Set LED pin as output
  pinMode(LDRPin, INPUT); // Set LDR pin as input
  lcd.init();//Initializing display
  lcd.clear();//To Power ON the back light
  lcd.backlight();// To Power OFF the back light

}

void loop() {
  temp = dht.readTemperature();
  distance = dist();
  disp_serial(temp,distance);
  led_on(distance);
  run_fan(temp,distance);
  
  disp_lcd();
}



int dist(void){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
 duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
 distance = duration * 0.034 / 2;
  return distance;
}
void run_fan(int temp,int distance){
  if ((temp>28) && (distance<30)) {
        count++;
        digitalWrite(RELAYPIN, LOW);
        if (count<=1) {
          Serial.println("Human detected");
          lcd_disp_intr2("Human","Detected");
          
          delay(50); // For debouncing
          
      }
      }
       else {
        count=0;
        
        digitalWrite(RELAYPIN, HIGH); // Turn off relay (and fan)
        delay(50);
      }
  
}

void disp_serial(int temp,int distance){
  Serial.print("  Distance : ");
  Serial.print(distance);
  Serial.print(" Temp : ");
  Serial.print(temp);
  Serial.print(" C");
  Serial.print("count : ");
  Serial.print(count);
  Serial.println("");
}
void led_on(int distance){
   LDRValue = analogRead(LDRPin); // Read value from LDR
    delay(50); // Wait for half a second
    // Print values to serial monitor
    Serial.print("LDR Value: ");
    Serial.println(LDRValue);
  

    // Check if light intensity is low or button is pressed
    if ((LDRValue > 100) && (distance < 30)) {
      digitalWrite(LEDPin, HIGH); // Turn on LED
    } else {
      digitalWrite(LEDPin, LOW); // Turn off LED
    }
}

void disp_lcd(){


  float humid=dht.readHumidity();
  float tempr=dht.readTemperature();
  lcd.clear();
  String hum="humidity (%) : " + String(humid,2);
  hum.trim();
  hum=hum+"  ";
  int len_hum=hum.length();

  String tmp="Temperature (C) : " + String(tempr,2);
  tmp.trim();
  tmp=tmp+"  ";
  int len_tmp=tmp.length();

  int n=max(len_hum,len_tmp);
  int p=min(len_hum,len_tmp);
  int diff=abs(n-p);

  int pos=0;
  for(pos=0;pos<16;pos++){
    lcd.setCursor(0,0);
    lcd.print(hum.substring(pos,pos+16+diff));
    lcd.print(hum.substring(0,pos-16-diff));//for looping of text

    lcd.setCursor(0,1);
    lcd.print(tmp.substring(pos,pos+16));
    lcd.print(tmp.substring(0,pos-16));//for looping of text
    delay(400);
  }
}

void lcd_disp_intr2(String intr,String intr2){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(intr);
  lcd.setCursor(0, 1);
  lcd.print(intr2);
  delay(2000);
}
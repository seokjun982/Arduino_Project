#include <MsTimer2.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

#define BUZ 10                       //부저 10번
LiquidCrystal_I2C lcd(0x3F, 16, 2);  // LCD 주소 
#define Button 3                      // 버튼 3번          

#define trigPin1 8// distnace1 == bottom 초음파센서
#define echoPin1 9 // distnace1 == botttom

#define trigPin2 12  // distnace2 == top
#define echoPin2 11  // distnace2 == top

volatile byte count = 0;
volatile byte top=0;
volatile byte bottom=0;

byte max_counter = 0; 

unsigned long cur_time = 0;
unsigned long pre_time = 0;

//char *sArr[5]; // 문자열 
SoftwareSerial P_Serial(6,7);
SoftwareSerial bluetooth(2,3);

void setup() {
  P_Serial.begin(9600);
  Serial.begin(9600);  //시리얼 9600
  lcd.init();          // lcd
  lcd.backlight();     // lcd

  //lcd.print(count); // count 출력
  pinMode(Button, INPUT_PULLUP);  // 버튼 3번
  attachInterrupt(digitalPinToInterrupt(Button), COUNT_IN, FALLING); // 버튼 인터럽트로 0으로 초기화
  pinMode(BUZ, OUTPUT);       // 부저 센서 10번
  pinMode(trigPin1, OUTPUT);  // 음파센서 트리거 8번
  pinMode(echoPin1, INPUT);   // 음파센서  에코9번
  pinMode(trigPin2, OUTPUT);  //음파센서 트리거7번
  pinMode(echoPin2, INPUT);   //음파센서 에코6번
}


void loop() {
  Split_data();
  long duration1, distance1;// TOP
  hcsr04_set1(&duration1, &distance1);
  long duration2, distance2;// LOW
  hcsr04_set2(&duration2, &distance2);
  tb_count(distance1, distance2); 
  lcd.print(count);
  clcd(count);
}

void Split_data(){
  char str[100];
  char *sArr[6];
  int index=0 , i = 0;
  char * ptr = NULL;
  if(P_Serial.available()){
    while(P_Serial.available()){
      str[index++] = P_Serial.read();
    }
    str[index]='\0';

    ptr = strtok(str, "#");

  while(ptr != NULL)
  {
    sArr[i++] = ptr;
    ptr = strtok(NULL, "#");
    
  }
  String a = sArr[0];
  String b = sArr[1];
  String c = sArr[2];
  String d = sArr[3];
  String e = sArr[4];
  max_counter = atoi(sArr[5]);
  profile_clcd(a,b,c,d,e);
  }
}
void hcsr04_set1(long *duration, long *distance){ //hcsr04 basic setting
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  *duration = pulseIn(echoPin1, HIGH);
  *distance = (*duration / 2) / 29.1; 
}
void hcsr04_set2(long *duration, long *distance){ //hcsr04 basic setting
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  *duration = pulseIn(echoPin2, HIGH);
  *distance = (*duration / 2) / 29.1; 
}
void tb_count(long distance1, long distance2){
  if (bottom > 0 && distance2 < 20) // top부분 증가조건
  {
    top++;
    tone(10, 262, 50);
    delay(50);
    if (top > 0 && bottom > 0) { // count증가조건
      count++;
      top = 0;
      bottom = 0;

      if (count == max_counter) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Break Time");
        delay(3000);
        count = 0;
      }
    }
  }
  if (distance1 < 20) {
    bottom++;
    tone(10, 294, 50);
    delay(50);
  }
  Serial.print("top:");
  Serial.print(top);
  Serial.print(" / ");
  Serial.print("bottom:");
  Serial.print(bottom);
  Serial.print(" / ");
  Serial.print("count:");
  Serial.println(count);

}
void clcd(int count){
  if(top==0 && bottom==0 && count==0)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Ready");
    delay(2000);
  }
  else{
    lcd.clear();
    lcd.setCursor(0, 0);   // lcd
    lcd.print("COUNT  ");  
    lcd.setCursor(0, 1);
    lcd.print(count);
    delay(500);
  }
  
}
#include <SPI.h> // SPI 라이브러리
#include <MFRC522.h> //RFID 라이브러리
#include <SoftwareSerial.h> // 시리얼 라이브러리

#define RST_PIN 9 // RST9번핀 (리셋 핀)
#define SS_PIN 10 // SS10번핀 (spi통신을 위한 ss핀)
void send_data(byte i);

byte RFID_Check();
void bluetooth_ID();
void print_RFprofile();

byte key[4]={0, 0, 0, 0}; //input_RFID_KEY_VALUE
char *ptr0[6] = {"None", "None", "None", "None", "None","None"}; //Dismatch_ID_INFORMATION
char *ptr1[6]= {"JoYhongHyeon","30","177","73","23","10"}; //Frist_ID_INFORMATION (Name/Age/Height/Weight/Bmi/Max_Count)
char *ptr2[6] , *ptr3[6],*ptr4[6],*ptr5[6],*ptr6[6],*ptr7[6],*ptr8[6],*ptr9[6],*ptr10[6],
*ptr11[6],*ptr12[6],*ptr13[6],*ptr14[6]; //Blank_INFORMATION(MAX=15)
char **profile[15] = {ptr0,ptr1,ptr2,ptr3,ptr4,ptr5,ptr6,ptr7,ptr8,ptr9,ptr10,ptr11,ptr12,ptr13,ptr14}; //INFORMATION_LIST
byte *key0[4] = {0,0,0,0}; //Error_RFID_KEY_VALUE
byte *key1[4] = {141,171,7,50}; //First_RFID_KEY_VALUE
byte *key2[4],*key3[4],*key4[4],*key5[4],*key6[4],*key7[4],*key8[4],*key9[4],*key10[4],
*key11[4],*key12[4],*key13[4],*key14[4]; //Blank_RFID_KEY_VALUE
byte **key_index[15]= {key0,key1,key2,key3,key4,key5,key6,key7,key8,key9,key10,key11,key12,key13,key14}; //RFID_LIST

SoftwareSerial bluetooth(2,3); //블루투스 2,3번
SoftwareSerial P_Serial(6,7); // 시리얼 6,7번
MFRC522 mfrc(SS_PIN, RST_PIN); // 


void setup() {
  // put your setup code here, to run once:
  P_Serial.begin(9600);
  SPI.begin();
  Serial.begin(9600);
  bluetooth.begin(9600);
  mfrc.PCD_Init();  // mfrc모듈 초기화
}

void loop() {
  if(bluetooth.available()){ // bluetooth에서 값이 들어오면 만족(available() : input_data의 크기(byte) 반환)
    bluetooth_ID(); 
  }
  else{
    if(!mfrc.PICC_IsNewCardPresent() || !mfrc.PICC_ReadCardSerial()){ //새로운 카드가 인식되거나 읽힌 RFC_Data가 있는지 확인
    delay(500);
    return;
    }
    RFID_Check();
    Serial.println();
  }
}


void bluetooth_ID(){
  String Name=""; //Read_Data
  char *sArr[5]; //Split_Read_Data
  byte index=0 , i = 0, j=0;
  char * ptr = NULL; //temp
  byte match_bluetooth=0; //match? 1 : 0
  
  while(bluetooth.available())  //existing data in buffer
  {
    char myChar = (char)bluetooth.read();  //문자 하나씩 저장
    Name+=myChar;   //String Name에 문자 하나씩 증
    delay(5);    //수신되는 문자 끊김 방지
  }
  if(!Name.equals(""))//Name 문자열에 data가 존재하면
  {
    Serial.println("input value : "+Name);

    for(i; i<15; i++){  //15명의 이름 비교
      if(Name.equals(profile[i][0])){ //같은 이름이 있는 프로필 발견시
        send_data(i); //2nd Board에 데이터 전송
        match_bluetooth = 1; //일치하면 match_bluetooth = 1로 표현
        break; //일치하면 반복문 종료
      }
      else{
        match_bluetooth =0; //불일치하면 match_bluetooth = 0 유지
      }
    }
    if(match_bluetooth==0){ //불일치하면 불일치 문구 전송
        Serial.println("no match!");
        Serial.println("input your name again");
        bluetooth.println("no match!");
        bluetooth.println("input your name again");
    }
  Name=""; //이름 초기화
  }
}



void RFID_Check(){
  byte match1,match2,match3,match4 = 0; //Inforamtion에 저장되어 있는 RFID_KEY_VALUE
  byte matchkey = 0; //match ? 1:0
  byte length=sizeof(key_index)/sizeof(key_index[0]); //=15

  Serial.print("Card UID:");

  for(byte i=0; i<4; i++){ //store RFID_KEY_VALUE
    Serial.print(mfrc.uid.uidByte[i]);
    Serial.print(" ");
    key[i]=mfrc.uid.uidByte[i];
  }
  Serial.println(); //몇 번째 사용자의 RFID_KEY_VALUE인지 판별
  for (int i = 0; i < length; i++) {
    if (key[0] == key_index[i][0]) match1 = 1;
    else match1 = 0;
    if (key[1] == key_index[i][1]) match2 = 1;
    else match2 = 0;
    if (key[2] == key_index[i][2]) match3 = 1;
    else match3 = 0;
    if (key[3] == key_index[i][3]) match4 = 1;
    else match4 = 0;
    if (match1*match2*match3*match4==1) {
        matchkey = i;
        break;
    }
  }
  send_data(matchkey); //matchkey=0 이면 none
}

void send_data(byte i){
  String sen1 = profile[i][0];
  String sen2 = profile[i][1];
  String sen3 = profile[i][2];
  String sen4 = profile[i][3];
  String sen5 = profile[i][4];
  String sen6 = profile[i][5];
  Serial.print("Name:");
  Serial.print(sen1);
  Serial.print("/age:");
  Serial.print(sen2);
  Serial.print("/height:");
  Serial.print(sen3);
  Serial.print("/weight:");
  Serial.print(sen4);
  Serial.print("/bmi:");
  Serial.print(sen5);
  Serial.print("/max_count:") );
  Serial.println(sen6);
  String send_data = sen1+"#"+sen2+"#"+sen3+"#"+sen4+"#"+sen5+"#"+sen6;
  P_Serial.print(send_data);
}
/*
 * Code By Kelvin(장현서) - kelvin@kakao.com
 * 삼성 휴먼테크논문대상
 * 수면 뇌파를 통한 이상적 맞춤 알람 제작
 * 뇌파 측정용 PLX-DAQ 연동 코드 포함
 * Update:2019.12.08
 */
#include<SoftwareSerial.h> //USB전원 연결로 인한 RX/TX 시리얼 단자 이동
#define BAUDRATE 57600 //57600 BandRate UART 시리얼 통신 설정
#define DEBUGOUTPUT 0 
SoftwareSerial mind(10, 11); //RX/TX 시리얼 단자 배정
int   generatedChecksum = 0; //체크섬 기본값 설정
byte  checksum = 0; //체크섬 값 선언

byte  payloadLength = 0;
byte  payloadData[32] = {0};//총 32byte 수신

void setup() //구동 설정(초기화)
{
  Serial.begin(BAUDRATE);
  mind.begin(BAUDRATE);
  Serial.println("CLEARDATA"); //PLX-DAQ의 데이터 클리어 명령
  Serial.println("LABEL,Time,quality,delta,theta,alpha,beta,gamma,attention,meditation"); //PLX-DAQ 엑셀 라벨 설정
}

byte ReadOneByte() //센싱부로부터 값 수신
{
  int ByteRead;
  while(!mind.available());
  ByteRead = mind.read();
  return ByteRead;
}

//체크섬 데이터 보증 검증 코드
void read_serial_data()
{
  if (ReadOneByte() == 0xAA)
  {
    if (ReadOneByte() == 0xAA)
    {
      payloadLength = ReadOneByte();
      if (payloadLength == 0x20)
      {
        generatedChecksum = 0;
        for (int i = 0; i < payloadLength; i++)
        {
          payloadData[i] = ReadOneByte();
          generatedChecksum += payloadData[i];
        }
        checksum = ReadOneByte();
        generatedChecksum = (~generatedChecksum) & 0xff;
//검증 끝

//PLX-DAQ로 데이터 출력
#if !DEBUGOUTPUT
  float quality = payloadData[1];
  float delta = payloadData[5];
  float theta = payloadData[8];
  float alpha = payloadData[11];
  float beta = payloadData[17];
  float gamma = payloadData[23];
  float attention = payloadData[29];
  float meditation = payloadData[31];
  Serial.print("DATA,TIME,");
  Serial.print(quality);
  Serial.print(",");
  Serial.print(delta);
  Serial.print(",");
  Serial.print(theta);
  Serial.print(",");
  Serial.print(alpha);
  Serial.print(",");
  Serial.print(beta);
  Serial.print(",");
  Serial.print(gamma);
  Serial.print(",");
  Serial.print(attention);
  Serial.print(",");
  Serial.println(meditation);
#endif
        }
      }
    }
}

//반복부
void loop()
{
  read_serial_data();
}

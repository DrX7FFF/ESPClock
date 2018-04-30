// max7219 registers
#define MAX7219_REG_NOOP         0x0
#define MAX7219_REG_DIGIT0       0x1
#define MAX7219_REG_DIGIT1       0x2
#define MAX7219_REG_DIGIT2       0x3
#define MAX7219_REG_DIGIT3       0x4
#define MAX7219_REG_DIGIT4       0x5
#define MAX7219_REG_DIGIT5       0x6
#define MAX7219_REG_DIGIT6       0x7
#define MAX7219_REG_DIGIT7       0x8
#define MAX7219_REG_DECODEMODE   0x9
#define MAX7219_REG_INTENSITY    0xA
#define MAX7219_REG_SCANLIMIT    0xB
#define MAX7219_REG_SHUTDOWN     0xC
#define MAX7219_REG_DISPLAYTEST  0xF

#define MOSI_PIN 2
#define CLK_PIN 3
#define CS_PIN 0

#define INTENSITY 7

#include <TimeLib.h>

byte screen[16];

byte animPacmanIndex;
byte animPacmanPos;
const byte animPacman[][6] = {
  {  0, 17, 27, 31, 14, 0},
  { 10, 27, 31, 31, 14, 0},
  { 14, 31, 31, 31, 14, 0},
  { 10, 27, 31, 31, 14, 0}
};

const byte number[][3] = { 
  { 14, 17, 14}, //0
  {  0, 31,  0}, //1
  { 19, 21,  9}, //2
  { 17, 21, 10}, //3
  { 28,  4, 15}, //4
  { 29, 21, 18}, //5
  { 15, 21, 23}, //6
  { 16, 23, 24}, //7
  { 27, 21, 27}, //8
  { 29, 21, 30}  //9
};

byte animwifiIndex;
const byte animwifiData[][8] = {
  {3,3,0,0,0,0,0,0},
  {3,11,8,6,0,0,0,0},
  {3,43,40,38,16,14,0,0},
  {3,171,168,166,144,78,32,30}
};

void sendByte (const byte reg, const byte data) {
    for(int i = 0; i<8; i++){
      digitalWrite(MOSI_PIN,(reg<<i)&0x80);
      digitalWrite(CLK_PIN,HIGH);
      digitalWrite(CLK_PIN,LOW);
    }
    for(int i = 0; i<8; i++){
      digitalWrite(MOSI_PIN,(data<<i)&0x80);
      digitalWrite(CLK_PIN,HIGH);
      digitalWrite(CLK_PIN,LOW);
    }
}
void sendByte2 (const byte reg, const byte data1, const byte data2) {
    digitalWrite(CS_PIN,LOW);
    sendByte(reg, data2);
    sendByte(reg, data1);
    digitalWrite (CS_PIN, HIGH);
}

void sendScreen() {
    for(int i=0; i<8; i++)
      sendByte2(i+1,screen[i], screen[i+8]);
}

void animWifi(){
  animwifiIndex=(animwifiIndex+1)%4;
  for(byte i=0;i<8;i++){
    sendByte2(8-i,animwifiData[animwifiIndex][i],0);
  }
}

void MatrixShow(){
  int pacmanPos;
  time_t t = now();
  byte min_unite = minute(t) % 10;
  byte min_dixaine = (minute(t)/10)%10;
  byte hour_unite = hour(t) % 10;
  byte hour_dixaine = (hour(t)/10)%10;
  byte sec = second(t);
  
  screen[15] = number[hour_dixaine][0];
  screen[14] = number[hour_dixaine][1];
  screen[13] = number[hour_dixaine][2];
  screen[12] = 0;
  screen[11] = number[hour_unite][0];
  screen[10] = number[hour_unite][1];
  screen[ 9] = number[hour_unite][2];
  screen[ 8] = 0;
  screen[ 7] = number[min_dixaine][0];
  screen[ 6] = number[min_dixaine][1];
  screen[ 5] = number[min_dixaine][2];
  screen[ 4] = 0;
  screen[ 3] = number[min_unite][0];
  screen[ 2] = number[min_unite][1];
  screen[ 1] = number[min_unite][2];
  screen[ 0] = 0;

  if (sec == 57 && myTicks == 5)
    animPacmanPos = 0;

  for (byte i=0; i<6; i++){
    pacmanPos = animPacmanPos - i;
    if (pacmanPos>=0 && pacmanPos<=16)
      screen[pacmanPos] = animPacman[animPacmanIndex][i];
  }

  animPacmanIndex=(animPacmanIndex+1)%4;
  if (animPacmanIndex==0)
    animPacmanPos++;
  
  screen[15] += 64*(sec>15 && sec<46)+128*(sec>0 && sec<31);
  screen[14] += 64*(sec>16 && sec<47)+128*(sec>1 && sec<32);
  screen[13] += 64*(sec>17 && sec<48)+128*(sec>2 && sec<33);
  screen[12] += 64*(sec>18 && sec<49)+128*(sec>3 && sec<34);
  screen[11] += 64*(sec>19 && sec<50)+128*(sec>4 && sec<35);
  screen[10] += 64*(sec>20 && sec<51)+128*(sec>5 && sec<36);
  screen[ 9] += 64*(sec>21 && sec<52)+128*(sec>6 && sec<37);
  screen[ 8] += 64*(sec>22 && sec<53)+128*(sec>7 && sec<38);
  screen[ 7] += 64*(sec>23 && sec<54)+128*(sec> 8 && sec<39);
  screen[ 6] += 64*(sec>24 && sec<55)+128*(sec> 9 && sec<40);
  screen[ 5] += 64*(sec>25 && sec<56)+128*(sec>10 && sec<41);
  screen[ 4] += 64*(sec>26 && sec<57)+128*(sec>11 && sec<42);
  screen[ 3] += 64*(sec>27 && sec<58)+128*(sec>12 && sec<43);
  screen[ 2] += 64*(sec>28 && sec<59)+128*(sec>13 && sec<44);
  screen[ 1] += 64*(sec>29 && sec<60)+128*(sec>14 && sec<45);
  //screen[ 0] += 0;

  sendScreen();
}

void MatrixInit(){
  pinMode(MOSI_PIN, OUTPUT);
  pinMode(CLK_PIN, OUTPUT);
  pinMode(CS_PIN, OUTPUT);
  sendByte2 (MAX7219_REG_SCANLIMIT, 7, 7);   // show all 8 digits
  sendByte2 (MAX7219_REG_DECODEMODE, 0, 0);  // using an led matrix (not digits)
  sendByte2 (MAX7219_REG_DISPLAYTEST, 0, 0); // no display test
  sendByte2 (MAX7219_REG_INTENSITY, INTENSITY, INTENSITY);   // character intensity: range: 0 to 15
  sendByte2 (MAX7219_REG_SHUTDOWN, 1, 1);    // not in shutdown mode (ie. start it up)
}
/*
void MatrixIntensity(byte intensity){
  sendByte2 (MAX7219_REG_INTENSITY, intensity & 0xF, intensity & 0xF);   // character intensity: range: 0 to 15  
}
*/

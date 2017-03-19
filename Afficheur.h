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

byte hour;
byte minute;
byte second;
byte myTicks;

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
  byte min_unite = minute % 10;
  byte min_dixaine = (minute/10)%10;
  byte hour_unite = hour % 10;
  byte hour_dixaine = (hour/10)%10;
  
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

  if (second == 57 && myTicks == 5)
    animPacmanPos = 0;

  for (byte i=0; i<6; i++){
    pacmanPos = animPacmanPos - i;
    if (pacmanPos>=0 && pacmanPos<=16)
      screen[pacmanPos] = animPacman[animPacmanIndex][i];
  }

  animPacmanIndex=(animPacmanIndex+1)%4;
  if (animPacmanIndex==0)
    animPacmanPos++;
  
  screen[15] += 64*(second>15 && second<46)+128*(second>0 && second<31);
  screen[14] += 64*(second>16 && second<47)+128*(second>1 && second<32);
  screen[13] += 64*(second>17 && second<48)+128*(second>2 && second<33);
  screen[12] += 64*(second>18 && second<49)+128*(second>3 && second<34);
  screen[11] += 64*(second>19 && second<50)+128*(second>4 && second<35);
  screen[10] += 64*(second>20 && second<51)+128*(second>5 && second<36);
  screen[ 9] += 64*(second>21 && second<52)+128*(second>6 && second<37);
  screen[ 8] += 64*(second>22 && second<53)+128*(second>7 && second<38);
  screen[ 7] += 64*(second>23 && second<54)+128*(second> 8 && second<39);
  screen[ 6] += 64*(second>24 && second<55)+128*(second> 9 && second<40);
  screen[ 5] += 64*(second>25 && second<56)+128*(second>10 && second<41);
  screen[ 4] += 64*(second>26 && second<57)+128*(second>11 && second<42);
  screen[ 3] += 64*(second>27 && second<58)+128*(second>12 && second<43);
  screen[ 2] += 64*(second>28 && second<59)+128*(second>13 && second<44);
  screen[ 1] += 64*(second>29 && second<60)+128*(second>14 && second<45);
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

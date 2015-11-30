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


byte hour;
byte minute;

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
    digitalWrite(CS_PIN,LOW);
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
    digitalWrite (CS_PIN, HIGH);
}

void animWifi(){
  animwifiIndex=(animwifiIndex+1)%4;
  for(byte i=0;i<8;i++){
    sendByte(8-i,animwifiData[animwifiIndex][i]);
  }
}

void MatrixShow(){
  byte unite = minute % 10;
  byte dixaine = (minute/10)%10;
  byte hour_12 = (hour+11)%12+1;
  sendByte(8,number[dixaine][0] + 64*(hour_12>6)+128*(hour_12>0));
  sendByte(7,number[dixaine][1] + 64*(hour_12>7)+128*(hour_12>1));
  sendByte(6,number[dixaine][2] + 64*(hour_12>8)+128*(hour_12>2));
  sendByte(5,0);
  sendByte(4,number[unite][0] + 64*(hour_12>9)+128*(hour_12>3));
  sendByte(3,number[unite][1] + 64*(hour_12>10)+128*(hour_12>4));
  sendByte(2,number[unite][2] + 64*(hour_12>11)+128*(hour_12>5));
  sendByte(1,0);
}

void MatrixInit(){
  pinMode(MOSI_PIN, OUTPUT);
  pinMode(CLK_PIN, OUTPUT);
  pinMode(CS_PIN, OUTPUT);
  sendByte (MAX7219_REG_SCANLIMIT, 7);   // show all 8 digits
  sendByte (MAX7219_REG_DECODEMODE, 0);  // using an led matrix (not digits)
  sendByte (MAX7219_REG_DISPLAYTEST, 0); // no display test
  sendByte (MAX7219_REG_INTENSITY, 4);   // character intensity: range: 0 to 15
  sendByte (MAX7219_REG_SHUTDOWN, 1);    // not in shutdown mode (ie. start it up)
}

void MatrixIntensity(byte intensity){
  sendByte (MAX7219_REG_INTENSITY, intensity & 0xF);   // character intensity: range: 0 to 15  
}


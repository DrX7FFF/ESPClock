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

#define CS_PIN 15

byte hour;
byte minute;

const byte number[][3] = { 
  {112,136,112}, //0
  {  0,248,  0}, //1
  {200,168,144}, //2
  {168,168,248}, //3
  { 56, 32,240}, //4
  {184,168, 72}, //5
  {240,168,232}, //6
  {  8,232, 24}, //7
  {216,168,216}, //8
  {184,168,120}  //9     
};

byte animwifiIndex;
const byte animwifiData[][8] = {
  {192,192,0,0,0,0,0,0},
  {192,208,16,96,0,0,0,0},
  {192,212,20,100,8,112,0,0},
  {192,213,21,101,9,114,4,120}
};

void sendByte (const byte reg, const byte data) {
    digitalWrite(CS_PIN,LOW);
    SPI.transfer (reg);
    SPI.transfer (data);
    digitalWrite (CS_PIN, HIGH);   
}

void animWifi(){
  animwifiIndex=(animwifiIndex+1)%4;
  for(byte i=0;i<8;i++){
    sendByte(i+1,animwifiData[animwifiIndex][i]);
  }
}

void MatrixShow(){
  byte unite = minute % 10;
  byte dixaine = (minute/10)%10;
  byte hour_12 = (hour+11)%12+1;
  sendByte(1,number[dixaine][0] + 2*(hour_12>6)+(hour_12>0));
  sendByte(2,number[dixaine][1] + 2*(hour_12>7)+(hour_12>1));
  sendByte(3,number[dixaine][2] + 2*(hour_12>8)+(hour_12>2));
  sendByte(4,0);
  sendByte(5,number[unite][0] + 2*(hour_12>9)+(hour_12>3));
  sendByte(6,number[unite][1] + 2*(hour_12>10)+(hour_12>4));
  sendByte(7,number[unite][2] + 2*(hour_12>11)+(hour_12>5));
  sendByte(8,0);
}

void MatrixInit(){
  pinMode(CS_PIN, OUTPUT);
  SPI.begin ();
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV128);
  sendByte (MAX7219_REG_SCANLIMIT, 7);   // show all 8 digits
  sendByte (MAX7219_REG_DECODEMODE, 0);  // using an led matrix (not digits)
  sendByte (MAX7219_REG_DISPLAYTEST, 0); // no display test
  sendByte (MAX7219_REG_INTENSITY, 4);   // character intensity: range: 0 to 15
  sendByte (MAX7219_REG_SHUTDOWN, 1);    // not in shutdown mode (ie. start it up)
}

void MatrixIntensity(byte intensity){
  sendByte (MAX7219_REG_INTENSITY, intensity & 0xF);   // character intensity: range: 0 to 15  
}


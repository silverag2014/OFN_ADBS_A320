/*  ADBS_A320  */


#include <Wire.h>
#include <ADBS_A320.h>

const int ADDR = 0x6b;

int Delta_X_Val;
int Delta_Y_Val;
int Val;

//----------------A320 Sensor pin define----------------
#define NRSTPIN 3       // PIN NRST connect to arduino pin 3
#define MotionPIN 4     // PIN Motion connect to arduino pin 4
#define OrientPIN 5     // PIN Orient connect to arduino pin 5
#define SHTDWNPIN 8     // PIN SHTDWN connect to arduino pin 8
	                // SDA (data line) is on analog input pin A4, and SCL (clock line) is on analog input pin A5
unsigned char I2C_Status;

void setup()
{
      Serial.begin(9600);
    
    //--------------PIN mode ------------------ 
    pinMode(SHTDWNPIN, OUTPUT);
    pinMode(MotionPIN, INPUT);
    pinMode(OrientPIN, OUTPUT);
    pinMode(NRSTPIN, OUTPUT); 
    
    Serial.println("There is A320");
    Init_A320();
}

 
void loop()
{
  if(digitalRead(MotionPIN) == LOW)
  {
     read_register(ADDR, Motion); 
     read_register(ADDR, Delta_X); 
     Delta_X_Val = Val;
     read_register(ADDR, Delta_Y);
     Delta_Y_Val = Val;
      if(Delta_X_Val & 0x80 )
      {
        Serial.println("move to right"); 
      }
      else
      {
        Serial.println("move to left");
      }
        if(Delta_Y_Val & 0x80 )
      {
        Serial.println("move to down"); 
      }
      else
      {
        Serial.println("move to up");
      }
       Serial.print(Delta_X_Val,HEX);
       Serial.print("   ");
       Serial.println(Delta_Y_Val,HEX);
     delay(500);  
  }
  else
  {
    Serial.println("  "); 
    delay(500);    
  }  
}



void Init_A320(void)
{ 
  //--------------Power Up----------------------
    digitalWrite(SHTDWNPIN, LOW);
    digitalWrite(OrientPIN, LOW); 
    //delay(100);
    digitalWrite(NRSTPIN, LOW);
    delayMicroseconds(2);
    digitalWrite(NRSTPIN, HIGH);
    //delay(10);
    digitalWrite(OrientPIN, HIGH);
    // Serial.println("Hello A320-1");
    
        //------------Enable I2C------------
    Wire.begin();// join i2c bus (address optional for master)
    delay(100);
    Wire.beginTransmission(ADDR);
    Wire.endTransmission();
    delayMicroseconds(2);
    
    read_register(ADDR, Product_ID);
    set_register(ADDR, Soft_RESET, 0x56);
    delayMicroseconds(10);
    read_register(ADDR, Soft_RESET);
    
    set_register(ADDR, OFN_Engine, 0xe4); 
    //set_register(ADDR, OFN_Engine, 0xf6); 
    set_register(ADDR, OFN_Resolution, 0x12);      //500cpi
    read_register(ADDR, OFN_Resolution);   
   
    set_register(ADDR, OFN_Speed_Control, 0x0e);    //Speed of switching 12ms, low cpi 250
    //set_register(ADDR, OFN_Speed_Control, 0xce);
    set_register(ADDR, OFN_Speed_ST12, 0x08);
    set_register(ADDR, OFN_Speed_ST21, 0x06); 
    set_register(ADDR, OFN_Speed_ST23, 0x40);      
    set_register(ADDR, OFN_Speed_ST32, 0x08); 
    set_register(ADDR, OFN_Speed_ST34, 0x48);
    set_register(ADDR, OFN_Speed_ST43, 0x0a); 
    set_register(ADDR, OFN_Speed_ST45, 0x50);      //500cpi
    set_register(ADDR, OFN_Speed_ST54, 0x48);    //Speed of switching 12ms, low cpi 250
    //set_register(ADDR, OFN_AD_CTRL, 0xc4);
    set_register(ADDR, OFN_AD_ATH_HIGH, 0x34); 
    set_register(ADDR, OFN_AD_DTH_HIGH, 0x3c);      //500cpi
    set_register(ADDR, OFN_AD_ATH_LOW, 0x18); 
    set_register(ADDR, OFN_AD_DTH_LOW, 0x20);
    set_register(ADDR, OFN_FPD_CTRL, 0x50);
    set_register(ADDR, OFN_Quantize_CTRL, 0x99);
    set_register(ADDR, OFN_XYQ_THRESH, 0x02);
    
    read_register(ADDR, Motion); 
    read_register(ADDR, Delta_X); 
    read_register(ADDR, Delta_Y); 
}

void set_register(int address, unsigned char reg, unsigned char val) {
  while(I2C_Status == 1);
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
  
  while(I2C_Status == 1);
  delayMicroseconds(2);
  
}

void read_register(int address, unsigned char reg) {
  while(I2C_Status == 1);
  Wire.beginTransmission(address); 
  Wire.write(reg);
  Wire.endTransmission();

  Wire.requestFrom(address, 1);
  while (Wire.available())   // slave may send less than requested
  {
    Val = Wire.read(); // receive a byte as character
    Serial.println(Val,HEX);         // print the character
  } 
  delay(10);
  while(I2C_Status == 1);
  delayMicroseconds(2);
}

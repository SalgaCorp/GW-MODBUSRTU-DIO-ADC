
/*

This code allows validating the operation of the HW, using reading of digital inputs, activation of outputs, reading analog inputs 0 to 3V, reading Modbus RTU data with "ModbusMaster" library.
This code is the basis that the VERDATA client uses to monitor sensors in Modbus RTU.
Created by Claudio PURCELL.
Thanks to :

Author:: Doc Walker (4-20ma@wvfans.net)
Author:: Ag Primatic (agprimatic@gmail.com)
Author:: Marius Kintel (marius@kintel.net)
 */



#include <ModbusMaster.h> //https://github.com/4-20ma/ModbusMaster
#include <SoftwareSerial.h>

#include <Adafruit_ADS1X15.h>

//Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
Adafruit_ADS1015 ads;     /* Use this for the 12-bit version */
const uint8_t i2c_sda = 21;
const uint8_t i2c_scl = 22;

/*!
  We're using a MAX485-compatible RS485 Transceiver.
  Rx/Tx is hooked up to the hardware serial port at 'Serial'.
  The Data Enable (DE) and Receiver Enable (RE) pins are hooked up as follows:
*/

#define MAX485_RE_NEG  23 //D4 RS485 has a enable/disable pin to transmit or receive data. Arduino Digital Pin 23 = Rx/Tx 'Enable'; High to Transmit, Low to Receive

//SALIDAS control  
//#define RE_DE               23  //GPIO23 IO23       RE_DE
//#define LED_PIN             12  //GPIO12 IO12       LED_EMB


//ENTRADAS DIGITALES
#define DIN1                  32  //GPIO32 IO32       DIN1
#define DIN2                  33  //GPIO33 IO33       DIN2
#define DIN3                  15  //GPIO15 IO15       DIN3
#define DIN4                  00  //GPIO00 IO00       DIN4
#define DIN5                  34  //GPIO34 IO34       DIN5  
#define DIN6                  39  //GPIO39 SENSOR_VN  DIN6   
  
    
//SALIDAS DIGITALES
#define DOUT1               5   //GPIO05 IO05       DOUT1
#define DOUT2               18  //GPIO18 IO18       DOUT2  
#define DOUT3               19  //GPIO19 IO19       DOUT3   
  
int Slave_ID  =     1;
int registro;
int cont=0;
long lastMillis = 0;  
uint8_t result;


SoftwareSerial RS485(14,13); // RX, TX
ModbusMaster node;

void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, HIGH); //Switch to transmit data
}

void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, LOW); //Switch to receive data
}

void setup()
{
    pinMode(MAX485_RE_NEG, OUTPUT);
    // Init in receive mode
    digitalWrite(MAX485_RE_NEG, LOW);
    // // DIGITAL OUTPUTS: control the relays or Open Collector transistors      
    pinMode(DOUT1, OUTPUT);
    pinMode(DOUT2, OUTPUT);
    pinMode(DOUT3, OUTPUT);
    digitalWrite(DOUT1, LOW);
    digitalWrite(DOUT2, LOW);
    digitalWrite(DOUT3, LOW);


    // DIGITAL INPUTS: receive state changes from the optocouplers    
    pinMode(DIN1, INPUT);
    pinMode(DIN2, INPUT);
    pinMode(DIN3, INPUT);
    pinMode(DIN4, INPUT);
    pinMode(DIN5, INPUT);
    pinMode(DIN6, INPUT);


    //Modbus communication runs at 9600 baud
    Serial.begin(9600, SERIAL_8N1);
    RS485.begin(9600);

    Wire.begin(i2c_sda, i2c_scl);
    Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");

    if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
    }
  
    node.begin(Slave_ID, RS485);

    // Callbacks allow us to configure the RS485 transceiver correctly
    node.preTransmission(preTransmission);
    node.postTransmission(postTransmission);
}


void loop() 
{
 
  
  long currentMillis = millis();
  
  if (currentMillis - lastMillis > 3000) 
  {

  cont++;
  if(cont==0){

    digitalWrite(DOUT1, LOW);
    digitalWrite(DOUT2, LOW);
    digitalWrite(DOUT3, LOW);
    }

  if(cont==1){

    digitalWrite(DOUT1, HIGH);
    digitalWrite(DOUT2, LOW);
    digitalWrite(DOUT3, LOW);
    }

  if(cont==2){

    digitalWrite(DOUT1, LOW);
    digitalWrite(DOUT2, HIGH);
    digitalWrite(DOUT3, LOW);
    }

  if(cont==3){

    digitalWrite(DOUT1, LOW);
    digitalWrite(DOUT2, LOW);
    digitalWrite(DOUT3, HIGH);
    cont=0;
    }

    Serial.println();
    Serial.println();
    int DI1 = digitalRead(DIN1);
    int DI2 = digitalRead(DIN2);
    int DI3 = digitalRead(DIN3);
    int DI4 = digitalRead(DIN4);
    int DI5 = digitalRead(DIN5);
    int DI6 = digitalRead(DIN6);

    Serial.print("DIN1: ");
    Serial.print(DI1);
    Serial.print("|| DIN2: ");
    Serial.print(DI2);
    Serial.print("|| DIN3: ");
    Serial.print(DI3);
    Serial.print("|| DIN4: ");
    Serial.print(DI4);
    Serial.print("|| DIN5: ");
    Serial.print(DI5);
    Serial.print("|| DIN6: ");
    Serial.print(DI6);
    Serial.println();
    Serial.println(); 
    
    int DO1 = digitalRead(DOUT1);
    int DO2 = digitalRead(DOUT2);
    int DO3 = digitalRead(DOUT3);
     
    Serial.print("DOUT1: ");
    Serial.print(DO1);
    Serial.print("|| DOUT2: ");
    Serial.print(DO2);
    Serial.print("|| DOUT3: ");
    Serial.print(DO3);
    
    Serial.println();
    Serial.println(); 

    int16_t adc0, adc1, adc2, adc3;
    float volts0, volts1, volts2, volts3;

    adc0 = ads.readADC_SingleEnded(0);
    adc1 = ads.readADC_SingleEnded(1);
    adc2 = ads.readADC_SingleEnded(2);
    adc3 = ads.readADC_SingleEnded(3);
  
    volts0 = ads.computeVolts(adc0);
    volts1 = ads.computeVolts(adc1);
    volts2 = ads.computeVolts(adc2);
    volts3 = ads.computeVolts(adc3);
  


    Serial.println("-----------------------------------------------------------");
    Serial.println("ADC ADS1115)");
    Serial.print("AIN0: "); Serial.print(adc0); Serial.print(" , "); Serial.print(volts0); Serial.print("V");
    Serial.print("||  AIN1: "); Serial.print(adc1); Serial.print(" , "); Serial.print(volts1); Serial.print("V");
    Serial.print("||  AIN2: "); Serial.print(adc2); Serial.print(" , "); Serial.print(volts2); Serial.print("V");
    Serial.print("||  AIN3: "); Serial.print(adc3); Serial.print(" , "); Serial.print(volts3); Serial.println("V");

    result = node.readInputRegisters(0, 8);

    if (result == node.ku8MBSuccess) {
      
      int ADC1 = node.getResponseBuffer(0x00);
      Serial.print("ADC1: ");
      Serial.println(ADC1);

      int ADC2 = node.getResponseBuffer(0x01);
      Serial.print("ADC2: ");
      Serial.println(ADC2);

      int ADC3 = node.getResponseBuffer(0x02);
      Serial.print("ADC3: ");
      Serial.println(ADC3);

      int ADC4 = node.getResponseBuffer(0x03);
      Serial.print("ADC4: ");
      Serial.println(ADC4);

      int ADC5 = node.getResponseBuffer(0x04);
      Serial.print("ADC5: ");
      Serial.println(ADC5);

      int ADC6 = node.getResponseBuffer(0x05);
      Serial.print("ADC6: ");
      Serial.println(ADC6);

      int ADC7 = node.getResponseBuffer(0x06);
      Serial.print("ADC7: ");
      Serial.println(ADC7);

      int ADC8 = node.getResponseBuffer(0x07);
      Serial.print("ADC8: ");
      Serial.println(ADC8);
      }

    Serial.println("");
    Serial.println("");

    //reading MODBUS RTU device ID from an ADAM HW
    result = node.readHoldingRegisters(0x00F7, 2);
    
    if (result == node.ku8MBSuccess) {
      int ID = node.getResponseBuffer(0x00);
      Serial.print("ID: ");
      Serial.println(ID);
      Serial.println("");
      } 
    
    else {
      // update of status failed
      Serial.println("Lectura MODBUS RTU falla o sin conexión!");
      }
    lastMillis = currentMillis;
  }
}

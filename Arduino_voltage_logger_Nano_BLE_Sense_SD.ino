//Arduino Nano BLE 33 Sense

#include <SPI.h>
#include <SD.h>

//SO P1.08
//SI P1.01
//CLK P0.13
//CS P1.13

const int chipSelect = D5;

 #define LENGTH 1000

 //int sensorValue_A0[LENGTH];
 int i = 0;
 float voltage_A0[LENGTH];

 bool flag = false;

 int led = 13;
  
void setup() {
  Serial.begin(9600);

  Serial.println("setup1");
  
  analogReadResolution(12);

  pinMode(D4, INPUT_PULLUP);
  attachInterrupt(D4,interrupt,RISING);

  pinMode(led, OUTPUT);
  flag = true;

  Serial.print("Initializing SD card...");
  SD.begin();

  // see if the card is present and can be initialized:
  //if (!SD.begin(chipSelect)) {
    //Serial.println("Card failed, or not present");
    // don't do anything more:
    //while (1);
  //}
  Serial.println("card initialized.");



  //pinMode(D4, INPUT_PULLUP);
  //attachInterrupt(D4,interrupt,RISING);
  Serial.println("setup2");  
}

void loop() {
  // make a string for assembling the data to log:
  String dataString = "";
  //Serial.println("loop");

  if(flag == true){
      digitalWrite(led, HIGH);  
      
      for(i=0; i<LENGTH; i++){
        voltage_A0[i] = analogRead(A0) * (3.3 / 4096.0);
        delay(2);
      }

      digitalWrite(led, LOW);  

      for(i=0; i<LENGTH; i++){
        Serial.print(voltage_A0[i], 6);
        Serial.print(",");
        dataString += String(voltage_A0[i], 6);
        dataString += ",";
      }
      //delay(1000);
      Serial.println();
      flag = false;

      
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File dataFile = SD.open("datalog.txt", FILE_WRITE);
  
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println(dataString);
      dataFile.close();
      // print to the serial port too:
      Serial.println(dataString);
    }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening datalog.txt");
    }

      //delay(2000);
      //go_to_sleep();
  }
}

void go_to_sleep(){
 NRF_POWER->SYSTEMOFF = 1; 
}

void interrupt(){
  flag = true;
}

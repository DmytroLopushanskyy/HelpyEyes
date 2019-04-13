#include "SD.h"
#define SD_ChipSelectPin 10
#include "TMRpcm.h"
//#include "SPI.h"
#include <stdio.h>
#include <Wire.h>

TMRpcm tmrpcm;

// Контакты TCS230 или TCS3200 для подключения к Arduino:
#define S0 4
//#define S1 5
#define S2 7
#define S3 6
#define BTN_PIN 2
#define sensorOut 8
#define LONG_RANGE
#define HIGH_ACCURACY
int sensorPin = A0;   // select the analog input pin for the photoresistor

int button_delay;
int button_logic = 1;

 
// для хранения частоты, считанной фотодиодами:
int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;
int measurement;
String finalColor;
String color;    // the number of the pushbutton pin
boolean buttonWasLow = false;         // variable flag for when the pushbutton goes low
static uint8_t lastBtnState = LOW;


// defines pins numbers
const int trigPin = 3;
const int echoPin = 5;

// defines variables
long duration;
int distance;
 
void setup() {
  // выставляем контакты S0, S1, S2 и S3 в режим OUTPUT:
  pinMode(S0, OUTPUT);
//  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(BTN_PIN, INPUT);
  digitalWrite(BTN_PIN, HIGH);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(9600);
  Wire.begin();
  //Serial.println("Ура!Hurray!");
  if(!SD.begin(SD_ChipSelectPin))
  { 
    //Serial.println("SD fail");
    return;
  }

  tmrpcm.speakerPin=9;
  tmrpcm.setVolume(6);
  tmrpcm.play("on.wav");
  // выставляем контакт sensorOut в режим INPUT:
  pinMode(sensorOut, INPUT);
 
  // задаем масштабирование частоты на 20%:
  digitalWrite(S0,HIGH);
  //digitalWrite(S1,LOW);
  // запускаем последовательную коммуникацию:

}
 
void loop() {

  if(!tmrpcm.isPlaying() or button_logic==3) {
      digitalWrite(tmrpcm.speakerPin, LOW);
      
    
      if (button_delay > 20){
        //Serial.println("2 sec!"); 
        
        button_delay = 0;
        button_logic++;
        if (button_logic == 5){
          button_logic = 1;
        }
        //Serial.println("Current mode: " + String(button_logic)); 
        if (button_logic == 1){
          //Serial.println("COLOR MODE"); 
          tmrpcm.play("next.wav");
          delay(2000);
          tmrpcm.play("co.wav");
        } 
        if (button_logic == 2){
          //Serial.println("LIGHT DETECTION MODE"); 
          tmrpcm.play("next.wav");
          delay(2000);
          tmrpcm.play("li.wav");
        } 
        if (button_logic == 3){
          //Serial.println("DISTANCE MODE"); 
          tmrpcm.play("next.wav");
          delay(2000);
          tmrpcm.play("in.wav");
          delay(1200);
          tmrpcm.play("di.wav");
          delay(2000);
          
        } 
        if (button_logic == 4){
          //Serial.println("DISTANCE MODE"); 
          tmrpcm.play("next.wav");
          delay(1400);
          tmrpcm.play("out.wav");
          delay(1500);
          tmrpcm.play("di.wav");
          delay(2000);
        } 
        buttonWasLow = false;
      }

      if (digitalRead(BTN_PIN) == LOW)  {
          buttonWasLow = true;
          button_delay++;
          delay(100);
          return;
      }else{
          button_delay = 0;
      }
      
      if (digitalRead(BTN_PIN) == HIGH && buttonWasLow && button_logic == 2){
        buttonWasLow = false;
        Serial.println(analogRead(sensorPin));
        if (analogRead(sensorPin) < 100){
            //Serial.println("LIGHT"); 
            tmrpcm.play("ex.wav");
            delay(1300);
            tmrpcm.play("lig.wav");
            delay(1000);
        }else if (analogRead(sensorPin) > 400){
            //Serial.println("DARK");
            tmrpcm.play("ex.wav");
            delay(1300);
            tmrpcm.play("dar.wav");
            delay(1000);
        }else if (analogRead(sensorPin) > 250){
            tmrpcm.play("dar.wav");
            delay(1000);
        }else{
            tmrpcm.play("lig.wav");
            delay(1000);
        }
      }
      
      
      // This if statement will only fire on the rising edge of the button input
      if (digitalRead(BTN_PIN) == HIGH && buttonWasLow && button_logic == 1)  {
          // reset the button low flag
          buttonWasLow = false;
          byte colorNum = detectColor();
          colorNum = (int) colorNum;
//          //Serial.println("pressed!");  
//          String colors[5];
//          for (int i=0; i < 5; i++){
//            //color = test();
//            color = detectColor();
//            //Serial.println(" " + color);  
//            colors[i] = color;
//         }  
//         if (colors[0] == colors[1] and colors[0] == colors[2] and colors[0] == colors[3] or colors[0] == colors[1] and colors[0] == colors[2] and colors[0] == colors[4] or colors[0] == colors[1] and colors[0] == colors[4] and colors[0] == colors[3] or colors[0] == colors[4] and colors[0] == colors[2] and colors[0] == colors[3]){
//            //Serial.println("FINAL COLOR: " + colors[0]);
//            finalColor = colors[0];
//         }else if(colors[4] == colors[1] and colors[4] == colors[2] and colors[4] == colors[3]){
//            //Serial.println("FINAL COLOR: " + colors[4]);
//            finalColor = colors[4];
//         }else{
//            //Serial.println("COLOR NOT DEFINED!");
//            finalColor = "noColor";
//         }
         if (colorNum % 3 == 2){
            tmrpcm.play("lig.wav");
            delay(1000);
            colorNum = colorNum - 1;
         }else if (colorNum % 3 == 0){
            tmrpcm.play("dar.wav");
            delay(900);
            colorNum = colorNum - 2;
         }
        Serial.print(colorNum);
//         //Serial.println(finalColor);
         if(colorNum == 28){
            tmrpcm.play("gre.wav");
         }else if(colorNum == 16){
            tmrpcm.play("yel.wav");
         }else if(colorNum == 19){
            tmrpcm.play("red.wav");
         }else if(colorNum == 34){
            tmrpcm.play("bro.wav");
         }else if(colorNum == 13){
            tmrpcm.play("ora.wav");
         }else if(colorNum == 4){
            tmrpcm.play("whi.wav");
         }else if(colorNum == 7){
            tmrpcm.play("bla.wav");
         }else if(colorNum == 25){
            tmrpcm.play("blu.wav");
         }else if(colorNum == 1){
            tmrpcm.play("pin.wav");
         }else if(colorNum == 10){
            tmrpcm.play("pur.wav");
         }else if(colorNum == 22){
            tmrpcm.play("gra.wav");
         }else if(colorNum == 37){
            tmrpcm.play("mar.wav");
         }
//         else if(finalColor == "noCo"){
//            //Serial.println("here");
//            tmrpcm.play("unknown.wav");
//            delay(1000);
//         }
         
         ////Serial.println(colors); 
      }
      
      if (button_logic == 3)  {
          buttonWasLow = false;
          //delay(100);
//           int distances[10];
//           for (int b=0; b < 10; b++){
//              distance = detectDistance(); 
//              distances[b] = distance;
//           }   
           
//          int count = 0;
          int finalNumber = 0;
          
//          for (int i=0; i<10; i++){
//            int maxNumber = distances[i] + 10;
//            int minNumber = distances[i] - 10;
//            for (int a=0; a<10; a++){
//              if (distances[a] > minNumber and distances[a] < maxNumber) count++;
//            }
//            if (count > 5){
//              finalNumber = distances[i];
//              //Serial.println("Final Distance: " + String(distances[i]));
//              count = 2002;
//            }else{
//              count = 0;
//            }
//          }
          finalNumber = detectDistance(); 
          
          if (finalNumber < 100 && finalNumber!=0){
            tmrpcm.play("fou.wav");
            delay(1500);
          }
      }
      if (button_logic == 4)  {
          buttonWasLow = false;
          delay(100);
//           int distances[10];
//           for (int b=0; b < 10; b++){
//              distance = detectDistance(); 
//              distances[b] = distance;
//           }   
           
//          int count = 0;
          int finalNumber = 0;
          
//          for (int i=0; i<10; i++){
//            int maxNumber = distances[i] + 10;
//            int minNumber = distances[i] - 10;
//            for (int a=0; a<10; a++){
//              if (distances[a] > minNumber and distances[a] < maxNumber) count++;
//            }
//            if (count > 5){
//              finalNumber = distances[i];
//              //Serial.println("Final Distance: " + String(distances[i]));
//              count = 2002;
//            }else{
//              count = 0;
//            }
//          }
          finalNumber = detectDistance(); 
          
          if (finalNumber < 400 && finalNumber!=0){
            tmrpcm.play("fou.wav");
            delay(1500);
          }
      }
   }
  
  
}

byte detectColor(){
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
 
  redFrequency = pulseIn(sensorOut, LOW);
  delay(100);

  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
 
  greenFrequency = pulseIn(sensorOut, LOW);
  delay(100);
 
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);

  blueFrequency = pulseIn(sensorOut, LOW);
  delay(100);

  byte values[30][4]={
    {1,33, 111, 52},
    {4,22, 23, 17},
    {7,255, 255, 255},
    {10,108, 135, 60},
    {13,29, 75, 75},
    {16,24, 35, 55},
    {30,115, 90, 84},
    {29,33, 36, 55},
    {26,66, 54, 30},
    {19,31, 126, 87},
    {22,53, 59, 46},
    {16,35, 55, 77},
    {7,170, 207, 150},
    {1,27, 60, 34},
    {27,130, 82, 40},
    {27,142, 139, 80},
    {29,80, 51, 74},
    {34,75, 115, 102},
    {34,82, 142, 127},
    {36,110, 190, 140},
    {29,58, 45, 46},
    {13,28, 63, 46},
    {1,27, 72, 55},
    {20,29, 114, 77},
    {17,20, 26, 34},
    {13,28, 43, 57},
    {13,28, 62, 69},
    {27,123, 125, 73},
    {30,70, 86, 91},
    {37, 52, 132, 90}
  };

  int diff[30];
  for (int i=0; i< 30; i++){
    diff[i] = abs((redFrequency - values[i][1])) + abs((greenFrequency - values[i][2]))+ abs((blueFrequency - values[i][3]));
//    Serial.print(diff[i] );
//    Serial.println("  ");
  }
  int minIndex = 0;
  int minValue = diff[minIndex];
  
  for(int i = 0; i < 30; i++)
  {
      if(diff[i] < minValue) {
          minValue = diff[i];
          minIndex = i;
      }
  };
  //return colors[minIndex];
//  Serial.println("Final:");
//Serial.print(values[minIndex][0]);
//  Serial.println("  ");
  return values[minIndex][0];
}

int detectDistance(){
   // Clears the trigPin
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(10);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH, 30000);
  
  // Calculating the distance
  distance= duration*0.034/2;
  
  // Prints the distance on the Serial Monitor
  //Serial.print("Distance: ");
  //Serial.println(distance);
  return distance;
}



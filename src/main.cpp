
#include "softUART.h"
#include <Arduino.h>

enum STATES 
{
  OFF,
  ON,
  READDIGITAL,
  READANALOG
};

int state = OFF;

unsigned long lastDraw = 0;
unsigned long drawDelay = 200;

char lastInput = '\0';

const char startChar = 's';
const char stopChar = 'c';
const char digitalChar = 'd';
const char analogChar = 'a';


SoftUART SoftSerial( 2 ,3, 9600, 8, false, false);

void setup() 
{
  //Serial.begin(9600);
  pinMode(8,INPUT_PULLUP);
  pinMode(9,INPUT_PULLUP);
  pinMode(10,INPUT_PULLUP);
  pinMode(11,INPUT_PULLUP);
  pinMode(12,INPUT_PULLUP);
  pinMode(13,INPUT_PULLUP);
}

void ClearOutput()
{
  SoftSerial.Print((char)27); // ESC
  SoftSerial.Print("[2J"); // clear screen
  SoftSerial.Print((char)27); // ESC
  SoftSerial.Print("[H"); // cursor to home
}

void DrawHeading() 
{
  SoftSerial.Println("View current Input Levels:");
  SoftSerial.Println("- D: Digital Inputs D8 - D13");
  SoftSerial.Println("- A: Analog Inputs A0 - A6");
  SoftSerial.Println("- C: Clear Screen");
  SoftSerial.Println("");
}

void DrawDigitalInputs() 
{
  SoftSerial.Print("D08: ");
  SoftSerial.Println(String(digitalRead(8)));
  SoftSerial.Print("D09: ");
  SoftSerial.Println(String(digitalRead(9)));
  SoftSerial.Print("D10: ");
  SoftSerial.Println(String(digitalRead(10)));
  SoftSerial.Print("D11: ");
  SoftSerial.Println(String(digitalRead(11)));
  SoftSerial.Print("D12: ");
  SoftSerial.Println(String(digitalRead(12)));
  SoftSerial.Print("D13: ");
  SoftSerial.Println(String(digitalRead(13)));
  SoftSerial.Println("");
}

void DrawAnalogOutputs()
{
  SoftSerial.Print("A0: ");
  SoftSerial.Println(String(analogRead(0)));
  SoftSerial.Print("A1: ");
  SoftSerial.Println(String(analogRead(1)));
  SoftSerial.Print("A2: ");
  SoftSerial.Println(String(analogRead(2)));
  SoftSerial.Print("A3: ");
  SoftSerial.Println(String(analogRead(3)));
  SoftSerial.Print("A4: ");
  SoftSerial.Println(String(analogRead(4)));
  SoftSerial.Print("A5: ");
  SoftSerial.Println(String(analogRead(5)));
  SoftSerial.Println("");
}

void EndOutput() 
{
  ClearOutput();

  SoftSerial.Println("Shutting down...");
  delay(2000);

  ClearOutput();

  state = OFF;
}

int InputToState(char inputChar, STATES newState) 
{
  if (lastInput == inputChar)
  {
    state = newState;
    ClearOutput();
    DrawHeading();
    return 1;
  }
  return 0;
}

void loop() {

  if (SoftSerial.Available())
  {
    lastInput = SoftSerial.Read();
  } 
  else 
  {
    lastInput = '\0';
  }

  if (InputToState(stopChar, OFF) == 1) 
  {
    ClearOutput();
  }
  
  switch (state)
  {
  case OFF:
    InputToState(startChar, ON);
    break;
  case ON:
    InputToState(digitalChar, READDIGITAL);
    InputToState(analogChar, READANALOG);
    break;
  case READDIGITAL:
    if (millis() - lastDraw >= drawDelay) 
    {
      ClearOutput();
      DrawHeading();
      DrawDigitalInputs();
      lastDraw = millis();
    }
    break;
  case READANALOG:
    if (millis() - lastDraw >= drawDelay) 
    {
      ClearOutput();
      DrawHeading();
      DrawAnalogOutputs();
      lastDraw = millis();
    }
    break;
  default:
    state = OFF;
    break;
  }
}
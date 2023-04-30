#include "softUART.h"

SoftUART::SoftUART(unsigned int pinRX, unsigned int pinTX, unsigned long bitRate, unsigned int dataLength, bool hasParity, bool hasTwoStopBits)
:pinRX(pinRX)
,pinTX(pinTX)
,bitRate(bitRate)
,dataLength(dataLength)
,hasParity(hasParity)
,hasTwoStopBits(hasTwoStopBits)
{
    pinMode(this->pinRX,INPUT);
    pinMode(this->pinTX,OUTPUT);

    digitalWrite(this->pinTX,HIGH);

    this->bitDelay = 1000000 / this->bitRate;

    this->receiveBuffer = "";

    this->recState = IDLE;
}

unsigned int SoftUART::Available() 
{

    unsigned int receivedChar = 0;
    unsigned int highCount = 0;
    unsigned int parityValue = 0;

    if (digitalRead(pinRX) == LOW && recState == IDLE)
    {
        delayMicroseconds(bitDelay-RECEIVE_MARGIN);
        recState = DATA_IN;
    }

    if (recState == DATA_IN)
    {
        for(unsigned int i = 0; i<dataLength; i++)
        {
            receivedChar >>= 1;
            if (digitalRead(pinRX) == HIGH)
            {
                receivedChar |= 0x80;
                highCount++;
            }
            
            delayMicroseconds(bitDelay-RECEIVE_MARGIN);
        }
        if (!hasParity) {
            recState=IDLE;
            receiveBuffer += (char)receivedChar;
        } else {
            recState=PARITYBIT_IN;
        }
        delayMicroseconds(bitDelay-RECEIVE_MARGIN);
        
    }

    if (recState == PARITYBIT_IN ) 
    {
        if (digitalRead(pinRX) == HIGH)
        {
            parityValue++;
        }

        if (highCount%2 == parityValue)
        {
            receiveBuffer += (char)receivedChar;
            recState=IDLE;
        }

        delayMicroseconds(bitDelay-RECEIVE_MARGIN);
    }

    return receiveBuffer.length();
}

unsigned int SoftUART::Print(char character)
{

    digitalWrite(pinTX,LOW);
    delayMicroseconds(bitDelay-SENDSPEED_MARGIN);

    unsigned int parity = 0;

    for (unsigned int i = 0; i < dataLength; i++) 
    {
        if (character & 0x01) 
        {
            digitalWrite(pinTX,HIGH);
        } 
        else 
        {
            digitalWrite(pinTX,LOW);
        }
        character >>= 1;
        delayMicroseconds(bitDelay-SENDSPEED_MARGIN);
    }

    if (parity) {
        if (parity%2 == 1) 
        { 
            digitalWrite(pinTX,HIGH);
        } 
        else 
        {
            digitalWrite(pinTX,LOW);
        }
        delayMicroseconds(bitDelay-SENDSPEED_MARGIN);
    }
    
    
    digitalWrite(pinTX,HIGH);
    delayMicroseconds(bitDelay-SENDSPEED_MARGIN);
    
    if (hasTwoStopBits) 
    {
        digitalWrite(pinTX,HIGH);
        delayMicroseconds(bitDelay-SENDSPEED_MARGIN);
    }
    return 0;
}

unsigned int SoftUART::Print(String text)
{
    for(unsigned int i = 0; i < text.length(); i++)
    {
        Print(text[i]);
    }
    return 0;
}

unsigned int SoftUART::Print(int number)
{
    Print(String(number,DEC));
    return 0;
}

unsigned int SoftUART::Println(String text)
{
    Print(text);
    Print('\n');
    Print((char)13); // ESC
    return 0;
}

char SoftUART::Read() 
{
    if (receiveBuffer.length() > 0) 
    {
        char firstChar = receiveBuffer[0];
        receiveBuffer.remove(0);
        return firstChar;
    } 
    return '\0';
}
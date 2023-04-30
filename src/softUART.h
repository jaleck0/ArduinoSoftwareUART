#ifndef SOFTUART_H
#define SOFTUART_H

#define STRINGBUFFER_SIZE 50
#define RECEIVE_MARGIN 5
#define SENDSPEED_MARGIN 4

#include <Arduino.h>

typedef enum
{
    IDLE,
    DATA_IN,
    PARITYBIT_IN
}  ReceiveState;


class SoftUART
{
public:
    SoftUART(unsigned int pinRX, unsigned int pinTX, unsigned long bitRate, unsigned int dataLength, bool hasParity, bool hasTwoStopBits);
    unsigned int Available();
    unsigned int Print(char character);
    unsigned int Print(String text);
    unsigned int Print(int number);

    unsigned int Println(String text);
    char Read();

private:
    unsigned long bitRate;
    unsigned int dataLength;
    bool hasParity;
    bool hasTwoStopBits;
    unsigned int pinRX;
    unsigned int pinTX;
    unsigned long bitDelay;
    String receiveBuffer;

    ReceiveState recState; 
};

#endif
# sm5100b
sm5100B GSM modem written in C

A couple of years ago I wanted to manage some devices (arduinos, computers) using SMSs. So I bought a GSM modem: an SM5100B. My first 
idea was to use them directly from and Arduino. Unfortunately, I found quite complicated doing complex things with and Arduino due to the limitations it has (memory, language) Most likely the worst part was me, not being able to solve the technical problems I found.

So I decided to interface it directly with it through the USB port of a computer by using C. And this is what the library is about. It has also been my first experiment with github.

The library has --theoretically-- four parts:
1. Serial management
2. Basic AT support for GSM modem(s) (it's quite standard, so it should work for most modems)
3. SMS Support
4. GPRS Internet connections.

I wrote "theoretically" because I have written it at the same time I have learned about GSM (I started with no idea about the AT commands, nor SMSs, PDU, etc) and some things have been left aside. For example, the serial part does not exist. I know I have to spend some time reading about serial communication (more specifically, serial set-up) and make it work whatever the USB state is.


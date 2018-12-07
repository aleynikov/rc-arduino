#ifndef gsmat_h
#define gsmat_h

#include "Arduino.h"
#include "SoftwareSerial.h"

class GSMAT
{
    public:
        GSMAT(int baund);
        void send(const char* msg, int wait_ms = 500);
        String get_content(); 
    private:
        SoftwareSerial _gsm;
        byte _ps;
        void _parse_response(byte b);
        void _reset_buffer();
        void _reset_content();
        void _wait(int ms);
};

#endif

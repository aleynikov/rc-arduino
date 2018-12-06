#include "gsmat.h"
#include "Arduino.h"
#include "SoftwareSerial.h"

#define PIN_RX 7
#define PIN_TX 8

enum _ps {
  PS_DETECT_MSG_TYPE,

  PS_IGNORING_COMMAND_ECHO,

  PS_HTTPACTION_TYPE,
  PS_HTTPACTION_RESULT,
  PS_HTTPACTION_LENGTH,

  PS_HTTPREAD_LENGTH,
  PS_HTTPREAD_CONTENT,

  PS_SAPBR,
  PS_CME_ERROR
};
byte ps = PS_DETECT_MSG_TYPE;

char content[255];
int content_pos = 0;
int content_length = 0;

char buffer[255];
int buffer_pos = 0;

GSMAT::GSMAT(int baund): _gsm(PIN_RX, PIN_TX)
{
    _gsm.begin(baund);
}

void GSMAT::_parse_response(byte b)
{  
    buffer[buffer_pos++] = b;

    if (buffer_pos >= sizeof(buffer))
        _reset_buffer();

    switch (ps) {
        case PS_DETECT_MSG_TYPE:
        {
            if (b == '\n')
                _reset_buffer();
            else {
                if (buffer_pos == 3 && strcmp(buffer, "AT+") == 0) {
                    ps = PS_IGNORING_COMMAND_ECHO;
                }
                else if (b == ':') {
                    if (strcmp(buffer, "+HTTPACTION:") == 0) {
                        ps = PS_HTTPACTION_TYPE;
                    }
                    else if (strcmp(buffer, "+HTTPREAD:") == 0) {
                        ps = PS_HTTPREAD_LENGTH;
                    }
                    else if (strcmp(buffer, "+CME ERROR:") == 0) {
                        ps = PS_CME_ERROR;
                    }

                    _reset_buffer();
                }
            }
        }
        break;

        case PS_IGNORING_COMMAND_ECHO:
        {
            if (b == '\n') {
                ps = PS_DETECT_MSG_TYPE;
                _reset_buffer();
            }
        }
        break;

        case PS_HTTPACTION_TYPE:
        {
            if (b == ',') {
                ps = PS_HTTPACTION_RESULT;
                _reset_buffer();
            }
        }
        break;

        case PS_HTTPACTION_RESULT:
        {
            if (b == ',') {
                ps = PS_HTTPACTION_LENGTH;
                _reset_buffer();
            }
        }
        break;

        case PS_HTTPACTION_LENGTH:
        {
            if (b == '\n') {
                _gsm.print("AT+HTTPREAD=0,");
                _gsm.println(buffer);

                ps = PS_DETECT_MSG_TYPE;
                _reset_buffer();
            }
        }
        break;

        case PS_HTTPREAD_LENGTH:
        {
            if (b == '\n') {
                content_length = atoi(buffer);

                ps = PS_HTTPREAD_CONTENT;
                _reset_buffer();
            }
        }
        break;

        case PS_HTTPREAD_CONTENT:
        {
            content[content_pos++] = b;
            content_length--;

            if (content_length <= 0) {
                ps = PS_DETECT_MSG_TYPE;
                _reset_buffer();
            }
        }
        break;

        case PS_SAPBR:
        {
            if (b == '\n') {
                ps = PS_DETECT_MSG_TYPE;
                _reset_buffer();
            }
        }
        break;

        case PS_CME_ERROR:
        {
            if (b == '\n') {
                ps = PS_DETECT_MSG_TYPE;
                _reset_buffer();
            }
        }
        break;
    }
}

void GSMAT::_reset_buffer()
{
    memset(buffer, 0, sizeof(buffer));
    buffer_pos = 0;
}

void GSMAT::_reset_content()
{
    memset(content, 0, sizeof(content));
    content_pos = 0;
}

void GSMAT::_wait(int ms)
{
    delay(ms);
}

void GSMAT::send(const char* msg, int wait_ms)
{  
    _gsm.println(msg);
    _wait(wait_ms);

    while (_gsm.available()) {
        _parse_response(_gsm.read());
    }
}

String GSMAT::get_content()
{
    String _content = String((char*) content);
    _reset_content();
    return _content;
}

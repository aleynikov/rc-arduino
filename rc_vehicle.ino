#include "gsmat.h"

#define GSM_BAUND 9600

GSMAT gsmat(GSM_BAUND);

void setup()
{
  gsmat.send("AT+SAPBR=3,1,\"APN\",\"vodafone\"");
  gsmat.send("AT+SAPBR=2,1");
  gsmat.send("AT+SAPBR=1,1");
  gsmat.send("AT+HTTPINIT");
  gsmat.send("AT+HTTPPARA=\"CID\",1");
  gsmat.send("AT+HTTPPARA=\"URL\",\"http://api.smart-drive.com.ua:7777/test/car/ping\"");

  Serial.begin(9600);
}

void ping()
{
  gsmat.send("AT+HTTPACTION=0", 5000);

  // show content
  Serial.print(gsmat.get_content());
}

void loop()
{
  ping();
}

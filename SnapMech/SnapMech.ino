#include <ESP32Servo.h>
#include "bleCharas.h"

#define SERVOS 6
Servo servos[SERVOS]; // create the servo objects
int servoPins[SERVOS] = {13, 27, 26, 25, 33, 32};
#define SRV_LOOKLR 0
#define SRV_LOOKUD 1
#define SRV_LIDBL 2
#define SRV_LIDTL 3
#define SRV_LIDBR 4
#define SRV_LIDTR 5

int UDState = 90;
int LRState = 90;
int blinkState;
int lidMod = 0;
int lidPos = 0;

typedef enum OpMode_enum
{
  Normal,
  RangeCheck
}
OpMode_t;
OpMode_t OpMode;

#define BLINK_LEFT 1
#define BLINK_RIGHT 2
#define CHMODE_RANGE 0x80

void setup()
{
  Serial.begin(115200);
  OpMode = Normal;
  
  log_i("Attaching servos!");
 
  for (int idx=0; idx<SERVOS; idx++)
  {
    servos[idx].attach(servoPins[idx]);
  }

  setupBLE();
}

void loop()
{
  loopBleNormal();
  switch (OpMode)
  {
    case Normal:
      DoNormal();
    break;
    case RangeCheck:
      DoRangeCheck();
    break;
  }
}

void DoNormal()
{
  blinkState =  charaData.normal[IDX_BLINK];
  if (blinkState & CHMODE_RANGE)
  {
    log_i("Mode: RangeCheck");
    OpMode = RangeCheck;

    // read current servo values
    for (int idx=0; idx<SERVOS; idx++)
    {
      charaData_sav.raw[idx] = (uint8_t)servos[idx].read();
    }
    writeBleData(charaData_sav.raw, RAW_LENGTH);
  }
  else
  {
    UDState = map(charaData.normal[IDX_EYES_Y], 1000, -1000, 50, 130);
    lidMod = (40 - UDState)/2;
    LRState = map(charaData.normal[IDX_EYES_X], 1000, -1000, 40, 140);
    lidPos = map(charaData.normal[IDX_LID_POS], 1000, -1000, 30, -30);
    
    servos[SRV_LOOKUD].write(UDState);
    servos[SRV_LOOKLR].write(LRState);

    if (blinkState & BLINK_LEFT)
    {
      servos[SRV_LIDBL].write(90);
      servos[SRV_LIDTL].write(90);
    }
    else 
    {
      servos[SRV_LIDBL].write(156+lidMod+lidPos);
      servos[SRV_LIDTL].write(63+lidMod-lidPos);
    }

    if (blinkState & BLINK_RIGHT)
    {
      servos[SRV_LIDBR].write(90);
      servos[SRV_LIDTR].write(84);
    }
    else 
    {
      servos[SRV_LIDBR].write(21-lidMod-lidPos);
      servos[SRV_LIDTR].write(107-lidMod+lidPos);
    }
  }
}

void DoRangeCheck()
{
  blinkState =  charaData.normal[IDX_BLINK];
  if (!(blinkState & CHMODE_RANGE))
  {
    log_i("Mode: Normal");
    OpMode = Normal;
  }
  else
  {
    for (int idx=0; idx<SERVOS; idx++)
    {
      servos[idx].write(charaData_sav.raw[idx]);
    }
  }
}

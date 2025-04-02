#ifndef __CM__
#define __CM__
#include <Arduino.h>
#include <ArduinoJson.h>
//Use version 5, not version 6

class CM
{
  public:
  enum ACFanModeEnum
  {
    _LOW = 0,
    _HIGH = 1,
    _AUTO = 2
  };

  struct PdmDigitalOutput
  {
    char szName[16];
    byte bSoftStartStepSize;
    byte bMotorOrLamp;
    byte bLossOfCommunication;
    byte bByte7;
    byte bByte8;
    float fFeedback;
    byte bCommand;
    
  };
  
  
  struct heaterStruct
  {
    bool bFanOn;
    int nFanSpeed;
    bool bAirFlowOut;
    bool bAutoMode;
    
    float fTargetTemp;//stored as C
    bool bHotWater;
    bool bFurnace;
    
  }heater;
  struct ACStruct
  {
    bool bCompressor;
    bool bFan;
    ACFanModeEnum fanMode;
    bool bMagicClimate;
    float fTargetTemp;//stored as C
    byte lastFanMode;
    byte lastOperatingMode;
    
  }ac;
  PdmDigitalOutput pdm1_output[13];//to accomidate 1-12 inclusive.  Total of 13
  PdmDigitalOutput pdm2_output[13];
  
  int nFreshTankLevel;
  int nGrayTankLevel;
  int nFreshTankDenom;
  int nGrayTankDenom;
  float fAmbientTemp;
  float fAmbientVoltage;
  struct fanStruct
  {
    int nSystemStatus;
    int nFanMode;
    int nSpeedMode;
    int nLight;
    int nSpeed;
    int nWindDirection;
    int nDomePosition;
    int fSetPoint;
    float fAmbientTemp;
    
  }roofFan;
  
  
 
  void init();
  float cToF (float fDeg);
  void data2Json(char *buffer,char *varName,bool b);
  void data2Json(char *buffer,char *varName,int n);
  void data2Json(char *buffer,char *varName,float f);
  void getHeaterInfo(char *buffer);
  void getACInfo(char *buffer);
  void getTankInfo(char *buffer);
  void getMiscInfo(char *buffer);
  void getAllInfo(char *buffer);
};
#endif

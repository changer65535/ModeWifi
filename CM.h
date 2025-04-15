  #ifndef __CM__
#define __CM__
#include <Arduino.h>
#include <mcp2515.h>
#include <ArduinoJson.h> //Use version 6, not version 7

#define THERMOSTAT_COMMAND_1 0x99FEF903
#define PDM1_COMMAND      0x94ef1e11
#define PDM2_COMMAND      0x94ef1f11

#define PDM1_MESSAGE      0x94EF111E
#define PDM2_MESSAGE      0x94EF111F
#define TANK_LEVEL        0x99FFB7AF
#define RIXENS_COMMAND    0x788
#define THERMOSTAT_AMBIENT_STATUS 0x99FF9C58
#define THERMOSTAT_STATUS_1 0x99FFE258 
#define ROOFFAN_STATUS    0x99FEA758
#define ROOFFAN_CONTROL 0x99FEA603
#define PDM1_SHORT 0x94E9111E
#define PDM2_SHORT 0x94E9111F



////////////PDM CONSTANTS DEFINED WITHIN STORYTELLER SCREEN-- MAY OR MAY NOT REFLECT ACTUAL WIRING
#define PDM1_DIN_CARGO_LIGHT_SW 3
#define PDM1_DIN_CABIN_LIGHT_SW 4
#define PDM1_DIN_AWNING_LIGHT_SW 5
#define PDM1_DIN_RECIRC_PUMP_SW 6
#define PDM1_DIN_AWNING_ENABLE_SW 7
#define PDM1_DIN_ENGINE_RUNNING 8
#define PDM1_DIN_AWNING_IN_SW 11
#define PDM1_DIN_AWNING_OUT_SW 12
#define PDM1_OUT_SOLAR_BACKUP 1
#define PDM1_OUT_CARGO_LIGHTS 2
#define PDM1_OUT_READING_LIGHTS 3
#define PDM1_OUT_CABIN_LIGHTS 4
#define PDM1_OUT_AWNING_LIGHTS 5
#define PDM1_OUT_RECIRC_PUMP 6
#define PDM1_OUT_AWNING_ENABLE 7
#define PDM1_OUT_EXHAUST_FAN 10
#define PDM1_OUT_FURNACE_POWER 11
#define PDM1_OUT_WATER_PUMP 12


#define PDM2_DIN_AUX_SW 4
#define PDM2_DIN_WATER_PUMP_SW 5
#define PDM2_DIN_MASTER_LIGHT_SW 6 f
#define PDM2_DIN_SINK_SW 9
#define PDM2_OUT_GALLEY_FAN 2
#define PDM2_OUT_REGRIGERATOR 3
#define PDM2_OUT_12V_USB 4
#define PDM2_OUT_AWNING_MOTOR_PLUS 5
#define PDM2_OUT_AWNING_MOTOR_MINUS 6
#define PDM2_OUT_TANK_MONITOR_PWR 7
#define PDM2_OUT_POWER_SW 8
#define PDM2_OUT_HVAC_POWER 9
#define PDM2_OUT_12V_SPEAKER 10
#define PDM2_OUT_SINK_PUMP 11
#define PDM2_OUT_AUX_POWER 12

class CM
{
  public:
   

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
    byte fanMode;
    byte operatingMode;
    byte fanSpeed;
    float fSetpointCool;
    word setpointHeat;
    word setpointCool;
    
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
  
  MCP2515* mPtr;
  can_frame lastACCommand;
  bool bVerbose = false;
  can_frame zeroPDM;
 
  void init(MCP2515* thisPtr);
  float cToF (float fDeg);
  void data2Json(char *buffer,char *varName,bool b);
  void data2Json(char *buffer,char *varName,int n);
  void data2Json(char *buffer,char *varName,float f);
  void getHeaterInfo(char *buffer);
  void getACInfo(char *buffer);
  void getTankInfo(char *buffer);
  void getMiscInfo(char *buffer);
  void getAllInfo(char *buffer);

  float byte2Float(byte b);
  float bytes2DegreesC(byte b1,byte b2);
  
  void printCan (can_frame m,bool bLF = true);
  
  ///DIAGNOSTICS
  void handleDiagnostics (can_frame m);
  ////AC COMMANDS
  void handleThermostatStatus(can_frame m);
  void setACFanSpeed (byte newSpeed);
  void acCommand (byte bFanMode,byte bOperatingMode,byte bSpeed);
  void setACFanMode (byte bFanMode);
  void setACOperatingMode (byte newMode);
  void acSetTemp (float fTemp);
  //RIXENS
  void handleRixens(can_frame m);
  //TANK
  void handleTankLevel(can_frame m);
  //PDM
  void handlePDMCommand(can_frame m);
  //ROOF FAN
  void handleRoofFanStatus (can_frame m);
  void handleRoofFanControl(can_frame m);
  void setVentSpeed (int nSpeed);
  void closeVent();
  void openVent();
  void setVentDirection (bool bDir);  //0 = out, 1 = in
  

  
void handleThermostatAmbientStatus (can_frame m);
};
#endif

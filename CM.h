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
#define ACK_CODE          0x98E80358
#define RIXENS_COMMAND            0x788
#define RIXENS_RETURN1            0x78A
#define RIXENS_RETURN2            0x78B
#define RIXENS_RETURN3            0x724
#define RIXENS_RETURN4            0x725
#define RIXENS_GLYCOLVOLTAGE      0x726
#define RIXENS_RETURN6            0x728


/*
 * 0x788 is Rixens CanBus ID
 * b[0] is command
 * if 1: set target temperature in celsius * 10 from b[1] and b[2] * 256
 * if 2: set blower speed as b[1] 0-100
 *       also if b[1] is 0xff fan will be auto
 * 
 * if 3: if b[1] = 1, heat source is furnace if b[0] is 0, turn off
 * if 4: if b[1] = 1, heat source is electric
 * if 5: if b[1] = 1, heat source is engine
 * if 6: if b[1] = 1, heat up the hot water
 * if 7: if b[1] = 1, engine pre-heet
 * if 8: update rixens control with ambient temp
 * if 0x0b Tell Rixen's engine is running b[1] = on/off
 * if 0x0c Enable Thermostat with b[1] = on/off
 * 
 * 
 * 
 * 0x726 is rixens diagnostic data
 * glycol temp is b[2] and b[3]
 * voltage is b[6] and b[7]
 * 
 * 
 * 0x725 is more diagnostic data
 * heater fan is b[4] and b[5]
 * heater glow is 6-7
 * heater fuel is 3
 * 
 * 0x724 bidirectional control
 * bit 53 is engine selected x[6] & 0b00100000; // Bit 53 in the payload
 * bit 51 is furnace selected bool furnaceSelected = x[6] & 0b00001000; // Bit 51 in the payload
 * bit 52 is electric selected bool electricSelected = x[6] & 0b00010000; // Bit 52 in the payload 
 * bool electricSelected = x[6] & 0b00010000; // Bit 52 in the payload
 * 
 *******************************************************
--FFB& is water level
b[0] = 0 = fresh water
b[1] = 2:Gray Water
b[2] = level
b[3] = resolution
*/


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

   String szMessage;
  //These are the last digital inputs coming from PDM
  can_frame lastPDM1inputs1to6;
  can_frame lastPDM1inputs7to12;
  can_frame lastPDM2inputs1to6;
  can_frame lastPDM2inputs7to12;
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
  PdmDigitalOutput pdm1_output[13];//to accomidate 1-12 inclusive.  Total of 13
  PdmDigitalOutput pdm2_output[13];

byte *bPtrPumpCommand = &pdm1_output[PDM1_OUT_WATER_PUMP].bCommand;
byte *CABIN_LIGHTS_COMMAND = &pdm1_output[PDM1_OUT_CABIN_LIGHTS].bCommand;
byte *CARGO_LIGHTS_COMMAND = &pdm1_output[PDM1_OUT_CARGO_LIGHTS].bCommand;
  
  struct heaterStruct
  {
    bool bFanOn;
    int nFanSpeed;
    bool bAirFlowOut;
    bool bAutoMode;
    
    float fTargetTemp;//stored as C
    bool bHotWater;
    bool bFurnace;
    float fGlycolOutletTemp;
    float fGlycolInletTemp;
    
    
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
  
  
  int nFreshTankLevel = -1;
  int nGrayTankLevel = -1;
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
    byte bRainSensor;
    
    
  }roofFan;
  
  MCP2515* mPtr;
  can_frame lastACCommand;
  bool bVerbose = false;

  
  bool bMiniPumpMode = false;
  
  can_frame zeroPDM;
  
  void handleCabinBlink (int nInit = 0);
  void pressdigitalbutton (can_frame mLast,int nDataIndex,int nByteNum);
  void init(MCP2515* thisPtr);
  float cToF (float fDeg);
  

  
  void getHeaterInfo(char *buffer);
  void getACInfo(char *buffer);
  void getTankInfo(char *buffer);
  void getMiscInfo(char *buffer);
  

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
  void acOff();
  //RIXENS
  void handleRixensCommand(can_frame m);
  void handleRixensReturn (can_frame m);
  void handleRixensGlycolVoltage(can_frame m);
  //TANK
  void handleTankLevel(can_frame m);

  //PDM MESSAGES
  void handleSupplyVoltage (can_frame m);
  float feedbackAmps(can_frame m,int nChannelNumber);
  void handleFeedback1to6 (float t,can_frame m);
  void handleFeedback7to12 (float t,can_frame m);
  void handleMessage134 (float t,can_frame m);
  void handleMessage135 (float t,can_frame m);
  void handleMessage136 (float t,can_frame m);
  void handleHeartBeat (can_frame m);
  void handlePDMMessage (float t,can_frame m);
  float tenBitAnalog(can_frame m,int channelNumber);
  char getDigitalInput (can_frame m,int nInputNumber);
  void handleMessage128 (float t,can_frame m);
  void handleMessage129 (float t,can_frame m);
  void handleMessage130 (float t,can_frame m);
  void handleMessage131 (float t,can_frame m);


  
  //PDM COMMANDS
  void printInputDiagnostics(byte b);
  void printBin(byte aByte);
  void handlePDMShort (can_frame m);
  void handlePDMDigitalInput (float t,can_frame m);
  void handlePDMCommand(can_frame m);
  void handleEngineOnAllOff(can_frame m);
  void handleFrontButtonDoubleTap(can_frame m);
  //PDM QUICKIES
  void pumpOff ();
  void circOff();
  void cabinOff();
  void awningOff();
  void cargoOff();
  void pressCabin();
  void pressCargo();
  void pressAwning();
  void pressCirc();
  void pressPump ();
  void pressAwningIn ();
  void pressAwningOut ();
  void pressAux();
  void auxOff();
  void auxOn();
  void cabinOn();
  void pressDrain();
  void cargoOn();
  void awningOn();
  void pressAwningEnable();
  
  //ROOF FAN
  void handleAck (can_frame m);
  void handleRoofFanStatus (can_frame m);
  void handleRoofFanControl(can_frame m);
  void setVentSpeed (int nSpeed);
  void closeVent();
  void openVent();
  void setVentDirection (bool bDir);  //0 = out, 1 = in

  //smart stuff
  void handleSmartSiphon (int handleSmartSiphon = 0);
  void handleDrinkBlink(bool bBlinkMode = 0);
  void handleMiniPump();
  void handleMinutePump(int nInitVal = 0);
  void handleShowerJerk(int nInitVal = 0);
  void handleRandomLights(int nInitVal = 0);
  void handleWaterTracker(int nInitVal = 0);
  void handleWaterTempBlink(int nInitVal = 0);
  

  
  void handleThermostatAmbientStatus (can_frame m);
};
#endif

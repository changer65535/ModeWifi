#include "cm.h"
void CM::init(MCP2515* thisPtr)
{

  
  mPtr = thisPtr;
  fAmbientTemp = 0;
  heater.bFanOn = false;
  heater.nFanSpeed = 0;
  heater.bAirFlowOut = false;
  heater.bAutoMode = false;
  heater.fTargetTemp = 31;
  heater.bHotWater = false;
  heater.fGlycolInletTemp = 0.0;
  heater.fGlycolOutletTemp = 0.0;
  
  //AC
  ac.bCompressor = false;
  ac.bFan = false;
  ac.fanMode = 0;
  
  
  ac.fanMode = 0;
  ac.operatingMode = 0;
  ac.fanSpeed = 0;
  ac.setpointHeat = 30;
  ac.setpointCool = 20;
  ac.fSetpointCool = 20.0;

  zeroPDM.can_id = 0;
  zeroPDM.can_dlc = 0;
  for (int i = 0;i<8;i++) zeroPDM.data[i] = 0;    
  lastPDM1inputs1to6  = zeroPDM;
  lastPDM1inputs7to12 = zeroPDM;
  lastPDM2inputs1to6  = zeroPDM;
  lastPDM2inputs7to12 = zeroPDM;

  lastACCommand = zeroPDM;
  lastACCommand.can_id = THERMOSTAT_COMMAND_1;
  lastACCommand.can_dlc = 8;      //off 1  0 64 64 25 99 25  0
  lastACCommand.data[0] = 1;
  lastACCommand.data[1] = 0;
  lastACCommand.data[2] = 0x64;
  lastACCommand.data[3] = 0x64;
  lastACCommand.data[4] = 0x25;
  lastACCommand.data[5] = 0x99;
  lastACCommand.data[6] = 0x25;
  lastACCommand.data[7] = 0;
  
    

  //PDM
  for (int i = 0;i<=12;i++)
  {
    pdm1_output[i].fFeedback = 0;
    pdm1_output[i].bSoftStartStepSize = 0;
    pdm1_output[i].bMotorOrLamp = 0;
    pdm1_output[i].bLossOfCommunication = 0;
    pdm1_output[i].bByte7 = 0;
    pdm1_output[i].bByte8 = 0;
    pdm1_output[i].bCommand = 0;
    
    pdm2_output[i].fFeedback = 0;
    pdm2_output[i].bSoftStartStepSize = 0;
    pdm2_output[i].bMotorOrLamp = 0;
    pdm2_output[i].bLossOfCommunication = 0;
    pdm2_output[i].bByte7 = 0;
    pdm2_output[i].bByte8 = 0;   
    pdm2_output[i].bCommand = 0;
  }
  strcpy(pdm1_output[1].szName,"SOLAR_BACKUP");
  strcpy(pdm1_output[2].szName,"CARGO_LIGHTS");
  strcpy(pdm1_output[3].szName,"READING_LIGHT");
  strcpy(pdm1_output[4].szName,"CABIN_LIGHTS");
  strcpy(pdm1_output[5].szName,"AWNING_LIGHTS");
  strcpy(pdm1_output[6].szName,"RECIRC_PUMP");
  strcpy(pdm1_output[7].szName,"AWNING_ENABLE");
  strcpy(pdm1_output[8].szName,"PDM_1_8");
  strcpy(pdm1_output[9].szName,"PDM_1_9");
  strcpy(pdm1_output[10].szName,"EXHAUST_FAN");
  strcpy(pdm1_output[11].szName,"FURNACE_POWER");
  strcpy(pdm1_output[12].szName,"WATER_PUMP");

  strcpy(pdm2_output[1].szName,"PDM_2_1");
  strcpy(pdm2_output[2].szName,"GALLEY_FAN");
  strcpy(pdm2_output[3].szName,"REGRIGERATOR");
  strcpy(pdm2_output[4].szName,"12V_USB");
  strcpy(pdm2_output[5].szName,"AWNING_M_P");
  strcpy(pdm2_output[6].szName,"AWNING_M_M");
  strcpy(pdm2_output[7].szName,"TANK_MON_PWR");
  strcpy(pdm2_output[8].szName,"POWER_SW");
  strcpy(pdm2_output[9].szName,"HVAC_POWER");
  strcpy(pdm2_output[10].szName,"12V_SPEAKER");
  strcpy(pdm2_output[11].szName,"SINK_PUMP");
  strcpy(pdm2_output[12].szName,"AUX_POWER");

  //Roof Fan
  roofFan.nSystemStatus = 0;
  roofFan.nFanMode = 0;
  roofFan.nSpeedMode = 0;
  roofFan.nLight = 0;
  roofFan.nSpeed = 0;
  roofFan.nWindDirection = 0;
  roofFan.nDomePosition = 0;
  roofFan.fSetPoint = 0;
  roofFan.fAmbientTemp = 0;
}




float CM::cToF(float fDeg)
{
  return (fDeg * 9.0 / 5.0  + 32.0);
}


void CM::handleCabinBlink (bool bInit)
{

  static int nBlinkState = 0;
  if (bInit) nBlinkState = 1;
  static long startMillis = 0;
  
  
  if (nBlinkState == 1)
  {
    
    pressdigitalbutton(lastPDM1inputs1to6,6,0);
    startMillis = millis();
    nBlinkState = 2;
    return;
  }
  if (nBlinkState == 2)
  {
    if (millis() - startMillis < 500) return;
    
    pressdigitalbutton(lastPDM1inputs1to6,6,0);
    nBlinkState = 0;
    
  }
}



void CM::pressdigitalbutton (can_frame mLast,int nDataIndex,int nByteNum)
{
  if (mLast.can_dlc == 0)
  {
    Serial.println("ERROR-- no old data");
    printCan(mLast);
    return;
  }
  
  if (bVerbose)
  {
    Serial.print("Last: ");printCan(mLast);
    Serial.print("nDataIndex = ");Serial.println(nDataIndex);
    Serial.print("nByteNum = ");Serial.println(nByteNum);
  }
  
  can_frame m = mLast;
  byte andMask = 0;
  
  if (nByteNum == 3) andMask = 0b00111111;
  if (nByteNum == 2) andMask = 0b11001111;
  if (nByteNum == 1) andMask = 0b11110011;
  if (nByteNum == 0) andMask = 0b11111100;
  byte orMask = (0b10 << (nByteNum * 2));
  if (bVerbose) Serial.print("andMask: ");
  if (bVerbose) Serial.println(andMask,BIN);
  if (bVerbose) Serial.print("orMask: ");
  if (bVerbose) Serial.println(orMask,BIN);
  
  m.data[nDataIndex] = (m.data[nDataIndex] & andMask) | orMask;
  if (bVerbose) printCan(m);
  mPtr->sendMessage(&m);
  delay(100);
  
  m.data[nDataIndex] = m.data[nDataIndex] & andMask;
  mPtr->sendMessage(&m);
  //delay(100);
  
  }
void CM::getHeaterInfo(char *buffer)
{
  StaticJsonDocument<256> doc;
  doc["bFanOn"] = heater.bFanOn;
  doc["nFanSpeed"] = heater.nFanSpeed;
  doc["bAirFlowOut"] = heater.bAirFlowOut;
  doc["bAutoMode"] = heater.bAutoMode;
  doc["fTargetTemp"] = cToF(heater.fTargetTemp);
  serializeJson (doc,buffer,250);
  //char sz1[32];data2Json(sz1,"bFanOn",heater.bFanOn);
  //char sz2[32];data2Json(sz2,"nFanSpeed",heater.nFanSpeed);
  //char sz3[32];data2Json(sz3,"bAirFlowOut",heater.bAirFlowOut);
  //char sz4[32];data2Json(sz4,"bAutoMode",heater.bAutoMode);
  //char sz5[32];data2Json(sz5,"fTargetTemp",cToF(heater.fTargetTemp));
  
  //sprintf(buffer,"{%s,%s,%s,%s,%s}",sz1,sz2,sz3,sz4,sz5);
  
}


void CM::getACInfo(char *buffer)
{
  StaticJsonDocument<256> doc;
  doc["bCompressor"] = ac.bCompressor;
  doc["bFan"] = ac.bFan;
  doc["fanMode"] = ac.fanMode;
  doc["setpointCool"] = ac.fSetpointCool;
  serializeJson (doc,buffer,128);
  
  
  
  
  //sprintf(buffer,"{%s,%s,%s,%s,%s}",sz1,sz2,sz3,sz4,sz5);
  
}
void CM::getTankInfo(char *buffer)
{
  StaticJsonDocument<128> doc;
  doc["nFreshTankLevel"] = nFreshTankLevel;
  doc["nGrayTankLevel"] = nGrayTankLevel;
  serializeJson (doc,buffer,64);
  
  //char sz1[32];data2Json(sz1,"nFreshTankLevel",nFreshTankLevel);
  //char sz2[32];data2Json(sz2,"nGrayTankLevel",nGrayTankLevel);
  //sprintf(buffer,"{%s,%s}",sz1,sz2);
}

void CM::getMiscInfo(char *buffer)
{
  StaticJsonDocument<128> doc;
  
  doc["fAmbientTemp"] = cToF(fAmbientTemp);
  doc["nVentSpeed"] = roofFan.nSpeed;
  serializeJson (doc,buffer,64);
  
  
 
 
}


void CM::getAllInfo(char *buffer)
{
  
   
  char heaterInfo[300];getHeaterInfo(heaterInfo);
  char acInfo[300];getACInfo(acInfo);
  char tankInfo[160];getTankInfo(tankInfo);
  char miscInfo[64];getMiscInfo(miscInfo);
  
  sprintf(buffer,"{\"heater\":%s,\"acInfo\":%s,\"tankInfo\":%s,\"miscInfo\":%s}",heaterInfo,acInfo,tankInfo,miscInfo);
  
}




float CM::byte2Float(byte b)
{
  float sign = 1;
  if ((b >> 7) == 0x01) sign = -1;
  b = b & 0x7F;
  return ((float)b * 0.7874015748 * sign);
}

void CM::printCan (can_frame m,bool bLF)
{
  
   if (m.can_id == PDM1_COMMAND)                    Serial.print("PDM1_CMD ");
   if (m.can_id == PDM2_COMMAND)                    Serial.print("PDM2_CMD ");
   if (m.can_id == PDM1_MESSAGE)                    Serial.print("PDM1_MSG ");
   if (m.can_id == PDM2_MESSAGE)                    Serial.print("PDM2_MSG ");
   if (m.can_id == TANK_LEVEL)                      Serial.print("TANK_LVL ");
   if (m.can_id == RIXENS_COMMAND)                  Serial.print("RIXN_CMD ");
   if (m.can_id == THERMOSTAT_AMBIENT_STATUS)       Serial.print("THRM_STS ");
   if (m.can_id == THERMOSTAT_STATUS_1)             Serial.print("THRM_ST1 ");
   if (m.can_id == ROOFFAN_STATUS)                  Serial.print("RFFN_STS ");
   if (m.can_id == ROOFFAN_CONTROL)                 Serial.print("RFFN_CTL");
   if (m.can_id == PDM1_SHORT)                      Serial.print("PDM1_SHT ");
   if (m.can_id == PDM2_SHORT)                      Serial.print("PDM2_SHT ");
   if (m.can_id == THERMOSTAT_COMMAND_1)            Serial.print("THRM_CTL ");
   
  Serial.print(m.can_id,HEX);Serial.print(" ");
  Serial.print(m.can_dlc);Serial.print(" ");
  for (int i = 0;i<m.can_dlc;i++)
  {
    if (m.data[i] < 0x10) Serial.print(" ");
    Serial.print(m.data[i],HEX);Serial.print("   ");
  }
  if (bLF) Serial.println();
}



///DIAGNOSTICS
void CM::handleDiagnostics (can_frame m)
{
  
  byte s1 = m.data[0] & 0x03;
  byte s2 = (m.data[0] >> 2) & 0x03;
  if (bVerbose) Serial.print(" s1-2: ");
  if ((s1 == 1) && (s2 == 1)) if (bVerbose) Serial.print("ok");
  else
  {
    if (bVerbose) 
    {
      Serial.print(s1);
      Serial.print(s2);
    }
    
  }
  if (bVerbose) 
  {
    Serial.print(" yel: ");
    Serial.print((m.data[0] >> 4) & 0x03);
    Serial.print(" red: ");
    Serial.print((m.data[0] >> 6) & 0x03);
    Serial.print(" DSA: ");
    Serial.print(m.data[1],HEX);
  }
  
  
  long spn1 = m.data[2] << 11 + m.data[3] << 3 + (m.data[4] & 0xe0) >> 5;
  long spn2 = m.data[2] << 3 + (m.data[4] & 0xe0) >> 5;
  byte instance = m.data[3];
   
  if (bVerbose) 
  {
    Serial.print(" SPN: ");
    Serial.print(spn1,HEX);
    Serial.print("/");
    Serial.print(spn2,HEX);
    Serial.print("-");
    Serial.print(instance,HEX);
    Serial.print(" FMI: ");
    Serial.print(m.data[4] & 0xf,HEX);
    Serial.print(" OCC #");
  }
  
  if (m.data[5] & 0x7f == 0x7f) 
  {
    if (bVerbose) Serial.print("n/a");
  }
  else if (bVerbose) Serial.print(m.data[5] & 0x7f,HEX);
  if (bVerbose) 
  {
    Serial.print(" DSA_Ext ");
    Serial.print(m.data[6],HEX);
  }
  
  if (bVerbose) Serial.println();
  

  
}
float CM::bytes2DegreesC(byte b1,byte b2)
{
  return ((int)b1 + (int)b2 * 0x100) * 0.03125 - 273.0;
}

////AC Controls
void CM::handleThermostatStatus(can_frame m)
{
  ac.operatingMode = m.data[1] & 0xf;
  ac.fanMode = (m.data[1] >> 4) & 0x3;
  ac.fanSpeed = m.data[2];
  ac.setpointHeat = (int)m.data[3] << 8 + (int)m.data[4];
  ac.setpointCool = ((word)m.data[5] << 8) + m.data[6];
  ac.fSetpointCool = bytes2DegreesC(m.data[5],m.data[6]);
  if (bVerbose) 
  {
    Serial.print("AC Operating Mode: ");Serial.print(ac.operatingMode);
    Serial.print(" fanmode: ");Serial.print(ac.fanMode);
    Serial.print(" fanspeed: ");Serial.print(ac.fanSpeed);
    Serial.print(" sp heat: ");Serial.print(ac.setpointHeat);
    Serial.print(" sp cool: ");Serial.print(ac.setpointCool);
    
    Serial.println();
  }
  
  
}
void CM::setACFanSpeed (byte newSpeed)
{

  Serial.println("AC Speed");
  can_frame m = lastACCommand;
  m.data[2] = newSpeed;
  if (bVerbose) Serial.print("new speed: ");
  if (bVerbose) Serial.println(m.data[2],BIN);
  lastACCommand = m;
  mPtr->sendMessage(&m); 
}

void CM::acCommand (byte bFanMode,byte bOperatingMode,byte bSpeed)
{

  
  Serial.println("AC Mode");
  //fanMode         00 auto
   //               01 always on
                  
  //operatingMode 0000b — Off
//                0001b — Cool
//                0010b — Heat
//                0011b — Auto heat/Cool
//                0100b — Fan only
//                0101b — Aux Heat
//                0110b — Window Defrost/Dehumidify

  //fanSpeed    0-125
  //fSetpointCool
  //fSetPointHot
                  
  struct can_frame m = lastACCommand;
  m.can_id = THERMOSTAT_COMMAND_1;
  m.can_dlc = 8;
  m.data[0] = 1;
  m.data[1] = (bFanMode << 4) + bOperatingMode;
  m.data[2] = bSpeed & 0xff;
  //ac.setpointCool = m.data[5] << 8 + m.data[6];
  
  
  m.data[5] = ac.setpointCool >> 8;
  m.data[6] = ac.setpointCool & 0xff;
  
  
  
  lastACCommand = m;
  mPtr->sendMessage(&m); 
}

//fanMode         00 auto
//                01 always on
void CM::setACFanMode (byte bFanMode)
{
  Serial.println("AC Fan");
  can_frame m = lastACCommand;
  m.data[1] = (m.data[1] & 0b11001111) + (bFanMode << 4);
  if (bVerbose) Serial.print("new fan mode: ");
  if (bVerbose) Serial.println(m.data[1],BIN);
  lastACCommand = m;
  mPtr->sendMessage(&m); 
}



void CM::setACOperatingMode (byte newMode)
{
  Serial.println("AC Mode");
  can_frame m = lastACCommand;
  m.data[1] = (m.data[1] & 0b11110000) + newMode;
  if (bVerbose) Serial.print("new operating mode: ");
  if (bVerbose) Serial.println(m.data[1],BIN);
  lastACCommand = m;
  mPtr->sendMessage(&m); 
}
void CM::acSetTemp (float fTemp)
{
    
   Serial.println("AC Set Temp");
   can_frame m = lastACCommand;
   //convert temp to bytes;
   fTemp = (fTemp + 273.0) / .03125;
   
   int nTemp = (int)fTemp;
   byte b1 = nTemp & 0xff;
   byte b2 = (nTemp & 0xff00) >> 8;
   m.data[5] = b1;
   m.data[6] = b2;
   lastACCommand = m;
   //printCan(m);
   mPtr->sendMessage(&m); 
   
   
}
//////////////TANK


void CM::handleTankLevel(can_frame m)
{
  static int lastFreshTankLevel = -1;
  
  if (m.data[0] == 0x00)
  {



    
    nFreshTankLevel = m.data[1];
    nFreshTankDenom = m.data[2];
    if (bVerbose) 
    {
      Serial.print("FRESH LEVEL: ");
      Serial.print(nFreshTankLevel);
      Serial.print("/");
      Serial.print(nFreshTankDenom);
    }
    

    
  }
  if (m.data[0] == 0x02)
  {
    nGrayTankLevel = m.data[1];
    nGrayTankDenom = m.data[2];
    if (bVerbose) 
    {
      Serial.print("GRAY LEVEL: ");
      Serial.print(nGrayTankLevel);
      Serial.print("/");
      Serial.print(nGrayTankDenom);
    }
  }
  if (bVerbose) Serial.println();
}

void CM::handleRixensReturn (can_frame m)
{
  if (bVerbose) Serial.println();
}
void CM::handleRixensGlycolVoltage(can_frame m)
{
  if (bVerbose) printCan(m,false);
  heater.fGlycolOutletTemp = (m.data[3] * 256 + m.data[2]) / 100.0;
  heater.fGlycolInletTemp = (m.data[1] * 256 + m.data[0]) / 100.0;

  float fVoltage = (m.data[7] * 256 + m.data[6]) / 10.0;
  if (bVerbose)
  {
    Serial.print(" glycolOutlet: ");Serial.print(heater.fGlycolOutletTemp);
    Serial.print(" glycolInlet: ");Serial.print(heater.fGlycolInletTemp);
    Serial.print(" voltage: ");Serial.print(fVoltage);
    Serial.println();
  }
    
}
void CM::handleRixensCommand(can_frame m)
{
  
  if (m.data[0] == 1) // set target temp anytime thermostat is controlled
  {
    heater.fTargetTemp = ((int)m.data[1] + (int)m.data[2] * 256) / 10.0;
    if (bVerbose) Serial.print("RIXENS SET TEMP: ");
    if (bVerbose) Serial.print(heater.fTargetTemp);
    
  }
  if (m.data[0] == 2) // set target temp anytime thermostat is controlled
  {
    
    if (m.data[1] == 0xff) 
    {
      heater.bAutoMode = true;
      if (bVerbose) Serial.print("RIXENS SET FAN SPEED: AUTO");
    }
    else
    {
      heater.bAutoMode = false;
     
      heater.nFanSpeed = ((int)m.data[1] + (int)m.data[2] * 256);
      if (bVerbose) Serial.print("          RIXENS SET FAN SPEED: ");
      if (bVerbose) Serial.print(heater.nFanSpeed);
    }
  }
  if (m.data[0] == 3)                     // Heatsource Furnace
  {
    
    if (m.data[1] == 0x01) 
    {
      heater.bFurnace = true;
      if (bVerbose) Serial.println("RIXENS HEATSOURCE FURNACE ON");
    }
    else
    {
      heater.bFurnace = false;
      if (bVerbose) Serial.print("RIXENS HEATSOURCE FURNACE OFF");
    }
  }
  if (m.data[0] == 0x06)                     // Hot Water Heater Function
  {
    
    if (m.data[1] == 0x01) 
    {
      
      heater.bHotWater = true;
      heater.bFurnace = true;
      if (bVerbose) Serial.print("RIXENS HOT WATER ON");
    }
    else
    {
      heater.bHotWater = false;
      if (bVerbose) Serial.print("RIXENS HOT WATER OFF");
    }
  }
  if (m.data[0] == 0x0c)                     // Hot Water Heater Function
  {
    
    if (bVerbose) Serial.print("???");
    if (bVerbose) Serial.println();
    
  }
  if (bVerbose) Serial.println();
  
}

void CM::handlePDMCommand(can_frame m)
{
  static bool bFirstTime = true;
  static long startMillis = 0;
  if (bFirstTime)
  {
    bFirstTime = false;
    startMillis = millis();
  }
  float fDT = (millis() - startMillis) / 1000;
  byte b0 = m.data[0] & 0x07;
  if (bVerbose)
  {
    Serial.print("B0: ");
    Serial.print(b0,HEX);
  }
  int nPDM = 0;
  if (m.can_id == PDM1_COMMAND) nPDM = 1;
  if (m.can_id == PDM2_COMMAND) nPDM = 2;
  if (bVerbose)
  {
    Serial.print(" PDM=");
    Serial.print(nPDM);
  }
  
  if (b0 == 0) // setup channel:
  {
    Serial.println("Setup Channel");
    Serial.print(fDT);Serial.print("  ");printCan(m,true);
    Serial.println("PDM Config");
    Serial.println("---------");
    Serial.print("PDM: ");Serial.println(nPDM);
    byte bChannel = m.data[1] & 0xf;
    Serial.print("Channel: ");
    Serial.print (bChannel,HEX);Serial.print(" ");
    if (nPDM == 1) Serial.println(pdm1_output[bChannel].szName);
    if (nPDM == 2) Serial.println(pdm2_output[bChannel].szName);
    byte bSoftStartStepSize = m.data[2];
    if (nPDM == 1) pdm1_output[bChannel].bSoftStartStepSize = bSoftStartStepSize;
    if (nPDM == 2) pdm2_output[bChannel].bSoftStartStepSize = bSoftStartStepSize;
    Serial.print ("Soft Start Pct: (0xFF=disabled)");Serial.println(bSoftStartStepSize,HEX);
    
   
    byte bMotorOrLamp = m.data[3];
    Serial.print("Motor Lamp (0=lamp, 1=motor): ");Serial.println(bMotorOrLamp,HEX);
    if (nPDM == 1) pdm1_output[bChannel].bMotorOrLamp = bMotorOrLamp;
    if (nPDM == 2) pdm2_output[bChannel].bMotorOrLamp = bMotorOrLamp;
    
    byte bLossOfCommunication = m.data[4];
    Serial.print("Loss of Comm: ");Serial.println(bLossOfCommunication,HEX);
    if (nPDM == 1) pdm1_output[bChannel].bLossOfCommunication = bLossOfCommunication;
    if (nPDM == 2) pdm2_output[bChannel].bLossOfCommunication = bLossOfCommunication;
    
    
    byte bByte7 = m.data[6];
    if (bVerbose) Serial.print("POR Comm-Enable/Type/Braking: ");
    if (bVerbose) Serial.println(bByte7,BIN);
    if (nPDM == 1) pdm1_output[bChannel].bByte7 = bByte7;
    if (nPDM == 2) pdm2_output[bChannel].bByte7 = bByte7;
    
    
    byte bByte8 = m.data[7];
    if (bVerbose) 
    {
      Serial.print("LSC/CAL/Response: ");
      Serial.println(bByte8,BIN);
    }
    if (nPDM == 1) pdm1_output[bChannel].bByte8 = bByte8;
    if (nPDM == 2) pdm2_output[bChannel].bByte8 = bByte8;
    return;
  }
  if (b0 == 0x04)//1-6
  {
    if (bVerbose) Serial.print("COMMAND ");
    if (nPDM == 1) for (int i = 1;i<=6;i++) pdm1_output[i].bCommand = m.data[i];
    if (nPDM == 2) for (int i = 1;i<=6;i++) pdm2_output[i].bCommand = m.data[i];
    if (bVerbose) Serial.print("0x04 1-6: ");
    if (bVerbose) Serial.println(m.data[7],BIN);
    return;
  }
  if (b0 == 0x05)//7-12
  {
    if (nPDM == 1) for (int i = 7;i<=12;i++) pdm1_output[i].bCommand = m.data[i-6];
    if (nPDM == 2) for (int i = 7;i<=12;i++) pdm2_output[i].bCommand = m.data[i-6];
    if (bVerbose) Serial.print("0x05 7-12: ");
    if (bVerbose) Serial.println(m.data[7],BIN);
    return;
    
  }
  Serial.print("UNKNOWN PDM COMMAND B0-- ");
  Serial.println(m.data[0],HEX);
  

  if (bVerbose) Serial.println();
}
//////////////ROOF FAN

void CM::handleAck (can_frame m)
{
  printCan(m);
  Serial.println();
}
void CM::handleRoofFanStatus (can_frame m)
{
  //dome position 0 = closed
  //                1 = 1/4
  //                2 = 1/2
  //                3 = 3/4
  //                4 = open
  if (bVerbose) Serial.print("RF: :");
  roofFan.nSystemStatus = m.data[1] & 0x3;
  roofFan.nFanMode = (m.data[1] >> 2) & 0x03;
  roofFan.nSpeedMode = (m.data[1] >> 4) & 0x03;
  roofFan.nLight = (m.data[1] >>6) & 0x03;
  roofFan.nSpeed = m.data[2];
  roofFan.nWindDirection = m.data[3] & 0x03;
  roofFan.nDomePosition = (m.data[3] >> 2) & 0xf;// 0 = closed, 1 = 1/4 2 = 1/2, 3 = 3/4, 4 = totally open
  
  
  roofFan.fSetPoint = bytes2DegreesC(m.data[6],m.data[7]);
  
  if (bVerbose) 
  {
    Serial.print("Rain: ");Serial.print(m.data[3] >> 6,HEX); 
    Serial.print(" stat: ");Serial.print(roofFan.nSystemStatus);
    Serial.print(" fanMode: ");Serial.print(roofFan.nFanMode);
    Serial.print(" speedMode: ");Serial.print(roofFan.nSpeedMode);
    Serial.print(" light: ");Serial.print(roofFan.nLight);
    Serial.print(" speed: ");Serial.print(roofFan.nSpeed);
    Serial.print(" dir: ");Serial.print(roofFan.nWindDirection);
    Serial.print(" pps: ");Serial.print(roofFan.nDomePosition);
    
    
    
    
    
    
    
    Serial.println();
  }
}

void CM::openVent() //vent position doesnt work.  alwayds full open or 
//0b0000 = closed
//0b0001 = 1/4
//0b0010 = 1/2
//0b0011 = 3/4
//0b0100 = 4/4
//0b0101 = stop
{
  if (bVerbose) Serial.println("opening vent");
  can_frame m;
  bool bDir = roofFan.nWindDirection & 1;
  m.can_id = ROOFFAN_CONTROL;
  m.can_dlc = 8;
  m.data[0] = 2;
  m.data[1] = 0b10101;//system 1, fan force on, speed mode manual
  m.data[2] = roofFan.nSpeed;
  m.data[3] = 0b01010000 + bDir ; //01-0100-00  rain sensor, open, air out
  //m.data[3] = (m.data[3] & 0b11000011) + (bDomePosition << 2);
  m.data[4] = 0;      //temp
  m.data[5] = 0;
  m.data[6] = 0;      //setpoint
  m.data[7] = 0;
  mPtr->sendMessage(&m);
  delay(100);
  
}

void CM::closeVent()
{
  if (bVerbose) Serial.println("closing vent");
  can_frame m;
  byte bDir = roofFan.nWindDirection & 1;
  m.can_id = ROOFFAN_CONTROL;
  m.can_dlc = 8;
  m.data[0] = 2;
  m.data[1] = 0b10101;//system 1, fan force on, speed mode manual
  m.data[2] = 0;//turn off
  m.data[3] = 0b1000000 +  + bDir;      //01-0000-00
  m.data[4] = 0;
  m.data[5] = 0;
  m.data[6] = 0;
  m.data[7] = 0;
  mPtr->sendMessage(&m);
}
void CM::setVentSpeed (int nSpeed)   //0 - 127
{
  byte bSpeed = nSpeed & 0xff;
  byte bDir = roofFan.nWindDirection & 1;
  Serial.print("setting speed: ");Serial.print(bSpeed);
  roofFan.nSpeed = bSpeed;
  //RV-C p.463 ROOF_FAN_COMMAND
  can_frame m;
  m.can_id = ROOFFAN_CONTROL;
  m.can_dlc = 8;
  m.data[0] = 2;    //instance is always 2
  m.data[1] = 0b10101;//system 1, fan force on, speed mode manual
  m.data[2] = bSpeed;
  if (roofFan.nDomePosition == 0) m.data[3] = 0b01010000 + bDir;   //01-0100-00//open the dome if the fan is off, so the fan doesn't push against a closed dome
  else m.data[3] = 0b01010100 + bDir; //01-0101-00//rain sensor on, "stopped",air out
 
  m.data[4] = 0;
  m.data[5] = 0;
  m.data[6] = 0;
  m.data[7] = 0;
  mPtr->sendMessage(&m);
  
}

void CM::setVentDirection (bool bDir)   //0 = out, 1 = in
{
 
  Serial.print("setting direction: ");Serial.print(bDir);
  
  //RV-C p.463 ROOF_FAN_COMMAND
  roofFan.nWindDirection = bDir;
  can_frame m;
  m.can_id = ROOFFAN_CONTROL;
  m.can_dlc = 8;
  m.data[0] = 2;
  m.data[1] = 0b10101;//system 1, fan force on, speed mode manual
  m.data[2] = roofFan.nSpeed;
  m.data[3] = 0b01010100 + (bDir & 0b1); //01-0101-00//rain sensor on, "stopped",air in(1) out(0)
 
  m.data[4] = 0;
  m.data[5] = 0;
  m.data[6] = 0;
  m.data[7] = 0;
  mPtr->sendMessage(&m);
  
}
void CM::handleRoofFanControl(can_frame m)
{
  bVerbose = true;
  if (bVerbose) 
  {
    
    
    Serial.print ("Instance: ");
    Serial.println(m.data[0]);
    Serial.print("B1: ");Serial.println(m.data[1],BIN);
    Serial.print("Speed: ");Serial.println(m.data[2],HEX);
    Serial.print("b3: ");Serial.println(m.data[3],BIN);
    Serial.print("Temp: ");Serial.println(bytes2DegreesC(m.data[4],m.data[5]));
    Serial.print("Set: ");Serial.println(bytes2DegreesC(m.data[6],m.data[7]));
  
    Serial.println("Roof Fan Control");
  }
   bVerbose = false;
}

void CM::handleThermostatAmbientStatus (can_frame m)
{
  int nInstance = m.data[0];
  fAmbientTemp = bytes2DegreesC(m.data[1],m.data[2]);
  
  if (bVerbose)
  {
    Serial.print("  AMBIENT TEMP: ");
    Serial.println(fAmbientTemp);
  }
}
void CM::handleMiniPump()
{
  
  static long pauseMillis = millis();
  static byte bLastPumpPress = 0;
  if (millis() - pauseMillis < 100) return;
  pauseMillis = millis();
  
  if (bMiniPumpMode == false) return;
  if (lastPDM2inputs1to6.can_dlc == 0) return;
  //get the command state of the pump
  byte bPumpPress = lastPDM2inputs1to6.data[7]  & 0b11000000;
 
  
  if ((bPumpPress == 0) && (bLastPumpPress > 0))
  {
    Serial.println("MiniPump!");
    if (pdm1_output[PDM1_OUT_WATER_PUMP].bCommand > 0) pressdigitalbutton(lastPDM2inputs1to6,7,3);
  }
  bLastPumpPress = bPumpPress;
  
}
void CM::handleSmartSiphon()
{
  static long pauseMillis = millis();
  static byte bLastPumpPress = 0;
  static byte smartSiphonStatus = 0;
  static long millisAtPress = millis();
  static byte bLastPumpCommand = 0;
  static float fAmpsAfter5Seconds = 0;
  if (millis() - pauseMillis < 100) return;
  
  pauseMillis = millis();

  
  if (bSmartSiphonMode == false) return;
  if (lastPDM2inputs1to6.can_dlc == 0) return;

  //if the pump if off, return;
  if (pdm1_output[PDM1_OUT_WATER_PUMP].fFeedback == 0)
  {
    Serial.println("feedback = 0");
    smartSiphonStatus = 0;
  }
  if ((pdm1_output[PDM1_OUT_WATER_PUMP].bCommand == 0) && (bLastPumpCommand > 0))
  {
    Serial.println("Pump is turned off");
    smartSiphonStatus = 0;
    bLastPumpCommand = pdm1_output[PDM1_OUT_WATER_PUMP].bCommand;
    return;
    
  }
  bLastPumpCommand = pdm1_output[PDM1_OUT_WATER_PUMP].bCommand;
  //get the command state of the pump
  byte bPumpPress = lastPDM2inputs1to6.data[7]  & 0b11000000;
 
  
  if ((smartSiphonStatus == 0) && (bPumpPress > 0) && (bLastPumpPress == 0)) //the user just pressed the button
  {
    Serial.println("Button Pressed");
    smartSiphonStatus = 1;
    millisAtPress = millis();
   
    
  }
  bLastPumpPress = bPumpPress;
  if ((smartSiphonStatus == 1) && (millis() - millisAtPress > 5000))
  {
    Serial.println("5 seconds!");
    smartSiphonStatus = 2;
    fAmpsAfter5Seconds = pdm1_output[PDM1_OUT_WATER_PUMP].fFeedback;
    Serial.println(fAmpsAfter5Seconds);
    
  }
  if (smartSiphonStatus == 2)

  {
    if (fAmpsAfter5Seconds - pdm1_output[PDM1_OUT_WATER_PUMP].fFeedback > 2)
    {
      Serial.println("DRY!!");
      //if (pdm1_output[PDM1_OUT_WATER_PUMP].bCommand > 0) pressdigitalbutton(lastPDM2inputs1to6,7,3);
      smartSiphonStatus = 0;
      
    }
  }
  
  
}
void CM::handleDrinkBlink(bool bBlinkMode)
{
  static bool bActive = false;
  if (bBlinkMode == 1) bActive = true;
  if (bBlinkMode == -1) bActive = false;
  if (!bActive) return;
  if (nFreshTankLevel == -1) return;
  static int lastFreshTankLevel = nFreshTankLevel;
  
  if (lastFreshTankLevel != nFreshTankLevel) 
  {
    handleCabinBlink(true);//true means init
    Serial.println("**BLINK**");
    
  }
  lastFreshTankLevel = nFreshTankLevel;
}

void CM::handleMinutePump(int nInitVal)
{
  static long pumpOnMillis = 0;
  static byte bLastPumpPress = 0;
  static int nMinutePumpStatus = 0;
  static int nWaitMins = 1;
  static bool bActive = false;

  byte bPumpPress;
  if (nInitVal ==  -1) bActive = false;
  
  if (nInitVal > 0) 
  {
    nWaitMins = nInitVal;
    bActive = true;
    Serial.println("Minute Pump Active");
  }
  if (bActive)
  {
    bPumpPress = lastPDM2inputs1to6.data[7]  & 0b11000000;
    if ((nMinutePumpStatus == 0) && (bPumpPress > 0) && (bLastPumpPress == 0)) //the user just pressed the button
    {
      Serial.println("Minute Pump Starts");
      nMinutePumpStatus = 1;
      pumpOnMillis = millis();  
    }
    long lWaitMillis = 60000 * nWaitMins;
    if ((nMinutePumpStatus == 1) && (millis() - pumpOnMillis > lWaitMillis))
    {
      Serial.println("Pump Off!");
      if (pdm1_output[PDM1_OUT_WATER_PUMP].bCommand > 0) pressdigitalbutton(lastPDM2inputs1to6,7,3);
      nMinutePumpStatus = 0;
      bActive = false;
    }
  }
  bLastPumpPress = bPumpPress;
}
  
  

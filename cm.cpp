#include "cm.h"
void CM::init(MCP2515* thisPtr)
{

  szMessage = "You stay classy, San Diego";
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
  roofFan.bRainSensor = 0;
}




float CM::cToF(float fDeg)
{
  return (fDeg * 9.0 / 5.0  + 32.0);
}


void CM::handleCabinBlink (int nInit)
{
  static int nBlinksLeft = 0;
  static int nBlinkState = 0;
  if (nInit > 0) 
  {
    nBlinkState = 1;
    nBlinksLeft = nInit * 2 - 1;
  }
  static long startMillis = 0;
  
  
  
  if (nBlinkState == 1)
  {
    
    pressCabin();
    startMillis = millis();
    nBlinkState = 2;
    return;
  }
  if (nBlinkState == 2)
  {
    if (millis() - startMillis < 500) return;
    
    pressCabin();
    nBlinksLeft--;
    if (nBlinksLeft <= 0) nBlinkState = 0;
    else 
    {
      nBlinkState = 2;
      startMillis = millis();
    }
    
  }
}

void CM::acOff()
{
  acCommand(0,0,0);
}
void CM::handleEngineOnAllOff(can_frame m)
{
  static byte lastB6 = 10;
  if (m.can_id != PDM1_MESSAGE) return;
  if (m.data[0] != 0xf8) return;
  byte b6 = m.data[6];
  //if (b6 == 0x20) Serial.print("!");else Serial.print(" ");
  
  //printCan (m,false);Serial.print("B6: ");Serial.println(b6,HEX);
  if ((b6 == 0x20) and (lastB6 != 0x20))
  {
    Serial.println("ENGINE JUST TURNED ON!!");
    acOff();
    closeVent();

    cabinOff();
    delay(500);
    cargoOff();
    delay(500);
    awningOff();
    delay(500);
    
    circOff();
    delay(500);
    pumpOff();
    delay(500);
    

    
    pressAwningEnable();
    pressAwningIn();
    
   
  }
  lastB6 = b6;
  return;
  
  
}

void CM::printBin(byte aByte) 
{
  for (int8_t aBit = 7; aBit >= 0; aBit--)
  Serial.write(bitRead(aByte, aBit) ? '1' : '0');
}
void CM::handleFrontButtonDoubleTap(can_frame m)
{
  static long tap1mSec = millis();
  static int tapState = 0;
  if (m.can_id != PDM2_MESSAGE) return;
  if (m.data[0] != 0xf0) return;
  byte b7 = m.data[7];
  
  if ((millis() - tap1mSec) > 10000) 
  {
    tapState = 0;
  }
  if (tapState == 0)
  {
    if ((b7 & 0b00110000) == 0x20)
    {
      Serial.println("TAP");
      tap1mSec = millis();
      tapState = 1;               //single press;
    }
    
  }
  if (tapState == 1)
  {
    if ((b7 & 0b00110000) == 0x00)
    {
      Serial.println("Release");
      tap1mSec = millis();
      tapState = 2;               //single press;
    }
  }
  if (tapState == 2)
  {
    if ((b7 & 0b00110000) == 0x20)
    {
      Serial.println("Second Tap");
      tapState = 3; 
    }
  }
  if (tapState == 3)
  {
    if ((b7 & 0b00110000) == 0x20)
    {
      
      Serial.println("Second Release");
      tapState = 4;
      
    }
    
    
  }
  if (tapState == 4)
  {
    Serial.println("Waiting");
    if ((millis() - tap1mSec) > 1000)
    {
      acOff();
      delay(500);
      closeVent();
      delay(500);
      cabinOff();
      delay(500);
      cargoOff();
      delay(500);
      awningOff();
      delay(500);
      
      circOff();
      delay(500);
      pumpOff();
      delay(500);
      pressAwningEnable();
      pressAwningIn();
      acOff();
      
      
      tapState = 0;
    }
  }
  
}
void CM::handlePDMDigitalInput (float t,can_frame m)         //This is message payload contains ambient voltage and digital inputs
{
  
  if (m.can_id == PDM1_MESSAGE)
  {
    handleEngineOnAllOff(m);
    if (bVerbose) Serial.print("PDM1 ");
    if (m.data[0] == 0xf0) 
    {
      lastPDM1inputs1to6 = m;
      if (bVerbose) Serial.print(" Button Saved 1-6 ");
    }
    
    if (m.data[0] == 0xf8) 
    {
      lastPDM1inputs7to12 = m;
      if (bVerbose) Serial.print(" Button Saved 7-12 ");
    }
    if (bVerbose) 
    {
      
      printBin(m.data[6]);
      Serial.print(" ");
      printBin(m.data[7]);
    }
  }
  if (m.can_id == PDM2_MESSAGE)
  {
    handleFrontButtonDoubleTap(m);
    
    if (bVerbose) Serial.print("PDM2 ");
    if (m.data[0] == 0xf0) 
    {
      lastPDM2inputs1to6 = m;
      
    }
    if (m.data[0] == 0xf8) 
    {
      lastPDM2inputs7to12 = m;
      if (bVerbose) Serial.print(" 7-12 ");
    }
    if (bVerbose) 
    {
      Serial.print("BUTTON.  ");
      printBin(m.data[6]);
      Serial.print(" ");
      printBin(m.data[7]);
      
    }
    
  }

    
  
    
  float fTemp = (m.data[4] & 0b11) * 256.0 + m.data[5];
  fAmbientVoltage = fTemp * 5.0 / 1024.0;
  if (bVerbose) Serial.print(" ");
  if (bVerbose) Serial.print(fAmbientVoltage);
  if (bVerbose) Serial.println("V");
    
}
void CM::handlePDMShort (can_frame m)
{
  
  if ((m.data[2] != 0) || (m.data[3] != 0))
  {
    Serial.print("***PDM SHORT FIRED!*** ");
    printCan(m,false);
    
  }
  if (bVerbose) Serial.println();
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


////PDM MESSAGES.  THIS IS A BIT OF , WELL, A SUCKFEST

void CM::handleHeartBeat (can_frame m)
{
    if (bVerbose) Serial.println("PDM ID");
    return; 
}

void CM::handleMessage134 (float t,can_frame m)
{
    if (bVerbose) Serial.print(" 134) ");
    if (bVerbose) Serial.print("channel: ");
    if (bVerbose) Serial.print(m.data[1] & 0b1111);
    if (bVerbose) Serial.println();
}
void CM::handleMessage135 (float t,can_frame m)
{
  if (bVerbose) Serial.print(" 135) ");
  if (bVerbose) Serial.println();
   
}
void CM::handleMessage136 (float t,can_frame m)
{
  if (bVerbose) Serial.print(" 136) ");
  if (bVerbose) Serial.println();
}

float CM::feedbackAmps(can_frame m,int nChannelNumber)
{
  int nByteOffset;
  if ((nChannelNumber == 1) || (nChannelNumber == 7)) nByteOffset = 2;
  if ((nChannelNumber == 2) || (nChannelNumber == 8)) nByteOffset = 3;
  if ((nChannelNumber == 3) || (nChannelNumber == 9)) nByteOffset = 4;
  if ((nChannelNumber == 4) || (nChannelNumber == 10)) nByteOffset = 5;
  if ((nChannelNumber == 5) || (nChannelNumber == 11)) nByteOffset = 6;
  if ((nChannelNumber == 6) || (nChannelNumber == 12)) nByteOffset = 7;
  float f = (float)m.data[nByteOffset] * 0.125;
  return f;
  
}

 void CM::handleFeedback1to6 (float t,can_frame m)            //PDM1 Feedback Amps
 {
  if (m.can_id == PDM1_MESSAGE)
  { 
    if (bVerbose) Serial.println("Feedback PDM1 1-6");
    for (int i = 1;i<=6;i++) pdm1_output[i].fFeedback = feedbackAmps(m,i);
    
    
  }
  if (m.can_id == PDM2_MESSAGE)
  {
    if (bVerbose) Serial.println("Feedback PDM2 1-6");
    for (int i = 1;i<=6;i++) pdm2_output[i].fFeedback = feedbackAmps(m,i);
    
    
  }

 }
 
void CM::handleFeedback7to12 (float t,can_frame m)
{
  
  if (m.can_id == PDM1_MESSAGE)
  {
    if (bVerbose) Serial.println("Feedback PDM1 7-12");
    for (int i = 7;i<= 12;i++) pdm1_output[i].fFeedback = feedbackAmps(m,i);
    
  }
  if (m.can_id == PDM2_MESSAGE)
  {
    if (bVerbose) Serial.println("Feedback PDM2 7-12");
    for (int i = 7;i<=12;i++) pdm2_output[i].fFeedback = feedbackAmps(m,i);
    
  }
  
    
}

void CM::handleSupplyVoltage (can_frame m)
{
  float fTemp = m.data[7] * 256 + m.data[6];
  if (bVerbose) 
  {
   
    Serial.print("Battery Supply Voltage: ");
    Serial.print(fTemp / 256.0);
    Serial.println("V");
  }
  return;
}

void CM::handlePDMMessage (float t,can_frame m)
{
  
  byte m0 = m.data[0];
  if ((m0 == 0xf0) || (m0 == 0xf8))                       // BUTTON PRESS!
  {
    handlePDMDigitalInput(t,m);
    return;
  }
  if (m0 == 0xFC)                                    //134 (86h) Motor Model Handshake
  {
    handleMessage134(t,m);                          
    return;
  }

  if (m0 == 0xFD)                                    //129 (81h) Analog Inputs 3-4, Output Diagnostics
  {
    //handleMessage129(t,m);                          
    return;
  }
 
  
  if ((m0 == 0xF9) || (m0 == 0xC9) || (m0 == 0x39))                   //F9 and C9 both seem to do the same thing
  {
    handleFeedback1to6(t,m);                            //Output feedback.  One of the bytes seems to become 1
    return;                                             //94EF111E           
  }
  if ((m0 == 0x0a) || (m0 == 0xCA) || (m0 == 0xFA))
  {
    handleFeedback7to12(t,m);                            //Output feedback.  One of the bytes seems to become 1
    return;                                             //94EF111E           
  }
  if (m0 == 0xfb) 
  {
    handleSupplyVoltage(m);
    return;
    
  }
  if (m0 == 0xfe)
  {
    handleHeartBeat(m);
    return;
  }
  printCan(m,false);
  Serial.println ("? PDM Message: ");
  
  
  
  
  
  return;
  
}
void CM::printInputDiagnostics(byte b)
{
  /*
   * 00 No faults
01 Short-circuit
10 Over-current
11 Open-circuit
   */
  for (int i = 0; i <=3; i++)
  {
    byte bTemp = (b & 0b11000000) >>6;
    if (bTemp == 0) Serial.print("NF ");
    if (bTemp == 1) Serial.print("SC ");
    if (bTemp == 2) Serial.print("OC ");
    if (bTemp == 3) Serial.print("OP ");
    b = b << 2;
  }
}


float CM::tenBitAnalog(can_frame m,int channelNumber)
{
  int nByteOffset = 0;
  if ((channelNumber == 1) || (channelNumber == 3) || (channelNumber == 5) || (channelNumber == 7)) nByteOffset = 4;
  if ((channelNumber == 2) || (channelNumber == 4) || (channelNumber == 6) || (channelNumber == 8)) nByteOffset = 6;
  
   
  long l = m.data[nByteOffset] + ((m.data[nByteOffset + 1] & 0b00000011) << 8);
  float fRet = (float)l * 0.00488759;
  return fRet;
}
char CM::getDigitalInput (can_frame m,int nInputNumber)
{
  int nByteOffset = 1 + ((nInputNumber-1) >> 2);
  byte bRet;
  if ((nInputNumber == 1) || (nInputNumber == 5) || (nInputNumber == 9)) bRet = (m.data[nByteOffset]  & 0b11000000) >> 6;
  if ((nInputNumber == 2) || (nInputNumber == 6) || (nInputNumber == 10)) bRet = (m.data[nByteOffset] & 0b00110000) >> 4;
  if ((nInputNumber == 3) || (nInputNumber == 7) || (nInputNumber == 11)) bRet = (m.data[nByteOffset] & 0b00001100) >> 2;
  if ((nInputNumber == 4) || (nInputNumber == 8) || (nInputNumber == 12)) bRet = (m.data[nByteOffset] & 0b00000011) >> 0;
  bRet = bRet & 0b11;
  if (bRet == 0b00) return ('o');
  if (bRet == 0b01) return ('-');
  if (bRet == 0b10) return ('+');
  if (bRet == 0b11) return ('?');
  
}


void CM::handleMessage128 (float t,can_frame m)
{
  Serial.print("128) D 1-12, Anlg 1-2 ");
  for (int i = 1;i<=12;i++) 
  {
    char ch = getDigitalInput(m,i);Serial.print(ch);Serial.print(" ");
  }
  if (bVerbose) Serial.print(tenBitAnalog(m,1));
  if (bVerbose) Serial.print("V ");
  if (bVerbose) Serial.print(tenBitAnalog(m,2));
  if (bVerbose) Serial.print("V ");
  if (bVerbose) Serial.println();
}
void CM::handleMessage129 (float t,can_frame m)
{
   
  if (bVerbose) Serial.print(" 129) Outpt Diags, Anlg 3-4 ");
  if (bVerbose) printInputDiagnostics(m.data[1]);
  if (bVerbose) Serial.print(" ");
  if (bVerbose) printInputDiagnostics(m.data[2]);
  if (bVerbose) Serial.print(" ");
  if (bVerbose) printInputDiagnostics(m.data[3]);
  
  if (bVerbose) Serial.print(" ");
  if (bVerbose) Serial.print(tenBitAnalog(m,3));
  if (bVerbose) Serial.print("V ");
  if (bVerbose) Serial.print(tenBitAnalog(m,4));
  if (bVerbose) Serial.print("V ");
  if (bVerbose) Serial.println();
}
void CM::handleMessage130 (float t,can_frame m)
{
  Serial.print(" 130) Sensor Status, Anlg 5-6 ");
  if ((m.data[1] & 1) == 1) Serial.print("!+ ");else Serial.print("   ");
  if ((m.data[1] & 2) == 2) Serial.print("!- ");else Serial.print("   ");
  float v = m.data[2] + ((m.data[3] & 0b00000011) << 8);
  v = v * 64.0/1024.0;
  Serial.print("Supply: ");Serial.print(v);Serial.print("v ");
  Serial.print(tenBitAnalog(m,5));Serial.print("V ");
  Serial.print(tenBitAnalog(m,6));Serial.print("V ");
  Serial.println();
}
void CM::handleMessage131 (float t,can_frame m)
{
   
  Serial.print(" 131) Outpt Diags, Anlg 3-4 ");
  printInputDiagnostics(m.data[1]);
  Serial.print(" ");
  printInputDiagnostics(m.data[2]);
  Serial.print(" ");
  printInputDiagnostics(m.data[3]);
  
  Serial.print(" ");
  Serial.print(tenBitAnalog(m,3));
  Serial.print("V ");
  Serial.print(tenBitAnalog(m,4));
  Serial.print("V ");
  Serial.println();
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
  roofFan.bRainSensor = m.data[3] >> 6;
  
  roofFan.fSetPoint = bytes2DegreesC(m.data[6],m.data[7]);
  
  if (bVerbose) 
  {
    Serial.print("Rain: ");Serial.print(roofFan.bRainSensor,HEX); 
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
    pumpOff();
  }
  bLastPumpPress = bPumpPress;
  
}
void CM::handleSmartSiphon(int nInitVal)
{
  static int nMode = 0;
  static int nCurrentamps = 0;
  static float fThresholdAmps = 0;
  static byte lastPumpCommand = 0;
  static long fiveSecondsAfterStart;
  //1 = init.  waiting for pump on
  //2 = pump is now on.  Waiting 5 seconds
  //3 = 5 seconds completed get amps
  
  
  if (nInitVal == 1)
  {
    Serial.println("Smart Siphon started");
    nMode = 1;
    lastPumpCommand = *bPtrPumpCommand;
  } 
  if (nInitVal < 0)
  {
    Serial.println("Smart Siphon stopped");
    nMode = 0;
    
  }
  
  if (nMode == 1)
  {
    if ((*bPtrPumpCommand > 0) && (lastPumpCommand == 0)) 
    {
      Serial.println("Pump has started");
      fiveSecondsAfterStart = millis() + 10000;
      nMode = 2;
      
    }
     
  }
  if (nMode == 2)
  {
    if ((*bPtrPumpCommand == 0) && (lastPumpCommand > 0))
    {
      Serial.println("User turned pump off");
      nMode = 1;
      
    }
    if (millis() > fiveSecondsAfterStart)
    {
      Serial.println("current Amps: ");
      fThresholdAmps = pdm1_output[PDM1_OUT_WATER_PUMP].fFeedback;
      Serial.println(fThresholdAmps);
      nMode = 3;
    }
    
  }
    
    
  
  if (nMode == 3)
  {
    float fAmps = pdm1_output[PDM1_OUT_WATER_PUMP].fFeedback;
    if (fAmps < fThresholdAmps - 1) 
    {
      pumpOff();
      nMode = 1;
      Serial.println("Pump Off");
    }
    
  }
  lastPumpCommand = *bPtrPumpCommand;
  
  
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
    handleCabinBlink(1);//true means init
    Serial.println("**BLINK**");
    
  }
  lastFreshTankLevel = nFreshTankLevel;
}

void CM::pressAwningEnable()
{
  pressdigitalbutton(lastPDM1inputs7to12,6,3);
}
void CM::pumpOff ()
{
  if (*bPtrPumpCommand > 0) pressdigitalbutton(lastPDM2inputs1to6,7,3);
}

void CM::circOff()
{
  if (pdm1_output[PDM1_OUT_RECIRC_PUMP].fFeedback > 0) pressCirc();
}

void CM::awningOff()
{
  if (pdm1_output[PDM1_OUT_AWNING_LIGHTS].fFeedback > 0) pressAwning();
}
void CM::cargoOff()
{
  if (pdm1_output[PDM1_OUT_CARGO_LIGHTS].bCommand > 0) pressCargo();
}
void CM::cabinOff()
{
  if (pdm1_output[PDM1_OUT_CABIN_LIGHTS].fFeedback > 0) pressCabin();
}
void CM::pressCabin()
{
  pressdigitalbutton(lastPDM1inputs1to6,6,0);
}

void CM::pressCargo()
{
  pressdigitalbutton(lastPDM1inputs1to6,6,1);
}

void CM::pressAwning()
{
  pressdigitalbutton(lastPDM1inputs1to6,7,3);
}
void CM::pressCirc()
{
  pressdigitalbutton(lastPDM1inputs1to6,7,2);
}
void CM::pressPump ()
{
  pressdigitalbutton(lastPDM2inputs1to6,7,3);
}

void CM::pressAwningIn ()
{
  pressdigitalbutton(lastPDM1inputs7to12,7,3);
}

void CM::pressAwningOut ()
{
  pressdigitalbutton(lastPDM1inputs7to12,7,2);
}

void CM::pressAux()
{
  pressdigitalbutton(lastPDM2inputs1to6,6,0);
}
void CM::pressDrain()
{
  pressdigitalbutton(lastPDM2inputs7to12,6,1);
}
void CM::auxOff()
{
   if (pdm2_output[PDM2_OUT_AUX_POWER].bCommand > 0) pressAux();
}
void CM::cabinOn()
{
    if (pdm1_output[PDM1_OUT_CABIN_LIGHTS].bCommand == 0) pressCabin(); 
}
void CM::cargoOn()
{
  if (pdm1_output[PDM1_OUT_CARGO_LIGHTS].bCommand == 0) pressCargo();
}
void CM::awningOn()
{
  if (pdm1_output[PDM1_OUT_AWNING_LIGHTS].bCommand == 0) pressAwning();
}
void CM::auxOn()
{
  if (pdm2_output[PDM2_OUT_AUX_POWER].bCommand == 0) pressAux();
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
      pumpOff();
      nMinutePumpStatus = 0;
      bActive = false;
    }
  }
  bLastPumpPress = bPumpPress;
}
void CM::handleShowerJerk(int nInitVal)
{
  static int nStatus = 0;
  static int nFinalTankLevel = -1;
  if (nFreshTankLevel == -1) return;  //case where tank is not initialized
  if (nInitVal > 0)
  {
    
    nFinalTankLevel = max(0,nFreshTankLevel - nInitVal);
    nStatus = 1; // running
    Serial.print("Shower Jerk Init.  Level: ");
    Serial.println(nFinalTankLevel);
    szMessage = "Jerk Running to: " + String(nFinalTankLevel);
  }
  if (nStatus > 0)
  {
    if (nFreshTankLevel <= nFinalTankLevel) 
    {
      pumpOff();
      nStatus = 0;
      nFinalTankLevel = -1;
      
    }
  }
  
  
}
void CM::handleRandomLights(int nInitVal)
{
  static int nStatus = 0;
  static int nRepeatInterval = 0;
  static long nextInterval = 0;
  if (nInitVal > 0)
  {
    nStatus = 1;
    nRepeatInterval = nInitVal;
    nextInterval = millis() + 60000 * nRepeatInterval;
    
    Serial.print("Random Lights Initialized.  Interval:");
    Serial.println(nRepeatInterval);
  }
  if (nInitVal < 0)
  {
    nStatus = 0;
    Serial.println("Random Lights Stopped");
  }
  if (nStatus == 1)
  {
    if (millis() > nextInterval)
    {
      pressCabin();
      pressCargo();
      pressAwning();
      
      nextInterval = millis() + 60000 * nRepeatInterval;
      Serial.println("Pressed light");
      
    }
  }
}
void CM::handleWaterTracker(int nInitVal)
{
  static int nStatus = 0;
  static int freshTankLevelOn;
  static byte lastPumpCommand = 0;
  
  if (nInitVal > 0)
  {
    nStatus = 1;
    Serial.print("Shower Tracker Initialized");
    lastPumpCommand = *bPtrPumpCommand;
    
  }
  if (nInitVal < 0)
  {
    nStatus = 0;
    Serial.println("Shower Tracker Stopped");
  }
  if (nStatus == 1)
  {
    
    if ((bPtrPumpCommand > 0) && (lastPumpCommand == 0))
    {
      nStatus = 2;
      freshTankLevelOn = nFreshTankLevel;
      Serial.println("Pump On"); 
    } 
  }
  if (nStatus == 2)
  {
    
    if ((bPtrPumpCommand == 0) && (lastPumpCommand > 0))
    {
      nStatus = 0;
      Serial.println("Pump repeased");
     
      szMessage = String(freshTankLevelOn - nFreshTankLevel) + " STUs used";
      Serial.println(szMessage);
      
      
    }
    
  }
  lastPumpCommand = *bPtrPumpCommand;
  
}
  
void CM::handleWaterTempBlink(int nInitVal)
{
  static int nStatus = 0;
  static float fLastTemp = 0;
  if (nInitVal > 0)
  {
    nStatus = 1;
    Serial.print("Temperature Blink Initialized");
    return;
    
    
  }
  if (nInitVal < 0) 
  {
    nStatus = 0;
    Serial.println("Temp Blink Stopped");
  }
  if (nStatus == 1)
  {
    
    if ((fLastTemp <= 50) && (heater.fGlycolOutletTemp > 50))
    {
      handleCabinBlink(5);//true means init
      Serial.println("**BLINK**");
      nStatus = 0;
    }
  }
  fLastTemp = heater.fGlycolOutletTemp;
  
  
  
}
  
  
  

  
  

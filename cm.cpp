#include "cm.h"
void CM::init()
{
  fAmbientTemp = 0;
  heater.bFanOn = false;
  heater.nFanSpeed = 0;
  heater.bAirFlowOut = false;
  heater.bAutoMode = false;
  heater.fTargetTemp = 31;
  heater.bHotWater = false;
  
  //AC
  ac.bCompressor = false;
  ac.bFan = false;
  ac.fanMode = _AUTO;
  ac.bMagicClimate = false;
  ac.fTargetTemp = 21;
  ac.lastFanMode = 0;
  ac.lastOperatingMode = 0;

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
}




float CM::cToF(float fDeg)
{
  return (fDeg * 9.0 / 5.0  + 32.0);
}
void CM::data2Json(char *buffer,char *varName,bool b)
{
  buffer[0] = 0;
  if (b == true) sprintf(buffer,"\"%s\":true",varName);
  else sprintf(buffer,"\"%s\":false",varName);
}
void CM::data2Json(char *buffer,char *varName,int n)
{
  
  buffer[0] = 0;
  sprintf(buffer,"\"%s\":%d",varName,n);
}
void CM::data2Json(char *buffer,char *varName,float f)
{
  
  buffer[0] = 0;
  sprintf(buffer,"\"%s\":%f",varName,f);
}

void CM::getHeaterInfo(char *buffer)
{
  
  char sz1[32];data2Json(sz1,"bFanOn",heater.bFanOn);
  char sz2[32];data2Json(sz2,"nFanSpeed",heater.nFanSpeed);
  char sz3[32];data2Json(sz3,"bAirFlowOut",heater.bAirFlowOut);
  char sz4[32];data2Json(sz4,"bAutoMode",heater.bAutoMode);
  char sz5[32];data2Json(sz5,"fTargetTemp",cToF(heater.fTargetTemp));
  
  sprintf(buffer,"{%s,%s,%s,%s,%s}",sz1,sz2,sz3,sz4,sz5);
  
}


void CM::getACInfo(char *buffer)
{
  char sz1[32];data2Json(sz1,"bCompressor",ac.bCompressor);
  char sz2[32];data2Json(sz2,"bFan",ac.bFan);
  char sz3[32];data2Json(sz3,"fanMode",ac.fanMode);
  char sz4[32];data2Json(sz4,"bMagicClimate",ac.bMagicClimate);
  char sz5[32];data2Json(sz5,"fTargetTemp",cToF(ac.fTargetTemp));
  
  sprintf(buffer,"{%s,%s,%s,%s,%s}",sz1,sz2,sz3,sz4,sz5);
  
}
void CM::getTankInfo(char *buffer)
{
  char sz1[32];data2Json(sz1,"nFreshTankLevel",nFreshTankLevel);
  char sz2[32];data2Json(sz2,"nGrayTankLevel",nGrayTankLevel);
  sprintf(buffer,"{%s,%s}",sz1,sz2);
}

void CM::getMiscInfo(char *buffer)
{
  char sz1[32];data2Json(sz1,"fAmbientTemp",cToF(fAmbientTemp));
  char sz2[32];data2Json(sz2,"nVentSpeed",roofFan.nSpeed);
  
  sprintf(buffer,"{%s,%s}",sz1,sz2);
 
}


void CM::getAllInfo(char *buffer)
{
  
  
  char heaterInfo[300];getHeaterInfo(heaterInfo);
  char acInfo[300];getACInfo(acInfo);
  char tankInfo[160];getTankInfo(tankInfo);
  char miscInfo[64];getMiscInfo(miscInfo);
  
  sprintf(buffer,"{\"heater\":%s,\"acInfo\":%s,\"tankInfo\":%s,\"miscInfo\":%s}",heaterInfo,acInfo,tankInfo,miscInfo);
  
  
  
}

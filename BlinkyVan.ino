
#include "CM.h"
#include <mcp2515.h>



CM cm;
bool bVerbose = false;
    



//////////////////////////////////CANBUS STUFF////////////////////

MCP2515 mcp2515(RX);  //pin 10 is where the SPI chip select.  Can be any GPIO




void handleCanbus ()
{
  static can_frame mLast;
  struct can_frame m;
  static long startVerboseTimer = 0;
  
  
  
  if (mcp2515.readMessage(&m) == MCP2515::ERROR_OK)
  {
    float t = millis() / 1000.0;
    
    
    
    if (bVerbose) cm.printCan(m,false);
    
    if ((m.can_id == PDM1_MESSAGE) || (m.can_id == PDM2_MESSAGE)) 
    {
      cm.handlePDMMessage(t,m);
      return;
    }
    if ((m.can_id == PDM1_SHORT) || (m.can_id == PDM2_SHORT))
    {
      cm.handlePDMShort(m);
      return;
    }
    if (m.can_id == RIXENS_COMMAND) 
    {
      cm.handleRixensCommand(m);
      return;
    }
    if (m.can_id == RIXENS_GLYCOLVOLTAGE)
    {
      cm.handleRixensGlycolVoltage(m);
      return;
    }
    if ((m.can_id == RIXENS_RETURN1) || 
        (m.can_id == RIXENS_RETURN2) ||
        (m.can_id == RIXENS_RETURN3) ||
        (m.can_id == RIXENS_RETURN4) ||
        (m.can_id == RIXENS_RETURN6)) 
    {
      cm.handleRixensReturn(m);
      if (bVerbose) Serial.println();
      return;
    }
    if (m.can_id == ROOFFAN_STATUS) 
    {
      cm.handleRoofFanStatus(m);
      return;
    }
    if (m.can_id == ROOFFAN_CONTROL) 
    {
      cm.handleRoofFanControl(m);
      return;
    }
    if (m.can_id == THERMOSTAT_AMBIENT_STATUS) 
    {
      cm.handleThermostatAmbientStatus(m);
      return;
    }
    if ((m.can_id == PDM1_COMMAND) || (m.can_id == PDM2_COMMAND))
    {
      cm.handlePDMCommand(m);
      if (bVerbose) Serial.println();
      return;
    }
   
    if (m.can_id == TANK_LEVEL) 
    {
      cm.handleTankLevel(m);
      return;
    }
    
    
    if (m.can_id == THERMOSTAT_STATUS_1)
    {
      cm.handleThermostatStatus(m);
      return;
    }
    if (m.can_id == ACK_CODE)
    {
      cm.handleAck(m);
      return;
    }
    if ((m.can_id == 0x98FECAAF) || (m.can_id == 0x99FECA58)) 
    {
      if (bVerbose) cm.printCan(m);
      cm.handleDiagnostics(m);
      return;
    }
    
    cm.printCan(m,false);
    Serial.print("  ?");
    Serial.println();
    
  }
}





















void setupCanbus ()
{
  cm.init(&mcp2515);
  int nError = mcp2515.reset();
  nError += mcp2515.setBitrate(CAN_500KBPS);
  nError += mcp2515.setNormalMode();
  if (nError != 0)
  {
    Serial.print("CanBus configuration error: ");
    Serial.println(nError);
    
    while (true);// no need continuing
  
  }
}
void setup()
{
    
    Serial.begin(115200);
    delay(10);
    while (!Serial); // wait for serial attach
    Serial.print("Serial OK");
    Serial.flush();
    delay(500);
    
    setupCanbus();

    
    
}



void loop()
{
   
   static long blinkTimer = millis();
   handleCanbus();
   if (millis() - blinkTimer > 5000) 
   {
    cm.pressCabin();
    blinkTimer = millis();
   }
   
   
   
   ///////////////SET LED TO BLINK IF ACCESS POINT MODE IS TRUE
   
}

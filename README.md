
# ModeWifi
Adding Wifi to a Storyteller Campervan

04/04/25:  Added double tap to turn off everything possible.  Turns off pumps, lights, ac and fan.  Then retracts awning and turns off awning lihts

***Don't start messing with this stuff unless you know what you're doing.  You could brick your van, and Storyteller will not help you.  You could also void any warrantee you may have.***
***There-- you have been warned!***
What it can do:
1.  Control A/C
2.  Control Vent Fan
3.  Push Toggle Buttons
4.  Read Tank Status

What it cannot do/control:
1.  Rixens
2.  Dimming mights
3.  Battery
4.  inverter Control

This has to do with the mechanism that ModeCom interfaces with it's PDM's/Rixens.  
It may be possible to COMPLETELY REPLACE Mode Com with this device-- at least on Com Bus 1

Step 1.  Build/Buy a splitter cable.
I used DT plugs and a crimper.  These plugs are awesome and useful, for when you need to connect-re-connect anything in the future.  

Step 2.  Build the board.
Step 3.  Attach to split canbus cable.  I used the connection between Canbus1 and the Rixens heater, under the Groove Lounge.  It has DT-plugs already.  Just unplug, attach your splitter cable and VIOLA!
Step 4.  Upload the data folder to your ESP-32.  Make sure your SSID and password are in the config,txt file.  

The system invokes a mini-dns.  It is called cm.local.
PARTS:
https://www.amazon.com/dp/B0DB86YJ8D
https://www.amazon.com/dp/B0B2NWK1QX
https://www.amazon.com/dp/B0BVH43P9L
https://www.amazon.com/dp/B0718T232Z




![C2DB098D-C8A6-492D-BD14-D10913AA1890_1_105_c](https://github.com/user-attachments/assets/bc3380b2-bb1e-4e78-ac02-17fa1cec6312)
![9859B1A8-ED90-4E91-A5DC-019881EF6B41_1_105_c](https://github.com/user-attachments/assets/c38c6dc8-7039-4bed-9ab6-0a4ef97f4efc)
![0BE04399-0B7E-4827-828C-AA42BEBAC04E_1_105_c](https://github.com/user-attachments/assets/866c1593-9f9a-4545-80b3-02fb4a69ca2c)

#CURRENT COMMANDS#

##WiFi Commands##
  
  password 
  addFile
  printFilters")
  
  ##BUTTONS##
  
  pressCargo"))
  
  pressCabin")) 
  
  pressAwning")) 
  
  pressCirc")) 
  
  pressPump")) 
  
  pressDrain")) 
  
  pressAux")) 
  
  cabinOn"))
  
  lightsOn"))
  
  lightsOff"))
  
  allOff")
  
  allOffXAux")) 
  
  allOn"))
  
  printPDM"))
  
  verbose")) 
  
  blink") 

  ###AC COMMANDS###
  
  acOff")) cm.acCommand(0,0,0);
  
  acOn")) cm.acCommand(1,1,64);
  
  acModeHeat")) cm.setACOperatingMode(0b10);
  
  acFanOnly")) cm.setACOperatingMode(0b100);

  acFanLow")) cm.setACFanSpeed(10);
  
  acFanHigh")) cm.setACFanSpeed(255);
  
  acAlwaysOn")) cm.setACFanMode(1);
  
  acAuto")) cm.setACFanMode(0);

  acSetSpeed "))

  acSetOperatingMode "))

  acSetFanMode "))
  
  acSetTemp "))// in C
  
  ###Power Commands###
  printAmps1 "))
  
  printAmps2 "))
  
  ###Awning Commands###
  awningEnable"))
  
  awningOut"))
  
  awningIn"))
  
  
  ###Vent Commands###
  openVent"))
  
  closeVent")) 
  
  setVentSpeed "))
  
  setVentDir "))
  
  rf"))//Reset Filters
  
  filterOut "))
  
  filterIn "))
  
  filterMode "))
  
  clearFilters"))
  
  changeMask "))
  
  filterOutB0"))
  
  filterInB0"))
  
  showChangeOnly"))
  
  parseRaw"))
  
  reset"))
  
  quickSSID "))
  
  wifi "))
  
  webOn")) 
  webOff"))
  start"
  stop")
  

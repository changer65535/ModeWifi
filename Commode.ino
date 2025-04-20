
#include <WebServer.h>
#include <Uri.h>
#include <HTTP_Method.h>
#include "cm.h"
#include <SPI.h>
#include <Wire.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <mcp2515.h>
#include <HTTPClient.h>


CM cm;
long lDataTimer = millis();
char dataBuffer[160];
bool bVerbose = false;
bool bWebVerbose = false;
bool bUploadDataToWeb = true;
int nPDMChannel = 0;
int nPDMToPrint = 0;
const char* hostURL = "http://130.211.161.177/cv/cvAjax.php";
const char* szSecret = "dfgeartdsfcvbfgg53564fgfgh";

    


void setWebVariable (const char *szVariableName,int nValue)
{
  char szPostData[256];
  sprintf(szPostData,"setVariable&secret=%s&varName=%s&varValue=%d",szSecret,szVariableName,nValue);
  sendPost(szPostData);
}
void setWebVariable (const char *szVariableName,float fValue)
{
  char szFloat[16];
  dtostrf(fValue,4,1,szFloat);
  char szPostData[256];
  sprintf(szPostData,"setVariable&secret=%s&varName=%s&varValue=%s",szSecret,szVariableName,szFloat);
  sendPost(szPostData);
}

void sendPost (char *szCommand)
{
  StaticJsonDocument<200> json;
  char postData[128];
  sprintf(postData,"command=%s",szCommand);
  
  Serial.println(postData);
  HTTPClient http;   
  http.begin(hostURL);  //Specify destination for HTTP request
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");             //Specify content-type header
  int httpResponseCode = http.POST(postData);   //Send the actual POST request
  if (httpResponseCode > 0) 
  {   
        
        String res = http.getString();
        //for (int i = 0;i<res.length();i++) Serial.println((byte)res[i]);
        //Serial.println(res.substring(0,1));
        res = res.substring(3);
        DeserializationError error = deserializeJson(json, res);
        
        if (bWebVerbose) 
        {
          Serial.println(httpResponseCode);
          Serial.print("Response: ");
          Serial.println(res);
          
          Serial.print("Error: ");
          Serial.println(error.c_str());
          Serial.println(json["command"].as<const char*>());
        }
        
        
        handleCommand(json["command"]);
        
        
 
  }
  else Serial.println("Error on sending HTTP POST");

  http.end();  //Free resources
  
}
void postCurrentState()
{
  static bool bFirstTime = true;
  if (!bUploadDataToWeb) return;
  StaticJsonDocument<1024> doc;
  doc["secret"] = szSecret;
  doc["temp"] = cm.fAmbientTemp;
  doc["acSetpointCool"] = cm.ac.fSetpointCool;
  doc["nDomePosition"] = cm.roofFan.nDomePosition;
  doc["nRoofFanSpeed"] = cm.roofFan.nSpeed;
  doc["acOperatingMode"] = cm.ac.operatingMode;
  doc["acFanMode"] = cm.ac.fanMode;
  doc["acFanSpeed"] = cm.ac.fanSpeed;
  doc["freshTankLevel"] = cm.nFreshTankLevel;
  doc["freshTankDenom"] = cm.nFreshTankDenom;
  doc["grayTankLevel"] = cm.nGrayTankLevel;
  doc["grayTankDenom"] = cm.nGrayTankDenom;
  if (cm.heater.bFurnace) doc["glycolOutletTemp"] = cm.heater.fGlycolOutletTemp;
  else doc["glycolOutletTemp"] = 0;
  
  doc["upTimeMins"] = millis() / 1000 / 60;
  
  for (int i = 0;i<=12;i++) doc["pdm1"][i] = (byte) (cm.pdm1_output[i].fFeedback * 8 + 0.01);
  for (int i = 0;i<=12;i++) doc["pdm2"][i] = (byte) (cm.pdm2_output[i].fFeedback * 8 + 0.01);
  
  
  HTTPClient http;   
  http.begin(hostURL);
  http.addHeader("Content-Type", "application/json");
  String json;
  serializeJson(doc, json);
  if (bWebVerbose) Serial.println(json);
  int httpResponseCode = http.POST(json);
  if (httpResponseCode > 0) 
  {   
        
    String res = http.getString();
    res = res.substring(3);
    if (bWebVerbose) 
    {
      Serial.println(res);
      Serial.println(httpResponseCode);
      Serial.print("Response: ");
      Serial.println(res);
    }
    DeserializationError error = deserializeJson(doc, res);
    if (bWebVerbose)
    {
      Serial.print("Error: ");
      Serial.println(error.c_str());
      Serial.println(doc["command"].as<const char*>());
    }
    if (bFirstTime) 
    {
      bFirstTime = false;
      
    }
    else handleCommand(doc["command"]);//if first time, there may be a command stuck in the que.  Clear it. 
    
  }
  else if (bVerbose) Serial.println("Error on sending HTTP POST");

  http.end();  //Free resources
  
  
  
  
}
void logData(int nSensorID, const char* szDataType, const char* szDataValue)
{
  Serial.println("---Sending Log Data---");
  char szPostData[128];
  sprintf(szPostData,"command=logData&sensorID=%i&dataType=%s&dataValue=%s",nSensorID,szDataType,szDataValue);
  sendPost(szPostData);
}
//Board Adafruit Feather ESP32 V2 March 19,2025



//OLD BOARD INFO
//board ESP32 dev module
//Upoload Speed 921600
//CPU freq 240Mhz
//Flash Freq 80 mHz
//Partition Scheme "default 4MB with spiffs
//Arduino runs on "core 1
//Events run on "core 1



/*
 * System On messages....
98EEFF1F 8  2  0 44  E  0  0  0 40        //EEFF=Address Claiming
98EEFF1E 8  A  0 44  E  0  0  0 40        //EEFF--Address Claiming
98EAFFFE 3  0 EE  0                       //Global request for DGNs
98EEFF1F 8  2  0 44  E  0  0  0 40        EEFF=Address Claiming
98EEFF1E 8  A  0 44  E  0  0  0 40        /EEFF=Address Claiming
98EEFFF2 8 80 13 DE  9  0 A0 A0 C0 
99FEF903 8  1 10  0 F9 24  A 25  0        //ACCommand

0.00  94EF1E11 8 F8 F1  0  0  0  0 FC FF    PDM Config
---------
PDM: 1
Channel: 1 SOLAR_BACKUP
Soft Start Pct: (0xFF=disabled)0
Motor Lamp (0=lamp, 1=motor): 0
Loss of Comm: 0
POR Comm-Enable/Type/Braking: 11111100
LSC/CAL/Response: 11111111
99FEF903 8  1 10  0 F9 24  A 25  0 

0.00  94EF1E11 8 F8 F3  0  0  0  0 FC FF      PDM Config
---------
PDM: 1
Channel: 3 READING_LIGHT
Soft Start Pct: (0xFF=disabled)0
Motor Lamp (0=lamp, 1=motor): 0
Loss of Comm: 0
POR Comm-Enable/Type/Braking: 11111100
LSC/CAL/Response: 11111111

0.00  94EF1E11 8 F8 F7  0  0  0  0 FC FF    PDM Config
---------PDM
PDM: 1
Channel: 7 AWNING_ENABLE
Soft Start Pct: (0xFF=disabled)0
Motor Lamp (0=lamp, 1=motor): 0
Loss of Comm: 0
POR Comm-Enable/Type/Braking: 11111100
LSC/CAL/Response: 11111111

0.00  94EF1F11 8 F8 F9  0  0  0  0 FC FF 
PDM Config
---------
PDM: 2
Channel: 9 HVAC_POWER
Soft Start Pct: (0xFF=disabled)0
Motor Lamp (0=lamp, 1=motor): 0
Loss of Comm: 0
POR Comm-Enable/Type/Braking: 11111100
LSC/CAL/Response: 11111111
En
0.00  94EF1E11 8 F8 FA  0  0  0  0 FC FF 
PDM Config
---------
PDM: 1
Channel: A EXHAUST_FAN
Soft Start Pct: (0xFF=disabled)0
Motor Lamp (0=lamp, 1=motor): 0
Loss of Comm: 0
POR Comm-Enable/Type/Braking: 11111100
LSC/CAL/Response: 11111111

0.00  94EF1F11 8 F8 F1 FF FF FF FF FF FF 
PDM Config
---------
PDM: 2
Channel: 1 PDM_2_1
Soft Start Pct: (0xFF=disabled)FF
Motor Lamp (0=lamp, 1=motor): FF
Loss of Comm: FF
POR Comm-Enable/Type/Braking: 11111111
LSC/CAL/Response: 11111111
99FEF903 8  1 10  0 F9 24  A 25  0 

1.00  94EF1E11 8 F8 F1  0  0  0  0 FD FF 
PDM Config
---------
PDM: 1
Channel: 1 SOLAR_BACKUP
Soft Start Pct: (0xFF=disabled)0
Motor Lamp (0=lamp, 1=motor): 0
Loss of Comm: 0
POR Comm-Enable/Type/Braking: 11111101
LSC/CAL/Response: 11111111

1.00  94EF1E11 8 F8 F2  0  0  0  0 FD FF 
PDM Config
---------
PDM: 1
Channel: 2 CARGO_LIGHTS
Soft Start Pct: (0xFF=disabled)0
Motor Lamp (0=lamp, 1=motor): 0
Loss of Comm: 0
POR Comm-Enable/Type/Braking: 11111101
LSC/CAL/Response: 11111111

1.00  94EF1F11 8 F8 FB  0  0  0  0 FD 83 
PDM Config
---------
PDM: 2
Channel: B SINK_PUMP
Soft Start Pct: (0xFF=disabled)0
Motor Lamp (0=lamp, 1=motor): 0
Loss of Comm: 0
POR Comm-Enable/Type/Braking: 11111101
LSC/CAL/Response: 10000011

1.00  94EF1E11 8 F8 F3  0  0  0  0 FD FF 
PDM Config
---------
PDM: 1
Channel: 3 READING_LIGHT
Soft Start Pct: (0xFF=disabled)0
Motor Lamp (0=lamp, 1=motor): 0
Loss of Comm: 0
POR Comm-Enable/Type/Braking: 11111101
LSC/CAL/Response: 11111111
98EE00AF 8 34  0 80  F  0  0  0 80 //Address Claim
99FEF903 8  1 10  0 F9 24  A 25  0 //ACCommand
99FEF903 8  1 10  0 F9 24  A 25  0 //ACCommand
99FEF903 8  1 10  0 F9 24  A 25  0 //ACCommand
98EE00C1 8 16 37 23  F  0  0  0  0  //Address Claimed  SN#16, SN #37 Man Code F.  Vent FanFan
99FEF903 8  1 10  0 F9 24  A 25  0 //ACCommand
99FEF903 8  1 10  0 F9 24  A 25  0 //ACCommand
99FEF903 8  1 10  0 F9 24  A 25  0 //ACCommand
99FEF903 8  1 10  0 F9 24  A 25  0 //ACCommand
99FEF903 8  1 10  0 F9 24  A 25  0 //ACCommand
99FECAC1 8  5 C1 FF FF FF FF FF  F //Status Room Fan
99FEF903 8  1 10  0 F9 24  A 25  0 //ACCommand
99FEA7C1 8  2 15  0 14 28 22  0  0 //Status Room Fan
99FEF903 8  1 10  0 F9 24  A 25  0 //ACCommand
99FEF903 8  1 10  0 F9 24  A 25  0 //ACCommand
99FEF903 8  1 10  0 F9 24  A 25  0 //ACCommand
99FEF903 8  1 10  0 F9 24  A 25  0 //ACCommand
99FECAC1 8  5 C1 FF FF FF FF FF  F //Status Room Fan
99FEA7C1 8  2 15  0 14 38 22  0  0 //ROOF_FAN_STATUS_1
99FEF903 8  1 10  0 F9 24  A 25  0 //ACCommand
99FEF903 8  1 10  0 F9 24  A 25  0 //ACCommand
99FF9CC1 8  1 40 25 FF FF FF FF FF //THERMOSTAT_AMBIENT_STATUS_1
98EE0058 8 16 37 23  F  0  0  0  0 //ACCommand
99FEF903 8  1 10  0 F9 24  A 25  0 //ACCommand
98E80358 8  0 FF FF FF FF F9 FE  1  //Acq.  Instance 3.  Acq code 0.  Air Conditioning
99FEF903 8  1 10  0 F9 24  A 25  0 //ACCommand
98E80358 8  0 FF FF FF FF F9 FE  1  //Acq.  Instance 3.  Acq code 0.  Air Conditioning

*/

//Mode COMM INFO
//PDM1    DO-1     Solar Backup
//        DO-2     Cargo Lights
//        DO-3     Reading Lights
//        DO-4     Cabin Lights
//        DO-5     
//        DO-6     Recirculation Pump
//        DO-7     Awning Enable
//        DO-8     
//        DO-9      //
//        DO-10     //Exhaust Fan.  Yes this is the ceiling fan
//        DO-11     //Furnace Power
//        DO-12     //Main Freshwater Pump

//        DI-1     Awning In Switch
//        DI-2     Awning Out Switch
//        DI-3     Cargo Light Switch
//        DI-4     Cabin Light Switch
//        DI-5     Awning Light Switch
//        DI-6     Recirc Pump Switch
//        DI-7     Awning Enable
//        DI-8     Engine Running

//PDM2    DO-2     Galley Fan
//        DO-3     Regrigerator
//        DO-4     12V/USB
//        DO-5     Awning Lights
//        DO-6
//        DO-7     Tank Mnth Pwr
//        DO-8     Swith PWR
//        DO-9
//        DO-A
//        DO-B    Macerator Pump
//        DO-C    Aux??

//        
//        Di-4     Aux1 Switch
//        Di-5     Water Pump
//        Di-6     Master Light Switch
//        Di-9     Sink Switch
          







//PDM2    
//        D05 Awning Lights

  


//async web server info:
//https://randomnerdtutorials.com/esp32-async-web-server-espasyncwebserver-library/

//Sketch Data upload.  
//  https://github.com/me-no-dev/arduino-esp32fs-plugin
//Upload to .app filder (then show package contents.  Then  /Java/Tools etc.
//Partition Scheme:  No OTA 1MB/3MB SPIFFS



const char* host = "cm";

WebServer server(80);



//////////////////////////////////CANBUS STUFF////////////////////

MCP2515 mcp2515(RX);  //pin 10 is where the SPI chip select.  Can be any GPIO
bool bReadBus = false;


/////////////////////////////////PDM STUFF//////////////////






//////////////////////////////////WEB SERVER STUFF////////////////////////
bool apFound = false;                 //used when finding Quick Access Point
bool apMode = false;
bool bAccessPointMode = false;
bool bNeverConnected = true;

#define LED 2
#define MAX_CONNECTIONS 10
struct
{
  char  ssid[32];
  char password[32];
}connections[MAX_CONNECTIONS];
int connectionsIndex = 0;

//ACCESS POINT
const char* ssidWAP     = "Commmode";
const char* passwordWAP = "123456789";
File fsUploadFile;        // a File object to temporarily store the received file


unsigned long filterOut[32];
int nFilterOutIndex = 0;
byte filterOutB0[16];         //bilters out the first byte for PDM command lookup
int nFilterOutB0Index = 0;

unsigned long filterIn[8];
int nFilterInIndex = 0;

byte filterInB0[16];         //bilters out the first byte for PDM command lookup
int nFilterInB0Index = 0;

int nFilterMode = 1;            //0 = no filter
                                //1 = filter in
                                //2 = filter out



bool bShowChangeOnly = false;
byte bChangeMask = 0xff;




void displayMessage(String szTemp)
{
  Serial.println(szTemp);
}

void waitForKey(String str)
{
  Serial.print("Waiting For Keyress: ");
  Serial.println(str);
  while(true) // remain here until told to break
  {
    if(Serial.available() > 0) 
    {
      
      byte ch = Serial.read();
      break;
      
    }
  }
}


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// WEB SERVER STUFF ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

bool setupNewAP()
{
  apMode = true;
  //statusLed(RgbColor(0,255,255));
  Serial.println("-=========SETTING UP WIFI ACCESS POINT=====");
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE); 
 //statusLed(RgbColor(0,255,255));
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  Serial.println("-------------------");
  WiFi.softAP(ssidWAP);
 //statusLed(RgbColor(0,255,255));
  IPAddress local_IP = IPAddress (192,168,8,100);
  if(!WiFi.softAPConfig(local_IP, IPAddress(192, 168, 8, 1), IPAddress(255, 255, 255, 0)))
  {
      Serial.println("AP Config Failed");
     //statusLed(RgbColor(0,0,0));
      return false;
  }
  
  displayMessage("AP-" + local_IP.toString());
 //statusLed(RgbColor(0,255,255));
  bAccessPointMode = true;
  return true;
}  

bool findExistingAP ()
{
  static unsigned long startMillis = millis();
  while (millis() - startMillis < 10000)
  {
    int n = WiFi.scanNetworks();Serial.print("scan done.  ");Serial.print(n);Serial.println(" networks found");
    for (int i = 0; i < n; ++i) 
    {
      
      Serial.print("Checking ");Serial.println(WiFi.SSID(i));
      int nIndex = ssidExists(WiFi.SSID(i));
      if (nIndex > -1) 
      {
        Serial.print(WiFi.SSID(i));
        Serial.println(" exists");
        WiFi.begin(connections[nIndex].ssid,connections[nIndex].password);
        return true;
      }
    }
  }
  displayMessage("NO AP FOUND");
  return false;
}


bool setupWiFi ()
{
  
  delay(10);
  Serial.println("Entering Wifi");
  
  
   Serial.print("...Starting Scan...");
   Serial.println (connectionsIndex);
   
   for (int i = 0;i<connectionsIndex;i++)
   {
     Serial.print(i);
     Serial.print(" ssid: ");
     Serial.print(connections[i].ssid);
     Serial.print(" password: ");
     Serial.println(connections[i].password);
   }

  // WiFi.scanNetworks will return the number of networks found
  if (apFound == true) 
  {
    
    //Wait for WiFi to connect
    unsigned int startConnectTime = millis();
    Serial.print("Connecting-->");
    while (WiFi.status() != WL_CONNECTED)
    {      
      Serial.print(".");
      
      
      delay(100);
      
      if (millis() - startConnectTime > 10000) 
      {
        displayMessage("");
        displayMessage("WIFI FAILURE");
        Serial.println("No AP found.  Setting up new AP");
    
        if (setupNewAP() == false)
        {
          displayMessage("TOTAL AP FAILURE");
          return false;
        }
        else return true;
        
       
      }
    }
    //waitForKey("connected");
  }
  
  return true;
 
}
//////////////////////////////////////////////////////SERVER STUFF BELOW/////
void redirectServer (String loc)
{
  server.sendHeader("Location", loc, true);
  server.send ( 302, "text/plain", "");
}
void handleFileUpload() 
{
  
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) 
  {
    String filename = upload.filename;
    if (filename.length() == 0)
    {
      Serial.println("Error File Upload Length 0");
      redirectServer(String("/success.html"));
      return;
    }
    
    if (!filename.startsWith("/")) filename = "/" + filename;
    
    Serial.print("handleFileUpload Name: "); Serial.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } 
  else 
  if (upload.status == UPLOAD_FILE_WRITE) 
  {
    Serial.print("handleFileUpload Data: "); Serial.println(upload.currentSize);
    if (fsUploadFile) fsUploadFile.write(upload.buf, upload.currentSize);
    
  } 
  else if (upload.status == UPLOAD_FILE_END) 
  {
    if (fsUploadFile) fsUploadFile.close();
    Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
    //redirect
    redirectServer(String("/success.html"));
    //if (filename == "/filters.txt")
    resetFilters();
    
  }
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  
}

void handleRoot() 
{
  Serial.println("Root");
  redirectServer(String("/index.html"));
}
void handleDir ()
{
    
    File root = SPIFFS.open("/");
    String sendStr = "";

    File file = root.openNextFile();
   
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200,"text/json","");
    while (file)
    {
    
      String fileName = file.name();
      Serial.println(fileName);
      
      file = root.openNextFile();
      server.sendContent(fileName + "\r\n");
    }

    
}
void printServerArgs ()
{
  Serial.print("Ajax Total Args: ");
  Serial.println(server.args());
  for (uint8_t i=0; i<server.args(); i++)
  {
    Serial.print(server.argName(i));
    Serial.print(": ");
    Serial.println(server.arg(i));
  }
}
void handleAJAX()
{

  bNeverConnected = false;//we have connected now!
  
  printServerArgs();
  
  String szCommand = server.arg("command");
  if (szCommand == "") return;
  
   
  Serial.print("Command is: ");
  Serial.println(szCommand);
  if (szCommand == "saveFile")
  {
    String szFname = server.arg("fname");
    String szContents = server.arg("contents");
    Serial.println (szFname);
    Serial.println (szContents);
    File file = SPIFFS.open("/" + szFname, FILE_WRITE);
    if (!file) 
    {
     
      Serial.println("There was an error opening the file for writing");
      return;
    }
    if (file.print(szContents)) Serial.println("saved...");
    else Serial.println("error saving");
    file.close();
    server.send(200, "text/plain", "{\"message\":\"file saved\"}");
    return;
  }
  if (szCommand == "deleteFile")
  {
    String szFname = server.arg("fname");
    Serial.println (szFname);
    SPIFFS.remove("/" + szFname);
    
    server.send(200, "text/plain", "file deleted");
    return;
  }
  if (szCommand == "sendString")
  {
    String szCommand = server.arg("str");
    //Serial.print("string command is ");Serial.println(szCommand);
    handleCommand(szCommand);
    return;
  } 
  if (szCommand == "getIP")
  {
    if (bAccessPointMode == true)
    {
      Serial.println("getIP-- AP");
      Serial.println( WiFi.softAPIP().toString());
      server.send(200, "text/plain", WiFi.softAPIP().toString());
     
      
    }
    else
    {
      Serial.println("getIP-- STA");
      Serial.println(WiFi.softAPIP().toString());
      server.send(200, "text/plain", WiFi.localIP().toString());
    }
    return;
    
     
  }
  if (szCommand == "edit")
  {
    String szReturn = "";
    String fname = String("/") + server.arg("fname");
    Serial.print("extracted fname is ");
    Serial.println(fname);
    File f = SPIFFS.open(fname);
    Serial.println("File Content:");
 
    while(f.available())
    {
      char ch = f.read();
        Serial.write(ch);
        szReturn = szReturn + ch;
    }
    server.send(200,"text/plain",szReturn);
 
    f.close();
    return;
    
  }
  if (szCommand == "getHeaterInfo")
  {
    char szBuffer[300];
    cm.getHeaterInfo(szBuffer);
    server.send(200, "text/plain", szBuffer);
    return;
    
  }
  if (szCommand == "getAllInfo")
  {
    char szBuffer[1000];
    cm.getAllInfo(szBuffer);
    server.send(200, "text/plain", szBuffer);
    return;
    
  }
    
    Serial.print("NOT PARSED.  REVERTING TO HANDLE COMMAND ");
    Serial.println(szCommand);
    handleCommand(szCommand);
    return;
  
}


void setupServer()
{
  //Start with mDNS
  if (MDNS.begin(host)) 
  {
    MDNS.addService("http", "tcp", 80);
    Serial.println();
    Serial.println("MDNS responder started");
    Serial.print("You can now connect to http://");
    Serial.print(host);
    Serial.println(".local");
  }

  ////Now do the server
  server.on("/", handleRoot);
  server.on ("/upload.html", HTTP_POST,[]()                       // if the client posts to the upload page
  {                                                               // Send status 200 (OK) to tell the client we are ready to receive
    server.send(200); 
  },                          
  handleFileUpload);
  server.on ("/files.html", HTTP_POST,[]()                       // if the client posts to the upload page
  {                                                               // Send status 200 (OK) to tell the client we are ready to receive
    server.send(200); 
  },                          
  handleFileUpload);
  
  server.on("/dir", []() {handleDir();});
  
  // Receive and save the file                                 
    
  server.on("dir",HTTP_GET,handleDir);
  server.serveStatic("/", SPIFFS, "/");
  server.on("/cm.dum",HTTP_POST,  handleAJAX);
  
  server.onNotFound(handleNotFound);
  
  server.begin();
  if (bAccessPointMode)
  {
    displayMessage("Soft-AP ");
    displayMessage(ssidWAP);
    displayMessage(WiFi.softAPIP().toString());
  }
  else
  {
    displayMessage("WebSocket Ok");
    displayMessage("HTTP  Ok");
    
    displayMessage(WiFi.SSID());
    displayMessage(WiFi.localIP().toString());
  }
}
int ssidExists (String search)
{
  int nRet = -1;
  for (int i = 0;i<connectionsIndex;i++)
  {
    if (bVerbose) Serial.print(" Checking: ");
    if (bVerbose) Serial.println(connections[i].ssid);
    if (String(connections[i].ssid) == search) return i;
  }
  return -1;
}

int gaInt (String szCommand,char del)//stands for get at!
{
  int spaceIndex = 0;
  if (del != ' ') spaceIndex = szCommand.indexOf(' ') + 1;
  int index = szCommand.indexOf(del,spaceIndex) + 1;
  if (index == 0) return -1;
  return szCommand.substring(index).toInt();
}
int gaInt (String szCommand,String del)//stands for get at!
{
  int spaceIndex = 0;
  int index = szCommand.indexOf(del) + del.length();
  if (index == 0) return -1;
  return szCommand.substring(index).toInt();
}

String gaStr (String szCommand,char del)//stands for get at!
{
  int spaceIndex = 0;
  if (del != ' ') spaceIndex = szCommand.indexOf(' ') + 1;
  int index = szCommand.indexOf(del,spaceIndex) + 1;
  if (index == 0) return "";
  return szCommand.substring(index);
}
float gaFloat (String szCommand,char del)//stands for get at!
{
  int spaceIndex = 0;
  if (del != ' ') spaceIndex = szCommand.indexOf(' ');
  
  int index = szCommand.indexOf(del,spaceIndex) + 1;
  if (index == 0) return -1;
  return szCommand.substring(index).toFloat();
}
int gaTime (String szCommand,char del)//returns total seconds
{
  int spaceIndex = 0;
  if (del != ' ') spaceIndex = szCommand.indexOf(' ');
  int index = szCommand.indexOf(del,spaceIndex) + 1;
  int colonIndex = szCommand.indexOf(':',index) + 1;
  int minutes = gaInt(szCommand,index);
  int seconds = gaInt(szCommand,colonIndex);
  return minutes * 60 + seconds;
  
}
unsigned long gaHex (String szCommand,char del)
{
  
 
  int index = szCommand.indexOf(del) + 1;
  //Serial.print ("szCommand= ");Serial.println(szCommand);
  //Serial.print("index=");Serial.println(index);
  if (index == 0) return -1;
  char *endChar;
  //Serial.println(szCommand[index]);
  return strtoull(&szCommand[index],&endChar,16);  
}
String extractQuote(String szCommand)
{
  int index1 = szCommand.indexOf('\"') + 1;
  if (index1 == 0) return "";
  int index2 = szCommand.indexOf('\"',index1);
  if (index2 == -1) return "";
  return szCommand.substring(index1,index2);
  
}

void printBin(byte aByte) 
{
  for (int8_t aBit = 7; aBit >= 0; aBit--)
    Serial.write(bitRead(aByte, aBit) ? '1' : '0');
}
void parseFile (String szFname)
{
  
  displayMessage("parsing " + szFname);
  File f = SPIFFS.open("/" + szFname, FILE_READ);
  displayMessage("file open");
  
  if (!f) 
  {
    waitForKey("FILE ERROR");
    //while (true)
    //{
    //  delay(1);
   // }
  }
  char buffer[120];
  while (f.available()) 
  {
   int l = f.readBytesUntil('\n', buffer, sizeof(buffer)-2);
   buffer[l] = 0;
   
   handleCommand(buffer);
   delay(10);
  //statusLed(RgbColor (0,255,0));
  } 
  f.close();
  
}

void resetFilters()
{
  handleCommand ("stop");
  Serial.println("reset Filters");
  nFilterOutIndex = 0;
  nFilterOutB0Index = 0;
  nFilterInIndex = 0;
  nFilterInB0Index = 0;
  parseFile ("filters.txt");
  handleCommand ("start");
  return;
}
void handleCommand (String szCommand)
{
  
  //Serial.println("Handling Command Now");
  if (szCommand != "") Serial.print("   HANDLE COMMAND: ");
  
  if ((szCommand.startsWith("password ") || (szCommand.startsWith("wifi "))))
  {
    displayMessage("PASSWORD HIDDEN");
  }
  else
  {
    displayMessage(szCommand);
  }
  
  if (szCommand.startsWith ("addFile "))
  {
    String fname = gaStr(szCommand,' ');
    parseFile(fname);
    return;
  }
  if (szCommand == "printFilters")
  {
    Serial.println(nFilterOutIndex);
    Serial.println("FILTER OUT--------");
    for (int i = 0;i<nFilterOutIndex;i++) Serial.println(filterOut[i],HEX); 
    
    Serial.println(nFilterOutB0Index);
    Serial.println("B[0]----------");
    for (int i = 0;i<nFilterOutB0Index;i++)
    {
        
        Serial.println(filterOutB0[i],HEX);  
    }
    Serial.println("FILTER IN--------");
    for (int i = 0;i<nFilterInIndex;i++) Serial.println(filterIn[i],HEX); 

    Serial.println("FILTER IN B0--------");
    for (int i = 0;i<nFilterInB0Index;i++) Serial.println(filterInB0[i],HEX); 
    
    Serial.println("DATA MASK");Serial.println(bChangeMask,BIN);
    
  }
  //BUTTONS
  
  if (szCommand.startsWith ("pressCargo")) cm.pressdigitalbutton(cm.lastpdm1inputs1to6,6,1);
  if (szCommand.startsWith ("pressCabin")) cm.pressdigitalbutton(cm.lastpdm1inputs1to6,6,0);
  if (szCommand.startsWith ("pressAwning")) cm.pressdigitalbutton(cm.lastpdm1inputs1to6,7,3);
  if (szCommand.startsWith ("pressCirc")) cm.pressdigitalbutton(cm.lastpdm1inputs1to6,7,2);
  if (szCommand.startsWith ("pressPump")) cm.pressdigitalbutton(cm.lastpdm2inputs1to6,7,3);
  if (szCommand.startsWith ("pressDrain")) cm.pressdigitalbutton(cm.lastpdm2inputs7to12,6,1);
  if (szCommand.startsWith ("pressAux")) cm.pressdigitalbutton(cm.lastpdm2inputs1to6,6,0);
  if (szCommand.startsWith ("cabinOn"))
  {
    if (cm.pdm1_output[PDM1_OUT_CABIN_LIGHTS].bCommand == 0) cm.pressdigitalbutton(cm.lastpdm1inputs1to6,6,0);
   
  }
  if (szCommand.startsWith("lightsOn"))
  {
    if (cm.pdm1_output[PDM1_OUT_CABIN_LIGHTS].bCommand == 0) cm.pressdigitalbutton(cm.lastpdm1inputs1to6,6,0);
    if (cm.pdm1_output[PDM1_OUT_CARGO_LIGHTS].bCommand == 0) cm.pressdigitalbutton(cm.lastpdm1inputs1to6,6,1);
  }
  if (szCommand.startsWith("lightsOff"))
  {
    if (cm.pdm1_output[PDM1_OUT_CABIN_LIGHTS].bCommand > 0) cm.pressdigitalbutton(cm.lastpdm1inputs1to6,6,0);
    if (cm.pdm1_output[PDM1_OUT_CARGO_LIGHTS].bCommand > 0) cm.pressdigitalbutton(cm.lastpdm1inputs1to6,6,1);
    if (cm.pdm1_output[PDM1_OUT_AWNING_LIGHTS].bCommand > 0) cm.pressdigitalbutton(cm.lastpdm1inputs1to6,7,3);
    
  }
  if (szCommand == "allOff")
  {
    if (cm.pdm1_output[PDM1_OUT_CABIN_LIGHTS].bCommand > 0) cm.pressdigitalbutton(cm.lastpdm1inputs1to6,6,0);
    delay(500);
    if (cm.pdm1_output[PDM1_OUT_CARGO_LIGHTS].bCommand > 0) cm.pressdigitalbutton(cm.lastpdm1inputs1to6,6,1);
    delay(500);
    if (cm.pdm1_output[PDM1_OUT_AWNING_LIGHTS].bCommand > 0) cm.pressdigitalbutton(cm.lastpdm1inputs1to6,7,3);
    delay(500);
    if (cm.pdm2_output[PDM2_OUT_AUX_POWER].bCommand > 0) cm.pressdigitalbutton(cm.lastpdm2inputs1to6,6,0);
    delay(500);
    if (cm.pdm1_output[PDM1_OUT_RECIRC_PUMP].bCommand > 0) cm.pressdigitalbutton(cm.lastpdm1inputs1to6,7,2);
    delay(500);
    if (cm.pdm1_output[PDM1_OUT_WATER_PUMP].bCommand > 0) cm.pressdigitalbutton(cm.lastpdm2inputs1to6,7,3);
    delay(500);
    
  }
  if (szCommand.startsWith("allOffXAux"))
  {
    if (cm.pdm1_output[PDM1_OUT_CABIN_LIGHTS].fFeedback > 0) cm.pressdigitalbutton(cm.lastpdm1inputs1to6,6,0);
    delay(500);
    if (cm.pdm1_output[PDM1_OUT_CARGO_LIGHTS].bCommand > 0) cm.pressdigitalbutton(cm.lastpdm1inputs1to6,6,1);
    delay(500);
    if (cm.pdm1_output[PDM1_OUT_AWNING_LIGHTS].fFeedback > 0) cm.pressdigitalbutton(cm.lastpdm1inputs1to6,7,3);
    delay(500);
    //if (cm.pdm2_output[PDM2_OUT_AUX_POWER].fFeedback > 0) cm.pressdigitalbutton(cm.lastpdm2inputs1to6,6,0);
    //delay(500);
    if (cm.pdm1_output[PDM1_OUT_RECIRC_PUMP].fFeedback > 0) cm.pressdigitalbutton(cm.lastpdm1inputs1to6,7,2);
    delay(500);
    if (cm.pdm1_output[PDM1_OUT_WATER_PUMP].bCommand > 0) cm.pressdigitalbutton(cm.lastpdm2inputs1to6,7,3);
    delay(500);
    
    
  }
  if (szCommand.startsWith("allOn"))
  {
    if (cm.pdm1_output[PDM1_OUT_CABIN_LIGHTS].bCommand == 0) cm.pressdigitalbutton(cm.lastpdm1inputs1to6,6,0);
    if (cm.pdm1_output[PDM1_OUT_CARGO_LIGHTS].bCommand == 0) cm.pressdigitalbutton(cm.lastpdm1inputs1to6,6,1);
    if (cm.pdm1_output[PDM1_OUT_AWNING_LIGHTS].bCommand == 0) cm.pressdigitalbutton(cm.lastpdm1inputs1to6,7,3);
    if (cm.pdm2_output[PDM2_OUT_AUX_POWER].bCommand == 0) cm.pressdigitalbutton(cm.lastpdm2inputs1to6,6,0);
    
  }
if (szCommand.startsWith("printPDM"))
  {
    for (int i = 1;i<=12;i++)
    {
      Serial.print("PDM1 #"); 
      Serial.print(i);
      Serial.print(" ");
      Serial.print(cm.pdm1_output[i].szName);
      Serial.print("-->");
      Serial.print("Soft Start Step: ");
      Serial.print(cm.pdm1_output[i].bSoftStartStepSize,HEX);
      Serial.print(",Motor/Lamp: ");
      Serial.print(cm.pdm1_output[i].bMotorOrLamp,HEX);
      Serial.print(", LOC: ");
      Serial.print(cm.pdm1_output[i].bLossOfCommunication,BIN);
      Serial.print(", Byte7: ");
      Serial.print(cm.pdm1_output[i].bByte7,BIN);
      Serial.print(", Byte8: ");
      Serial.print(cm.pdm1_output[i].bByte8,BIN);
      Serial.print(" CMD: ");
      Serial.print(cm.pdm1_output[i].bCommand);
      Serial.print(", Feedback: ");
      Serial.print(cm.pdm1_output[i].fFeedback);
      Serial.println("A");
      
    }
    for (int i = 1;i<=12;i++)
    {
      Serial.print("PDM2 #"); 
      Serial.print(i);
      Serial.print(" ");
      Serial.print(cm.pdm2_output[i].szName);
      Serial.print("-->");
      Serial.print("Soft Start Step: ");
      Serial.print(cm.pdm2_output[i].bSoftStartStepSize,HEX);
      Serial.print(",Motor/Lamp: ");
      Serial.print(cm.pdm2_output[i].bMotorOrLamp,HEX);
      Serial.print(", LOC: ");
      Serial.print(cm.pdm2_output[i].bLossOfCommunication,BIN);
      Serial.print(", Byte7: ");
      Serial.print(cm.pdm2_output[i].bByte7,BIN);
      Serial.print(", Byte8: ");
      Serial.print(cm.pdm2_output[i].bByte8,BIN);
      Serial.print(" CMD: ");
      Serial.print(cm.pdm2_output[i].bCommand); 
      Serial.print(", Feedback: ");
      Serial.print(cm.pdm2_output[i].fFeedback);
      Serial.println("A");
      
    }
  }
   
  if (szCommand.startsWith("verbose")) 
  {
    bVerbose = !bVerbose;
    cm.bVerbose = bVerbose;
  }
  if (szCommand == "blink") cm.nBlinkState = 1;
  if (szCommand.startsWith("acOff")) cm.acCommand(0,0,0);
  if (szCommand.startsWith("acOn")) cm.acCommand(1,1,64);
  if (szCommand.startsWith("acModeHeat")) cm.setACOperatingMode(0b10);
  if (szCommand.startsWith("acFanOnly")) cm.setACOperatingMode(0b100);

  if (szCommand.startsWith("acFanLow")) cm.setACFanSpeed(10);
  if (szCommand.startsWith("acFanHigh")) cm.setACFanSpeed(255);
  
  
  if (szCommand.startsWith("acAlwaysOn")) cm.setACFanMode(1);
  if (szCommand.startsWith("acAuto")) cm.setACFanMode(0);
  
 
  if (szCommand.startsWith ("printAmps1 "))
  {
    nPDMChannel = gaInt(szCommand,' ');
    nPDMToPrint = 1;
    
   
  }
  if (szCommand.startsWith ("printAmps2 "))
  {
    nPDMChannel = gaInt(szCommand,' ');
    nPDMToPrint = 2;
    
    
  }
  if (szCommand.startsWith("acSetSpeed "))
  {
    int nSpeed = gaInt(szCommand,' ');
    cm.setACFanSpeed(nSpeed);
  }
  if (szCommand.startsWith("acSetOperatingMode "))
  {
    int nMode = gaInt(szCommand,' ');
    cm.setACOperatingMode(nMode);
  }
  
  if (szCommand.startsWith("acSetFanMode "))
  {
    int nMode = gaInt(szCommand,' ');
    cm.setACFanMode(nMode);
  }
  if (szCommand.startsWith("acSetTemp "))// in C
  {
    float fTemp = gaFloat(szCommand,' ');
    cm.acSetTemp(fTemp);
  }
  if (szCommand.startsWith("awningEnable"))
  {
    cm.pressdigitalbutton(cm.lastpdm1inputs7to12,6,3);
    
  }
  if (szCommand.startsWith("awningOut"))
  {
    cm.pressdigitalbutton(cm.lastpdm1inputs7to12,7,2);
    
    
  }
  if (szCommand.startsWith("awningIn"))
  {
    cm.pressdigitalbutton(cm.lastpdm1inputs7to12,7,3);
  }
  
  
  
  if (szCommand.startsWith("openVent")) cm.openVent();
  if (szCommand.startsWith("closeVent")) cm.closeVent();
  if (szCommand.startsWith("setVentSpeed "))
  {
    int nSpeed = gaInt(szCommand,' ');
    cm.setVentSpeed(nSpeed);
  }
  if (szCommand.startsWith("setVentDir "))
  {
    int nDir = gaInt(szCommand,' ');
    cm.setVentDirection(nDir);
  }
  
  if (szCommand.startsWith ("rf"))//Reset Filters
  {
    resetFilters();
    return;
  }
  if (szCommand.startsWith("filterOut "))
  {
    unsigned long n = gaHex(szCommand,' ');
    filterOut[nFilterOutIndex] = n;
    nFilterOutIndex++;
    //Serial.print("filtering out: ");
    //Serial.println (n,HEX);
  }

  if (szCommand.startsWith("filterIn "))
  {
    unsigned long n = gaHex(szCommand,' ');
    filterIn[nFilterInIndex] = n;
    nFilterInIndex++;
    //Serial.print("filtering in: ");
    //Serial.println (n,HEX);
  }

  if (szCommand.startsWith("filterMode "))
  {
    int n = gaInt(szCommand," ");
    nFilterMode = n;
    
  }
  if (szCommand.startsWith("clearFilters"))//because this gets confused with startswith "reset"
  {
    resetFilters();
    
  }
  if (szCommand.startsWith("changeMask "))
  {
    bChangeMask = gaHex(szCommand,' ');
    Serial.println(bChangeMask);
  }
  
  if (szCommand.startsWith("filterOutB0"))
  {
    byte n = gaHex(szCommand,' ');
    filterOutB0[nFilterOutB0Index] = n;
    nFilterOutB0Index++;
    Serial.print("filtering out B0: ");
    Serial.println (n,HEX);
  }
  if (szCommand.startsWith("filterInB0"))
  {
    byte n = gaHex(szCommand,' ');
    filterInB0[nFilterInB0Index] = n;
    nFilterInB0Index++;
    Serial.print("filtering in B0: ");
    Serial.println (n,HEX);
  }
  if (szCommand.startsWith("showChangeOnly"))
  {
    
    Serial.print("ShowChangeOnly: ");
    bShowChangeOnly = !bShowChangeOnly;
    Serial.print (bShowChangeOnly);
    
  }
  if (szCommand.startsWith("parseRaw"))
  {

    //parseRaw();
  }
  
  
  if (szCommand.startsWith("reset"))
  {
    ESP.restart();
  }
  
  
  if (szCommand.startsWith ("quickSSID "))
  {
    Serial.println();
    int index1 = szCommand.indexOf(" ") + 1;
    int index2 = szCommand.indexOf(",") + 1;
    String s = szCommand.substring(index1,index2-1);
    String p = szCommand.substring(index2);
    char szSSID[32];
    char szPassword[32];
    
    s.toCharArray(szSSID,sizeof(szSSID));
    p.toCharArray(szPassword,sizeof(szPassword));
    Serial.println(szSSID);
    Serial.println(szPassword);
    if (WiFi.begin(szSSID,szPassword))
    {
      long startConnectTime = millis();
      Serial.println("quickAP launching");
      while (WiFi.status() != WL_CONNECTED)
      {      
          Serial.print(".");
          delay(250);
          if (millis() - startConnectTime > 10000) 
          {
            apFound = false;
            Serial.println("quickSSID fail");
            return;
          }
      }
      apFound = true;
      Serial.println("quickSSID successful");
    }
    
    
  }
  if (szCommand.startsWith("wifi "))
  {
    
    if (connectionsIndex < MAX_CONNECTIONS)
    {
      int index1 = szCommand.indexOf(" ") + 1;
    
      int index2 = szCommand.indexOf(",") + 1;
      String s = szCommand.substring(index1,index2-1);
      
      String p = szCommand.substring(index2);
      s.toCharArray(connections[connectionsIndex].ssid,sizeof(connections[connectionsIndex].ssid));
      p.toCharArray(&connections[connectionsIndex].password[0],sizeof(connections[connectionsIndex].password));
      //Serial.println(szCommand);
      Serial.print("Adding WIFI:");
      Serial.print(s);
      Serial.print(",");
      Serial.println(p);
      connectionsIndex++;
    }
    
  }
  if (szCommand.startsWith ("webOn")) bUploadDataToWeb = true;
  if (szCommand.startsWith ("webOff")) bUploadDataToWeb = false;
  if (szCommand == "start")
  {
    bReadBus = true;
    return;
  }
  if (szCommand == "stop")
  {
    nPDMChannel = 0;
    nPDMToPrint = 0;
    
    bReadBus = false;
    return;
  }
  
  
  
  
  
  
  
}

void handleSerial()
{
  static char currentCommand[32];
  static int cIndex = 0;
  
  if (!Serial.available()) return;
  char ch = Serial.read();
  
  
  if (ch != 0x0a)
  {
    currentCommand[cIndex] = ch;
    cIndex++;
  }
  if (cIndex>=sizeof(currentCommand)) 
  {
    Serial.println("Serial Error");
    return;
  }
  
  if (ch == '\r') 
  {
    currentCommand[cIndex - 1] = 0;
    cIndex = 0;
    Serial.println(currentCommand);
    handleCommand(currentCommand); 
  }
}

void setupSPIFFS()
{
  displayMessage("Opening Spiffs");
  
  if (!SPIFFS.begin(true))
  {
    
    displayMessage("SPIFFS FAIL");
    while (true)
    {
    
    }
    
  }
  displayMessage("Spiffs Open");
  return;
  File root = SPIFFS.open("/");
  delay(100);
  Serial.println("--------");
  Serial.println("READING ROOT..");
  File file = root.openNextFile();
  
  while(file)
  {
    
      Serial.print("FILE: ");
      Serial.println(file.name());
      file = root.openNextFile();
      delay(50);
  }
  displayMessage("SPIFFS OK");
 
  
}


void setupConfig()
{
  Serial.println("Init Config File");
  Serial.println("reading config");
  parseFile("config.txt");
  Serial.println("done with setup config");
}




bool dataChanged (can_frame m)
{
  static can_frame mLast;
  bool dataChanged = false;
  byte mask = bChangeMask;        //E.G.1000 0010
  for (int i = m.can_dlc;i<8;i++) m.data[i] = 0; //clear out the end stoff so changes can be seen
  for (int i = 7;i>=0;i--)
  {
    if ((mask & 1) == 1)
    {
      if (m.data[i] != mLast.data[i]) 
      {
        dataChanged = true;
        mLast = m;
        return true;
      }
    }
    mask = mask >> 1;
  }
  return false;
  
}

void handleCanbus ()
{
  static can_frame mLast;
  struct can_frame m;
  static long startVerboseTimer = 0;
  
  if (!bReadBus) return;
  
  if (mcp2515.readMessage(&m) == MCP2515::ERROR_OK)
  {
    float t = millis() / 1000.0;
    
    
    //now filter stuff
    if (nFilterMode == 1)
    {
      if (!filterInMsg(m.can_id)) return;
      if (!filterInByte0(m.data[0])) return;
      
    }
    if (nFilterMode == 2)
    {
      if (filterOutMsg(m.can_id)) return;
      if (filterOutByte0(m.data[0])) return;
      
    }
    
    
    
    if (bShowChangeOnly)
    {
      if (!dataChanged (m)) return;
    }
    ////Do the stuff
    if (bVerbose) cm.printCan(m,false);
    
    if ((m.can_id == PDM1_MESSAGE) || (m.can_id == PDM2_MESSAGE)) 
    {
      handlePDMMessage(t,m);
      return;
    }
    if ((m.can_id == PDM1_SHORT) || (m.can_id == PDM2_SHORT))
    {
      handlePDMShort(m);
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
bool filterInMsg (long drg)
{
  for (int i = 0;i<nFilterInIndex;i++)
  {
    if (filterIn[i] == drg) return true;
  }
  return false;
}
bool filterOutMsg (long drg)
{
  for (int i = 0;i<nFilterOutIndex;i++)
  {
    if (filterOut[i] == drg) return true;
  }
  return false;
}
bool filterOutByte0 (byte b)
{
  for (int i = 0;i<nFilterOutB0Index;i++)
  {
    if (filterOutB0[i] == b) return true;
  }
  return false;
}
bool filterInByte0 (byte b)
{
  for (int i = 0;i<nFilterInB0Index;i++)
  {
    if (filterInB0[i] == b) return true;
  }
  return false;
}
 






float tenBitAnalog(can_frame m,int channelNumber)
{
  int nByteOffset = 0;
  if ((channelNumber == 1) || (channelNumber == 3) || (channelNumber == 5) || (channelNumber == 7)) nByteOffset = 4;
  if ((channelNumber == 2) || (channelNumber == 4) || (channelNumber == 6) || (channelNumber == 8)) nByteOffset = 6;
  
   
  long l = m.data[nByteOffset] + ((m.data[nByteOffset + 1] & 0b00000011) << 8);
  float fRet = (float)l * 0.00488759;
  return fRet;
}
float feedbackAmps(can_frame m,int nChannelNumber)
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

void printInputs(byte b)
{
  /*
   * 00 Open Circuit
01 Short-to-ground
10 Short-to-battery
11 Not Available
   */
  for (int i = 0; i <=3; i++)
  {
    byte bTemp = (b & 0b11000000) >> 6;
    if (bTemp == 0) if (bVerbose) Serial.print("OP ");
    if (bTemp == 1) if (bVerbose) Serial.print("S- ");
    if (bTemp == 2) if (bVerbose) Serial.print("S+ ");
    if (bTemp == 3) if (bVerbose) Serial.print("?? ");
    b = b << 2;
  }
}

char getDigitalInput (can_frame m,int nInputNumber)
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

void printInputDiagnostics(byte b)
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

void handleMessage128 (float t,can_frame m)
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
void handleMessage129 (float t,can_frame m)
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
void handleMessage130 (float t,can_frame m)
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
void handleMessage131 (float t,can_frame m)
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
 void handleFeedback1to6 (float t,can_frame m)            //PDM1 Feedback Amps
 {
  if (m.can_id == PDM1_MESSAGE)
  { 
    if (bVerbose) Serial.println("Feedback PDM1 1-6");
    for (int i = 1;i<=6;i++) cm.pdm1_output[i].fFeedback = feedbackAmps(m,i);
    
    
  }
  if (m.can_id == PDM2_MESSAGE)
  {
    if (bVerbose) Serial.println("Feedback PDM2 1-6");
    for (int i = 1;i<=6;i++) cm.pdm2_output[i].fFeedback = feedbackAmps(m,i);
    
    
  }

 }
 
void handleFeedback7to12 (float t,can_frame m)
{
  
  if (m.can_id == PDM1_MESSAGE)
  {
    if (bVerbose) Serial.println("Feedback PDM1 7-12");
    for (int i = 7;i<= 12;i++) cm.pdm1_output[i].fFeedback = feedbackAmps(m,i);
    
  }
  if (m.can_id == PDM2_MESSAGE)
  {
    if (bVerbose) Serial.println("Feedback PDM2 7-12");
    for (int i = 7;i<=12;i++) cm.pdm2_output[i].fFeedback = feedbackAmps(m,i);
    
  }
  
    
}
void handleMessage134 (float t,can_frame m)
{
    if (bVerbose) Serial.print(" 134) ");
    if (bVerbose) Serial.print("channel: ");
    if (bVerbose) Serial.print(m.data[1] & 0b1111);
    if (bVerbose) Serial.println();
}
void handleMessage135 (float t,can_frame m)
{
  if (bVerbose) Serial.print(" 135) ");
  if (bVerbose) Serial.println();
   
}
void handleMessage136 (float t,can_frame m)
{
  if (bVerbose) Serial.print(" 136) ");
  if (bVerbose) Serial.println();
}



void handleEngineOnAllOff(can_frame m)
{
  static byte lastB6 = 10;
  if (m.can_id != PDM1_MESSAGE) return;
  if (m.data[0] != 0xf8) return;
  byte b6 = m.data[6];
  //if (b6 == 0x20) Serial.print("!");else Serial.print(" ");
  
  //cm.printCan (m,false);Serial.print("B6: ");Serial.println(b6,HEX);
  if ((b6 == 0x20) and (lastB6 != 0x20))
  {
    Serial.println("ENGINE JUST TURNED ON!!");
    handleCommand("acOff");
    handleCommand("closeVent");
    handleCommand("allOffXAux");
    handleCommand("awningEnable");
    handleCommand("awningIn");
    
    exit;
  }
  lastB6 = b6;
  return;
  
  
}
void handleFrontButtonDoubleTap(can_frame m)
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
      cm.acCommand(0,0,0);
      delay(500);
      cm.closeVent();
      delay(500);
      handleCommand("allOff");
      handleCommand("awningEnable");
      handleCommand("awningIn");
      handleCommand("acOff");
      handleCommand("closeVent");
      
      tapState = 0;
    }
  }
  
}
void handlePDMDigitalInput (float t,can_frame m)         //This is ambient voltage and digital inputs
{
  
  if (m.can_id == PDM1_MESSAGE)
  {
    handleEngineOnAllOff(m);
    if (bVerbose) Serial.print("PDM1 ");
    if (m.data[0] == 0xf0) 
    {
      cm.lastpdm1inputs1to6 = m;
      if (bVerbose) Serial.print(" Button Saved 1-6 ");
    }
    
    if (m.data[0] == 0xf8) 
    {
      cm.lastpdm1inputs7to12 = m;
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
      cm.lastpdm2inputs1to6 = m;
      if (bVerbose) Serial.print(" 1-6 ");
    }
    if (m.data[0] == 0xf8) 
    {
      cm.lastpdm2inputs7to12 = m;
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

    
  if (nPDMToPrint == 1) Serial.println(cm.pdm1_output[nPDMChannel].fFeedback);
  if (nPDMToPrint == 2) Serial.println(cm.pdm2_output[nPDMChannel].fFeedback);
  
    
  float fTemp = (m.data[4] & 0b11) * 256.0 + m.data[5];
  cm.fAmbientVoltage = fTemp * 5.0 / 1024.0;
  if (bVerbose) Serial.print(" ");
  if (bVerbose) Serial.print(cm.fAmbientVoltage);
  if (bVerbose) Serial.println("V");
    
}
void handlePDMShort (can_frame m)
{
  
  if ((m.data[2] != 0) || (m.data[3] != 0))
  {
    Serial.print("***PDM SHORT FIRED!*** ");
    cm.printCan(m,false);
    
  }
  if (bVerbose) Serial.println();
}

void handleSupplyVoltage (can_frame m)
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
void handleHeartBeat (can_frame m)
{
    if (bVerbose) Serial.println("PDM ID");
    return; 
}
void handlePDMMessage (float t,can_frame m)
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
    return;                                           //94EF111E           
  }
  if ((m0 == 0x0a) || (m0 == 0xCA) || (m0 == 0xFA))
  {
    handleFeedback7to12(t,m);                            //Output feedback.  One of the bytes seems to become 1
    return;                                           //94EF111E           
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
  cm.printCan(m,false);
  Serial.println ("? PDM Message: ");
  
  
  
  
  
  return;
  
}


void setupCanbus ()
{
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
    setupSPIFFS();
   
    
    Serial.println();
    Serial.println("Initializing...");
    Serial.flush();

   
    displayMessage("Config...");
    setupConfig();
     
    
    
    bool wifiOK = setupWiFi();
    if (apFound || wifiOK) setupServer();
  
    
    delay(100);   //important...
    displayMessage("Running...");
    cm.init(&mcp2515);
    pinMode(LED,OUTPUT);
    setupCanbus();

    
    
    
    setWebVariable("currentTemperature",(float)69.2);
    //parseRaw();
    
}

void handleUploadData()
{
  static long lastUpload = millis();
  if (millis() - lastUpload >5000)
  {
    lastUpload = millis();
    //setWebVariable("currentTemperature",cm.fAmbientTemp);
    postCurrentState();
  }
}


void loop()
{
 
   handleSerial();
   server.handleClient();
   handleCanbus();
   handleUploadData();
   cm.handleCabinBlink();
   
   ///////////////SET LED TO BLINK IF ACCESS POINT MODE IS TRUE
   if (apMode == true)
   {
      if ((millis() % 2000) < 1000) digitalWrite(LED,LOW);
        else digitalWrite (LED,HIGH);
      
   }
   
}

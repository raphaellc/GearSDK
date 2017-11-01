#include "declarations.h"
#include "Gear_Server.h"

void ReceivedMessage(JsonObjectType type, JsonObject& root);

void InitOjbects()
{
    g_mpu.init();
    button_0.init();
    
    rgbLed.init();
    rgbLed.SetColor(1023,256,0);
    
    pot.init();
    
    wifi.InitWiFi();
}

String MountJSONHeader()
{
    jsonHeader = "{\"header\":";
    jsonHeader = jsonHeader + "{\"buttons\":[";
    jsonHeader = jsonHeader + button_0.GetHeader() + "],";

    jsonHeader = jsonHeader + "\"potentiometers\":[";
    jsonHeader = jsonHeader + pot.GetHeader() + "],";

    jsonHeader = jsonHeader + "\"rgb_leds\":[";
    jsonHeader = jsonHeader + rgbLed.GetHeader() + "],";

    jsonHeader = jsonHeader + "\"mpus\":[";
    jsonHeader = jsonHeader + g_mpu.GetHeader() + "]";
    
    jsonHeader = jsonHeader + "}}";
    
    return jsonHeader;
}

void SendObjects()
{
    String message = "";
    String s = button_0.updatedData();
    message = message + s;

    s = pot.updatedData();
    message = message + "@" + s;

    s = rgbLed.updatedData();
    message = message + "@" + s;

    s = g_mpu.updatedData();
    message = message + "@" +  s;

    if(message != ""){
        webSocket.sendTXT(0, message);
    }
}

void ReceivedMessage(JsonObjectType type, JsonObject& root)
{
    switch(type)   
    {
        case JsonObjectType::RGB_LED:
        {
            int v1 = (root)["rgb_led"]["value"]["r"];
            int v2 = (root)["rgb_led"]["value"]["g"];
            int v3 = (root)["rgb_led"]["value"]["b"];

            int mode = (root)["rgb_led"]["mode"];

            rgbLed.SetColor(v1,v2,v3);
            rgbLed.SetMode((LedMode)mode, 800, 200);
            break;
        }
        default:
            break;
    }
}

void setup() 
{
    Serial.begin(115200);

    for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }

    InitOjbects();

    ConfigWebServer();

    rgbLed.SetColor(0,1023,0);
    rgbLed.SetMode(LedMode::BLINKING, 800,200);
}

void loop() 
{
    g_mpu.readRawMPU();
    g_mpu.CalculateAngles();

    rgbLed.update();

    if(!headerReceivedByClient)
    {
        webSocket.sendTXT(0, jsonHeader);
    }
    else
    { 
        SendObjects();
        delay(40);
    }

    WebServerLoop();
}

void webSocketEvent(uint8_t num, int type, uint8_t* payload, size_t length) 
{
    switch(type) 
    {
        case WStype_DISCONNECTED:{
            Serial.printf("[%u] Disconnected!\n", num);
            rgbLed.SetColor(1023,0,0);
            break;
        }
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

            // send message to client
            webSocket.sendTXT(num, "Hi, software :)");
            
            //Reset the boolean to control the header
            headerReceivedByClient = false;

            //SET HEADER TO send
            MountJSONHeader();

            break;
        }
        case WStype_TEXT:{
            //Serial.printf("[%u] get Text: %s\n", num, payload);
            if(payload[0] == '_')
            {
                // webSocket.sendTXT(num, parserJSON);
                webSocket.sendTXT(num,"hi");
            }
            else if(payload[0] == '@') //Header Handshake
            {
                headerReceivedByClient = true;
            }
            else if(payload[0] == '{') { //JSON
               
                StaticJsonBuffer<350> jsonBuffer;
                JsonObject& root = jsonBuffer.parseObject(payload);
    
                String sJson = "";
                root.printTo(sJson);

                int pntIndex = sJson.indexOf(":");

                if(pntIndex > 0)
                {
                    String jsonType = sJson.substring(2, pntIndex-1);  
                    if(jsonType == "rgb_led")  
                        ReceivedMessage(JsonObjectType::RGB_LED, root);
                }
            }
            break;
        }
        case WStype_BIN:{
            Serial.printf("[%u] get binary length: %u\r\n", num, length);
            hexdump(payload, length);
      
            // echo data back to browser
            //webSocket.sendBIN(num, payload, length);
            break;
        }
        default:{
            Serial.printf("Invalid WStype [%d]\r\n", type);
            break;
        }
    }
}
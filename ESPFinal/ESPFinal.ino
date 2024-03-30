//////////////////////////////////////////////////
///////// MQTT ESP8266 TEST 2022 03 21
//////////////////////////////////////////////////


#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "ArduinoJson.h"

#define LED_PIN_g 0
#define LED_PIN_y 4
#define LED_PIN_r 5

#define PWMRANGE 255

// Update these with values suitable for your VM.
#define MQTT_ID  "ESP8266_2022_31"
//const char* mqtt_server = "192.168.1.3";
const char* mqtt_server = "10.6.0.24";                    ///////FINAL IP
//const char* mqtt_server = "broker.hivemq.com";              /////// Test IP
#define intopic  "esp_in_sec"
#define outtopic  "esp_out_sec"


void connect(void);
int print_AD(void);
void setup_wifi(void);
void callback(char* topic, byte* payload, unsigned int length);



// Update these with values suitable for your network.
const char* ssid = "SOURCE";                                //////FINAL INTERNET
const char* password = "Pelle!23";

WiFiClient espClient;
PubSubClient client(espClient);

DynamicJsonDocument doc(100);

////////////////////////////////////////////////////////////////////// setup start
void setup() {

  Serial.begin(115200);

  //led set up
  pinMode(LED_PIN_g, OUTPUT);
  pinMode(LED_PIN_y, OUTPUT);
  pinMode(LED_PIN_r, OUTPUT);


  Serial.println("START 2022");

  delay(1000);


  setup_wifi();                               // init Wifi
  client.setServer(mqtt_server, 1883);        // set server IP and port number
  client.setCallback(callback);               // define receive callback

  connect();                                  // connect to mqtt server


}
///////////////////////////////////////////////////////////////////////   setup end

///////////////////////////////////////////////////////////////////////   loop start
void loop()
{

  if (!client.connected()) {
    connect();   // reconnect if needed
  }

  while (1)

  {
    client.loop();
    char msg[50];
    
    if (Serial.available() > 0)
    {
      String tmpMsg = Serial.readStringUntil('\n');

      if (tmpMsg.indexOf('=') != -1 )
      {
        int res_com = tmpMsg.indexOf("Com=");
        if (tmpMsg.indexOf("State=") > -1) {
          tmpMsg = toJSON(tmpMsg);
          tmpMsg.toCharArray(msg, 50);
          client.publish(outtopic, msg);
        }
        else if (tmpMsg.indexOf("Lid=") > -1) {
          tmpMsg = toJSON(tmpMsg);
          tmpMsg.toCharArray(msg, 50);
          client.publish(outtopic, msg);
        }
        else if (tmpMsg.indexOf("Com=") > -1) {
          tmpMsg = toJSON(tmpMsg);
          tmpMsg.toCharArray(msg, 50);
          client.publish(outtopic, msg);
        }
        else if (tmpMsg.indexOf("Dire=") > -1) {
          tmpMsg = toJSON(tmpMsg);
          tmpMsg.toCharArray(msg, 50);
          client.publish(outtopic, msg);
        }
        else if (tmpMsg.indexOf("Dist=") > -1) {
          tmpMsg = toJSON(tmpMsg);
          tmpMsg.toCharArray(msg, 50);
          client.publish(outtopic, msg);
        }
      }
    }

    //sprintf (msg, "%s", color);

    //    int sum = analogRead(A0);
    //    String color = "";
    //
    //    Serial.print(" AD converter= ");  Serial.println(sum);
    //
    //    if (sum <= 300) {
    //      color = "red";
    //    }
    //    else if (sum >= 601) {
    //      color = "green";
    //    }
    //    else {
    //      color = "yellow";
    //    }
    //
    //    char msg[50];
    //    color.toCharArray(msg, 50);
    //
    //
    //    //char msg[50] = "My message";
    //    //   012345678901234567890
    //    // sprintf (msg, "%s", color);
    //
    //    Serial.print("Publish message: "); Serial.println(msg);
    //
    //    client.publish(outtopic, msg);

  }  ////         while end
}  /////////////////////////////////////////////////////////////         LOOP END



String toJSON(String tmpMsg) {
  String JSONmsg;
  int pos1 = tmpMsg.indexOf("=");
  int len = tmpMsg.length();
  String COM, VAL;

  COM = tmpMsg.substring(0, pos1);
  VAL = tmpMsg.substring(pos1 + 1);

  JSONmsg = "{\"" + COM + "\":" + VAL + '}';
  if(COM=="State")JSONmsg = "{\"" + COM + "\":\"" + VAL + "\"}";;

  return JSONmsg;
}

////////////////////////////////////////////////////////////////         setup WIFI
void setup_wifi()
{

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());                       ////////   Print IP number
}///////////////////////////////////////////////////////////////   setup WIFI end


/////////////////////////////////////////////////////////////// callback when message arrive
void callback(char* topic, byte * payload, unsigned int length)
{

  Serial.print("Message arrived :");
  char tp[30];
  snprintf(tp, length, "%s", payload);
  Serial.println(tp);


  parse_msg(payload);


}


///////////////////////////////////////////////////////////////////  connect to MQTT broker
void connect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_ID)) {
      Serial.println("connected to MQTT broker");
      client.subscribe(intopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}




void parse_msg(uint8_t *data)                                               // parse message
{
  Serial.println("Parsing start: ");

  DeserializationError error = deserializeJson(doc, data);                // parse json message

  if (error) {                                                            // Test if parsing succeeds.
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  JsonObject root = doc.as<JsonObject>();                                 //

  String com;                                                             // (JSON) name ( =command)
  double val;                                                             // (JSON) value

  for (JsonPair kv : root)
  {
    com = (kv.key().c_str());
    val = kv.value().as<double>();
  }

  Serial.print(com); Serial.print(":"); Serial.println(val);

  if (val == 1) {
    digitalWrite(LED_PIN_r, 1);
    digitalWrite(LED_PIN_g, 0);
    digitalWrite(LED_PIN_y, 0);
  }
  else if (val == 2) {
    digitalWrite(LED_PIN_r, 0);
    digitalWrite(LED_PIN_g, 0);
    digitalWrite(LED_PIN_y, 1);
  }
  else if (val == 3) {
    digitalWrite(LED_PIN_r, 0);
    digitalWrite(LED_PIN_g, 1);
    digitalWrite(LED_PIN_y, 0);
  }
  else {
    digitalWrite(LED_PIN_r, 0);
    digitalWrite(LED_PIN_g, 0);
    digitalWrite(LED_PIN_y, 0);
  }

} // end parse message

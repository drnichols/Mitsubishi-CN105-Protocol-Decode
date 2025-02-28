
#define MQTT_BASETOPIC "Ecodan/ASHP"
#define MQTT_LWT MQTT_BASETOPIC "/LWT"

#define MQTT_STATUS MQTT_BASETOPIC "/Status"
#define MQTT_COMMAND MQTT_BASETOPIC "/Command"

#define MQTT_STATUS_ZONE1 MQTT_STATUS "/Zone1"
#define MQTT_STATUS_ZONE2 MQTT_STATUS "/Zone2"
#define MQTT_STATUS_HOTWATER MQTT_STATUS "/HotWater"
#define MQTT_STATUS_SYSTEM MQTT_STATUS "/System"
#define MQTT_STATUS_TEST MQTT_STATUS "/Test"
#define MQTT_STATUS_ADVANCED MQTT_STATUS "/Advanced"
#define MQTT_STATUS_WIFISTATUS MQTT_STATUS "/WiFiStatus"

#define MQTT_COMMAND_ZONE1 MQTT_COMMAND "/Zone1"
#define MQTT_COMMAND_ZONE2 MQTT_COMMAND "/Zone2"
#define MQTT_COMMAND_HOTWATER MQTT_COMMAND "/HotWater"
#define MQTT_COMMAND_SYSTEM MQTT_COMMAND "/System"

#define MQTT_COMMAND_ZONE1_TEMP_SETPOINT MQTT_COMMAND_ZONE1 "/TempSetpoint"
#define MQTT_COMMAND_ZONE1_FLOW_SETPOINT MQTT_COMMAND_ZONE1 "/FlowSetpoint"
#define MQTT_COMMAND_ZONE1_CURVE_SETPOINT MQTT_COMMAND_ZONE1 "/CurveSetpoint"

#define MQTT_COMMAND_ZONE2_TEMP_SETPOINT MQTT_COMMAND_ZONE2 "/TempSetpoint"
#define MQTT_COMMAND_ZONE2_FLOW_SETPOINT MQTT_COMMAND_ZONE2 "/FlowSetpoint"
#define MQTT_COMMAND_ZONE2_CURVE_SETPOINT MQTT_COMMAND_ZONE2 "/CurveSetpoint"

#define MQTT_COMMAND_SYSTEM_HEATINGMODE MQTT_COMMAND_SYSTEM "/HeatingMode"
#define MQTT_COMMAND_HOTWATER_SETPOINT MQTT_COMMAND_HOTWATER "/Setpoint"
#define MQTT_COMMAND_HOTWATER_BOOST MQTT_COMMAND_HOTWATER "/Boost"
#define MQTT_COMMAND_SYSTEM_POWER MQTT_COMMAND_SYSTEM "/Power"
#define MQTT_COMMAND_SYSTEM_TEMP MQTT_COMMAND_SYSTEM "/Temp"


String MQTTCommandZone1TempSetpoint = MQTT_COMMAND_ZONE1_TEMP_SETPOINT;
String MQTTCommandZone1FlowSetpoint = MQTT_COMMAND_ZONE1_FLOW_SETPOINT;
String MQTTCommandZone1CurveSetpoint = MQTT_COMMAND_ZONE1_CURVE_SETPOINT;

String MQTTCommandZone2TempSetpoint = MQTT_COMMAND_ZONE2_TEMP_SETPOINT;
String MQTTCommandZone2FlowSetpoint = MQTT_COMMAND_ZONE2_FLOW_SETPOINT;
String MQTTCommandZone2CurveSetpoint = MQTT_COMMAND_ZONE2_CURVE_SETPOINT;

String MQTTCommandHotwaterSetpoint = MQTT_COMMAND_HOTWATER_SETPOINT;
String MQTTCommandHotwaterBoost = MQTT_COMMAND_HOTWATER_BOOST;

String MQTTCommandSystemHeatingMode = MQTT_COMMAND_SYSTEM_HEATINGMODE;
String MQTTCommandSystemPower = MQTT_COMMAND_SYSTEM_POWER;
String MQTTCommandSystemTemp = MQTT_COMMAND_SYSTEM_TEMP;

String HostName;


void readSettingsFromConfig() {
  // Clean LittleFS for testing
  //LittleFS.format();

  // Read configuration from LittleFS JSON
  DEBUG_PRINTLN("Mounting File System...");
  if (LittleFS.begin()) {
    DEBUG_PRINTLN("Mounted File System");
    if (LittleFS.exists("/config.json")) {
      //file exists, reading and loading
      DEBUG_PRINTLN("Reading config file");
      File configFile = LittleFS.open("/config.json", "r");
      if (configFile) {
        DEBUG_PRINTLN("Opened config file");
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, configFile);
        if (error) {
          DEBUG_PRINT("Failed to read file: ");
          DEBUG_PRINTLN(error.c_str());
        } else {
          DEBUG_PRINTLN("Parsed JSON: ");
          serializeJson(doc, Serial);
          DEBUG_PRINTLN();

          // Build in safety check, otherwise ESP will crash out and you can't get back in
          if (doc.containsKey(mqttSettings.wm_mqtt_client_id_identifier)) {
            if ((sizeof(doc[mqttSettings.wm_mqtt_client_id_identifier]) > 0) && ((sizeof(doc[mqttSettings.wm_mqtt_client_id_identifier]) + 1) <= clientId_max_length)) {
              strcpy(mqttSettings.clientId, doc[mqttSettings.wm_mqtt_client_id_identifier]);
            }
          }
          if (doc.containsKey(mqttSettings.wm_mqtt_hostname_identifier)) {
            if ((sizeof(doc[mqttSettings.wm_mqtt_hostname_identifier]) > 0) && ((sizeof(doc[mqttSettings.wm_mqtt_hostname_identifier]) + 1) <= hostname_max_length)) {
              strcpy(mqttSettings.hostname, doc[mqttSettings.wm_mqtt_hostname_identifier]);
            }
          }
          if (doc.containsKey(mqttSettings.wm_mqtt_port_identifier)) {
            if ((sizeof(doc[mqttSettings.wm_mqtt_port_identifier]) > 0) && ((sizeof(doc[mqttSettings.wm_mqtt_port_identifier]) + 1) <= port_max_length)) {
              strcpy(mqttSettings.port, doc[mqttSettings.wm_mqtt_port_identifier]);
            }
          }
          if (doc.containsKey(mqttSettings.wm_mqtt_user_identifier)) {
            if ((sizeof(doc[mqttSettings.wm_mqtt_user_identifier]) > 0) && ((sizeof(doc[mqttSettings.wm_mqtt_user_identifier]) + 1) <= user_max_length)) {
              strcpy(mqttSettings.user, doc[mqttSettings.wm_mqtt_user_identifier]);
            }
          }
          if (doc.containsKey(mqttSettings.wm_mqtt_password_identifier)) {
            if ((sizeof(doc[mqttSettings.wm_mqtt_password_identifier]) > 0) && ((sizeof(doc[mqttSettings.wm_mqtt_password_identifier]) + 1) <= password_max_length)) {
              strcpy(mqttSettings.password, doc[mqttSettings.wm_mqtt_password_identifier]);
            }
          }
        }
      }
      configFile.close();
    } else {
      DEBUG_PRINTLN("No config file exists, use placeholder values");
    }
  } else {
    DEBUG_PRINTLN("Failed to mount File System");
  }
}

void saveConfig() {
  // Read MQTT Portal Values for save to file system
  DEBUG_PRINTLN("Copying Portal Values...");
  strcpy(mqttSettings.clientId, custom_mqtt_client_id.getValue());
  strcpy(mqttSettings.hostname, custom_mqtt_server.getValue());
  strcpy(mqttSettings.port, custom_mqtt_port.getValue());
  strcpy(mqttSettings.user, custom_mqtt_user.getValue());
  strcpy(mqttSettings.password, custom_mqtt_pass.getValue());

  DEBUG_PRINT("Saving config... ");
  File configFile = LittleFS.open("/config.json", "w");
  if (!configFile) {
    DEBUG_PRINTLN("[FAILED] Unable to open config file for writing");
  } else {
    JsonDocument doc;
    doc[mqttSettings.wm_mqtt_client_id_identifier] = mqttSettings.clientId;
    doc[mqttSettings.wm_mqtt_hostname_identifier] = mqttSettings.hostname;
    doc[mqttSettings.wm_mqtt_port_identifier] = mqttSettings.port;
    doc[mqttSettings.wm_mqtt_user_identifier] = mqttSettings.user;
    doc[mqttSettings.wm_mqtt_password_identifier] = mqttSettings.password;

    if (serializeJson(doc, configFile) == 0) {
      DEBUG_PRINTLN("[FAILED]");
    } else {
      DEBUG_PRINTLN("[DONE]");
      serializeJson(doc, Serial);
      DEBUG_PRINTLN();
    }
  }
  configFile.close();
  shouldSaveConfig = false;
}

//callback notifying us of the need to save config
void saveConfigCallback() {
  saveConfig();
}

void initializeWifiManager() {
  DEBUG_PRINTLN("Starting WiFi Manager");
  // Reset Wifi settings for testing
  //wifiManager.resetSettings();

  // Set or Update the values
  custom_mqtt_client_id.setValue(mqttSettings.clientId, clientId_max_length);
  custom_mqtt_server.setValue(mqttSettings.hostname, hostname_max_length);
  custom_mqtt_port.setValue(mqttSettings.port, port_max_length);
  custom_mqtt_user.setValue(mqttSettings.user, user_max_length);
  custom_mqtt_pass.setValue(mqttSettings.password, password_max_length);

  // Add the custom MQTT parameters here
  wifiManager.addParameter(&custom_mqtt_client_id);
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_pass);

  //set minimum quality of signal so it ignores AP's under that quality
  //defaults to 8%
  //wifiManager.setMinimumSignalQuality();

  HostName = "EcodanBridge-";
  HostName += String(ESP.getChipId(), HEX);
  WiFi.hostname(HostName);

  wifiManager.setConfigPortalTimeout(120);                // Timeout before launching the config portal
  wifiManager.setBreakAfterConfig(true);                  // Saves settings, even if WiFi Fails
  wifiManager.setSaveConfigCallback(saveConfigCallback);  // Set config save notify callback

  if (!wifiManager.autoConnect("Ecodan Bridge AP")) {
    DEBUG_PRINTLN("Failed to connect and hit timeout");
    delay(3000);
    ESP.reset();
  }

  DEBUG_PRINTLN("WiFi Connected!");
  wifiManager.startWebPortal();
}


void initializeMqttClient() {
  DEBUG_PRINT("Attempting MQTT connection to: ");
  DEBUG_PRINT(mqttSettings.hostname);
  DEBUG_PRINT(":");
  DEBUG_PRINTLN(mqttSettings.port);
  MQTTClient.setServer(mqttSettings.hostname, atoi(mqttSettings.port));
}


void MQTTonConnect(void) {
  DEBUG_PRINTLN("MQTT ON CONNECT");
  MQTTClient.publish(MQTT_LWT, "online");
  delay(10);

  MQTTClient.subscribe(MQTTCommandZone1TempSetpoint.c_str());
  MQTTClient.subscribe(MQTTCommandZone1FlowSetpoint.c_str());
  MQTTClient.subscribe(MQTTCommandZone1CurveSetpoint.c_str());
  MQTTClient.subscribe(MQTTCommandZone2TempSetpoint.c_str());
  MQTTClient.subscribe(MQTTCommandZone2FlowSetpoint.c_str());
  MQTTClient.subscribe(MQTTCommandZone2CurveSetpoint.c_str());
  MQTTClient.subscribe(MQTTCommandSystemHeatingMode.c_str());
  MQTTClient.subscribe(MQTTCommandHotwaterSetpoint.c_str());
  MQTTClient.subscribe(MQTTCommandHotwaterBoost.c_str());
  MQTTClient.subscribe(MQTTCommandSystemPower.c_str());
  MQTTClient.subscribe(MQTTCommandSystemTemp.c_str());
}


uint8_t MQTTReconnect() {
  if (MQTTClient.connected()) {
    return 1;
  }

  DEBUG_PRINT("With Client ID: ");
  DEBUG_PRINT(mqttSettings.clientId);
  DEBUG_PRINT(", Username: ");
  DEBUG_PRINT(mqttSettings.user);
  DEBUG_PRINT(" and Password: ");
  DEBUG_PRINTLN(mqttSettings.password);

  if (MQTTClient.connect(mqttSettings.clientId, mqttSettings.user, mqttSettings.password, MQTT_LWT, 0, true, "offline")) {
    DEBUG_PRINTLN("MQTT Server Connected");
    MQTTonConnect();
    digitalWrite(Red_RGB_LED, LOW);     // Turn off the Red LED
    digitalWrite(Green_RGB_LED, HIGH);  // Flash the Green LED
    delay(10);
    digitalWrite(Green_RGB_LED, LOW);
    return 1;
  } else {
    DEBUG_PRINT("Failed with Error Code: ");
    DEBUG_PRINTLN(MQTTClient.state());
    switch (MQTTClient.state()) {
      case -4:
        DEBUG_PRINTLN("MQTT_CONNECTION_TIMEOUT");
        break;
      case -3:
        DEBUG_PRINTLN("MQTT_CONNECTION_LOST");
        break;
      case -2:
        DEBUG_PRINTLN("MQTT_CONNECT_FAILED");
        break;
      case -1:
        DEBUG_PRINTLN("MQTT_DISCONNECTED");
        break;
      case 0:
        DEBUG_PRINTLN("MQTT_CONNECTED");
        break;
      case 1:
        DEBUG_PRINTLN("MQTT_CONNECT_BAD_PROTOCOL");
        break;
      case 2:
        DEBUG_PRINTLN("MQTT_CONNECT_BAD_CLIENT_ID");
        break;
      case 3:
        DEBUG_PRINTLN("MQTT_CONNECT_UNAVAILABLE");
        break;
      case 4:
        DEBUG_PRINTLN("MQTT_CONNECT_BAD_CREDENTIALS");
        break;
      case 5:
        DEBUG_PRINTLN("MQTT_CONNECT_UNAUTHORIZED");
        break;
    }
    return 0;
  }
  return 0;
}


void handleMqttState() {
  if (!MQTTClient.connected()) {
    analogWrite(Green_RGB_LED, 30);   // Green LED on, 25% brightness
    digitalWrite(Red_RGB_LED, HIGH);  // Add the Red LED to the Green LED = Orange
    MQTTReconnect();
  }
  MQTTClient.loop();
}
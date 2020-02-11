#include <IRremote.h>

#include <EEPROM.h>
#include <WiFi.h>
#include <WebServer.h>
#include "controllers.h"

/* global variables */
const char* ssid = "ESP32_AP";
const char* password = "xaubre02_xbedna62";
WebServer server(80);

/* IR receiver is connected to GPIO15 */
int RECV_PIN = 15;
IRrecv irrecv(RECV_PIN);
/* IR diode is connected to GPIO5 */
IRsend  irsend;


void generateSignal(int code) {
    irsend.sendNEC(code, 32);
}

int storeSignal(void) {
    irrecv.resume();

    decode_results results;
    while (irrecv.decode(&results) == 0) {
        delay(100);
    }

    irrecv.resume();

    if (results.decode_type != NEC)
        return 0xFFFFFFFF;

    return results.value;
}

/*
 *  Retrieves the code of the button for the given controller.
 *  If the controller is the custom then the code is retrieved
 *  from the EEPROM.
 */
int retrieveCode(int controller, int button) {
    if (controller != CUSTOM_INDEX) {
        return Controllers[controller][button];
    }
    else {
        int code = 0;
        code |= (int(EEPROM.read(button * 4 + 0)) << 24);
        code |= (int(EEPROM.read(button * 4 + 1)) << 16);
        code |= (int(EEPROM.read(button * 4 + 2)) << 8);
        code |= (int(EEPROM.read(button * 4 + 3)) << 0);
        return code;
    }
}

/*
 * Stores the code of the given button to EEPROM.
 */
void storeCode(int button, int code) {
    irrecv.enableIRIn(false);

    EEPROM.write(button * 4 + 0, (byte)(code >> 24));
    EEPROM.write(button * 4 + 1, (byte)(code >> 16));
    EEPROM.write(button * 4 + 2, (byte)(code >> 8));
    EEPROM.write(button * 4 + 3, (byte)(code >> 0));
    EEPROM.commit();

    irrecv.enableIRIn(true);
}

void handleCommand() {
    // check arguments
    if (server.args() == 3 && server.argName(0) == "controller"
                           && server.argName(1) == "button"
                           && server.argName(2) == "store") {
        // parse arguments
        int controller = server.arg(0).toInt();
        int     button = server.arg(1).toInt();
        bool     store = server.arg(2) == "true";

        // check indices
        if (controller < 0 || controller > CUSTOM_INDEX || button < 0 || button > 17) {
            // bad request
            server.send(400, "text/plain", "Index out of range!");
            return;
        }

        // DEBUG
        Serial.print("Controller: ");
        Serial.println(controller);
        Serial.print("Button: ");
        Serial.print(button);
        Serial.print(", HEX value: 0x");
        Serial.println(retrieveCode(controller, button), HEX);
        Serial.println();

        if (store) {
            // check controller
            if (controller != CUSTOM_INDEX) {
                // bad request
                server.send(400, "text/plain", "Only 'Custom' controller can be overwritten!");
                return;
            }

            // store a signal
            int code = storeSignal();
            if (code == 0xFFFFFFFF) {
                server.send(400, "text/plain", "Failed to read the code, Try again.");
                return;
            }

            storeCode(button, code);
            Serial.print("Value stored: 0x");
            Serial.println(retrieveCode(CUSTOM_INDEX, button), HEX);
            // send response -> Stored
            server.send(202);
        }
        else {
            // generate a corresponding signal
            generateSignal(retrieveCode(controller, button));
            // send response -> OK
            server.send(200);
        }
    }
    else {
        // bad request
        server.send(400, "text/plain", "Invalid arguments!");
    }
}

void setup() {
    // init serial port
    Serial.begin(115200);

    // init EEPROM
    EEPROM.begin(72);

    // init WiFi access point
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);

    // init web server
    server.onNotFound(handleCommand);
    server.begin();

    // init IR receiver
    irrecv.enableIRIn(true);

    // DEBUG
    Serial.println();
    Serial.print("AccessPoint: '");
    Serial.print(ssid);
    Serial.print("'\nLocal IP:    ");
    Serial.println(WiFi.softAPIP());
    Serial.println();
}

void loop() {
    server.handleClient();
    delay(1);
}

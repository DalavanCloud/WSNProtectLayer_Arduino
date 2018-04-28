#include <Arduino.h>
#include <EEPROM.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>

#include "addresses.h"
#include "conf_common.h"

#define BUFFER_SIZE 64

uint8_t address    = 0;
uint8_t value      = 0;
String  line;

void setup()
{
    Serial.begin(115200);
    // Serial.setTimeout(200);
}


void saveNodeID(uint8_t node_id)
{
    eeprom_write_byte(0, node_id);
}

void saveNodeKey(uint8_t *key, uint8_t node_id)
{
    eeprom_write_block(key, CONFIG_START_ADDRESS + (node_id * KEY_SIZE), KEY_SIZE);
}

#define saveBSKey(key)saveNodeKey(key, 0)


void loop()
{
    uint8_t buffer[BUFFER_SIZE];
    if(Serial.available() > 0){
        uint8_t len1 = Serial.read();
        while(Serial.available() < 1);
        uint8_t len2 = Serial.read();
        if(len1 != len2){
            buffer[0] = REPLY_ERR_LEN;
            Serial.write(buffer, 1);
            Serial.flush();
            return;
        }

        while(Serial.available() < 1);
        if(Serial.readBytes(reinterpret_cast<char*>(buffer), len1) != len1){
            buffer[0] = REPLY_ERR_MSG_SIZE;
            Serial.write(buffer, 1);
            Serial.flush();
            return;
        }
    
        if(buffer[0] == MSG_ID){
            if(len1 < 2){
                buffer[0] = REPLY_ERR_MSG_SIZE; // TODO maybe different error code
                Serial.write(buffer, 1);
                Serial.flush();
            }
            saveNodeID(buffer[1]);
        } else if(buffer[0] == MSG_BS_KEY){
            if(len1 < KEY_SIZE + 1){
                buffer[0] = REPLY_ERR_MSG_SIZE; // TODO maybe different error code
                Serial.write(buffer, 1);
                Serial.flush();
            }
            saveBSKey(buffer + 1);
        } else if(buffer[0] == MSG_NODE_KEY){
            if(len1 < KEY_SIZE + 2){
                buffer[0] = REPLY_ERR_MSG_SIZE; // TODO maybe different error code
                Serial.write(buffer, 1);
                Serial.flush();
            }
            saveNodeKey(buffer + 2, buffer[1]);
        }

        buffer[0] = REPLY_OK;
        Serial.write(buffer, 1);
        Serial.flush();

        // TODO finalizing message => sleep

        // // write only if it changes (limit writes)
        // if(EEPROM.read(address) != value){
        //     EEPROM.write(address, value);
        // }

        // // check stored value
        // if(EEPROM.read(address) == value){
        //     // Serial.write("OK");
        //     Serial.flush();
        // } else {
        //     Serial.write("Failed to write to EEPROM");
        //     Serial.flush();
        //     set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        //     sleep_enable();
        //     sleep_mode();
        // }

    }
}
#ifdef CAN_EMULATOR

#include "usbutil.h"
#include "canread.h"
#include "serialutil.h"
#include "log.h"
#include <stdlib.h>

#define NUMERICAL_SIGNAL_COUNT 11
#define BOOLEAN_SIGNAL_COUNT 5
#define STATE_SIGNAL_COUNT 2
#define EVENT_SIGNAL_COUNT 1

extern SerialDevice serialDevice;
extern UsbDevice USB_DEVICE;
extern Listener listener;

const char* NUMERICAL_SIGNALS[NUMERICAL_SIGNAL_COUNT] = {
    "steering_wheel_angle",
    "torque_at_transmission",
    "engine_speed",
    "vehicle_speed",
    "accelerator_pedal_position",
    "odometer",
    "fine_odometer_since_restart",
    "latitude",
    "longitude",
    "fuel_level",
    "fuel_consumed_since_restart",
};

const char* BOOLEAN_SIGNALS[BOOLEAN_SIGNAL_COUNT] = {
    "parking_brake_status",
    "brake_pedal_status",
    "headlamp_status",
    "high_beam_status",
    "windshield_wiper_status",
};

const char* STATE_SIGNALS[STATE_SIGNAL_COUNT] = {
    "transmission_gear_position",
    "ignition_status",
};

const char* SIGNAL_STATES[STATE_SIGNAL_COUNT][3] = {
    { "neutral", "first", "second" },
    { "off", "run", "accessory" },
};

const char* EVENT_SIGNALS[EVENT_SIGNAL_COUNT] = {
    "door_status",
};

struct Event {
    const char* value;
    bool event;
};

Event EVENT_SIGNAL_STATES[EVENT_SIGNAL_COUNT][3] = {
    { {"driver", false}, {"passenger", true}, {"rear_right", true}},
};

void setup() {
    srand(42);

    initializeLogging();
    initializeSerial(&serialDevice);
    initializeUsb(&USB_DEVICE);
}

bool usbWriteStub(uint8_t* buffer) {
    debug("Ignoring write request -- running an emulator\r\n");
    return true;
}

void loop() {
    while(1) {
        sendNumericalMessage(
                NUMERICAL_SIGNALS[rand() % NUMERICAL_SIGNAL_COUNT],
                rand() % 50 + rand() % 100 * .1, &listener);
        sendBooleanMessage(BOOLEAN_SIGNALS[rand() % BOOLEAN_SIGNAL_COUNT],
                rand() % 2 == 1 ? true : false, &listener);

        int stateSignalIndex = rand() % STATE_SIGNAL_COUNT;
        sendStringMessage(STATE_SIGNALS[stateSignalIndex],
                SIGNAL_STATES[stateSignalIndex][rand() % 3], &listener);

        int eventSignalIndex = rand() % EVENT_SIGNAL_COUNT;
        Event randomEvent = EVENT_SIGNAL_STATES[eventSignalIndex][rand() % 3];
        sendEventedBooleanMessage(EVENT_SIGNALS[eventSignalIndex],
                randomEvent.value, randomEvent.event, &listener);

        processListenerQueues(&listener);
        readFromHost(&USB_DEVICE, usbWriteStub);
        readFromSerial(&serialDevice, usbWriteStub);
    }
}

void reset() { }

const char* getMessageSet() {
    return "emulator";
}

#endif // CAN_EMULATOR

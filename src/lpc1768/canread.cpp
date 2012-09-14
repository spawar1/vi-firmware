#include "canread.h"
#include "canutil_lpc1768.h"
#include "signals.h"

CanMessage receiveCanMessage(CanBus* bus) {
    CAN_MSG_Type message;
    CAN_ReceiveMsg(CAN_CONTROLLER(bus), &message);

    CanMessage result = {message.id, 0};
    result.data = message.dataA[0];
    result.data |= (((uint64_t)message.dataA[1]) << 8);
    result.data |= (((uint64_t)message.dataA[2]) << 16);
    result.data |= (((uint64_t)message.dataA[3]) << 24);
    result.data |= (((uint64_t)message.dataB[0]) << 32);
    result.data |= (((uint64_t)message.dataB[1]) << 40);
    result.data |= (((uint64_t)message.dataB[2]) << 48);
    result.data |= (((uint64_t)message.dataB[3]) << 56);

    return result;
}


#ifndef CAN_EMULATOR

extern "C" {

void CAN_IRQHandler() {
    for(int i = 0; i < getCanBusCount(); i++) {
        CanBus* bus = &getCanBuses()[i];
        if((CAN_IntGetStatus(CAN_CONTROLLER(bus)) & 0x01) == 1) {
            CanMessage message = receiveCanMessage(bus);
            QUEUE_PUSH(CanMessage, &bus->receiveQueue, message);
        }
    }
}

}

#endif // CAN_EMULATOR


#include "signal.h"

int gCurrentSignal[MAX_SIGNAL_SLOTS];
struct SignalReciever* gSignalRecievers[MAX_SIGNAL_SLOTS];

void signalSetSender(struct SignalSender* sender, int value) {
    if (sender->currentSignal != value) {
        sender->currentSignal = value;

        gCurrentSignal[sender->signalIndex] = !gCurrentSignal[sender->signalIndex];

        struct SignalReciever* curr = gSignalRecievers[sender->signalIndex];

        while (curr) {
            curr->callback(curr->data, sender->signalIndex, gCurrentSignal[sender->signalIndex]);
            curr = curr->next;
        }
    }
}

void signalListen(struct SignalReciever* reciever, int signalIndex, void* data, SignalRecieveCallback callback) {
    reciever->next = gSignalRecievers[signalIndex];

    if (gSignalRecievers[signalIndex]) {
        gSignalRecievers[signalIndex]->prev = reciever;
    }

    gSignalRecievers[signalIndex] = reciever;
    reciever->prev = 0;

    reciever->signalIndex = signalIndex;
    reciever->data = data;
    reciever->callback = callback;
}

void signalUnlisten(struct SignalReciever* reciever) {
    if (gSignalRecievers[reciever->signalIndex] == reciever) {
        gSignalRecievers[reciever->signalIndex] = reciever->next;

        if (reciever->next) {
            reciever->next->prev = 0;
        }
    } else {
        if (reciever->next) {
            reciever->next->prev = reciever->prev;
        }
        reciever->prev->next = reciever->next;
    }

    reciever->next = 0;
    reciever->prev = 0;
}

void signalResetAll() {
    int i;

    for (i = 0; i < MAX_SIGNAL_SLOTS; ++i) {
        gCurrentSignal[0] = 0;
        gSignalRecievers[0] = 0;
    }
}
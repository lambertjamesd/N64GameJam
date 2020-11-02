
#ifndef _SIGNAL_SIGNAL_H
#define _SIGNAL_SIGNAL_H

#define MAX_SIGNAL_SLOTS 5

struct SignalSender {
    short signalIndex;
    short currentSignal;
};

typedef void (*SignalRecieveCallback)(void* data, int signalIndex, int currentSignal);

struct SignalReciever {
    struct SignalReciever* next;
    struct SignalReciever* prev;
    int signalIndex;
    void* data;
    SignalRecieveCallback callback;
};

extern int gCurrentSignal[MAX_SIGNAL_SLOTS];
extern struct SignalReciever* gSignalRecievers[MAX_SIGNAL_SLOTS];

void signalSetSender(struct SignalSender* sender, int value);
void signalListen(struct SignalReciever* reciever, int signalIndex, void* data, SignalRecieveCallback callback);
void signalUnlisten(struct SignalReciever* reciever);
void signalResetAll();

#endif
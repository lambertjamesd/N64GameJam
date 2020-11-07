
#ifndef _PUZZLE_SIGNAL_H
#define _PUZZLE_SIGNAL_H

#define MAX_SIGNAL_SLOTS 5

struct SignalSender {
    short signalIndex;
    short currentSignal;
};

typedef void (*SignalRecieveCallback)(void* data, int signalIndex, int currentSignal);

struct SignalReciever {
    struct SignalReciever* next;
    struct SignalReciever* prev;
    void* data;
    SignalRecieveCallback callback;
    int signalIndex;
};

extern int gCurrentSignal[MAX_SIGNAL_SLOTS];
extern struct SignalReciever* gSignalRecievers[MAX_SIGNAL_SLOTS];

void signalSetSender(struct SignalSender* sender, int value);
void signalListen(struct SignalReciever* reciever, int signalIndex, void* data, SignalRecieveCallback callback);
void signalUnlisten(struct SignalReciever* reciever);
void signalResetAll();

#endif
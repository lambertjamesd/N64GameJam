
#include <ultralog.h>
#include "audio.h"
#include "src/boot.h"

extern OSSched         gScheduler;

#define MAX_SEQUENCE_COUNT      2

u8* gAudioHeapBuffer;

ALSeqPlayer	   *gSequencePlayer;
static u8          *gSequenceData[MAX_SEQUENCE_COUNT];
static s32         gSequenceLen[MAX_SEQUENCE_COUNT];
static ALSeq       *gSequence[MAX_SEQUENCE_COUNT];
static ALSeqMarker gSequenceStart[MAX_SEQUENCE_COUNT];
static ALSeqMarker gSequenceLoopStart[MAX_SEQUENCE_COUNT];
static ALSeqMarker gSequenceEnd[MAX_SEQUENCE_COUNT];
static int gNextSeq = 0;

ALHeap             gAudioHeap;
ALSndPlayer gSoundPlayer;

#define MAX_PENDING_SOUNDS  4
#define UNUSED_PENDING_SOUND -1

struct PendingSound {
    ALSndId snd;
    float pitch;
    float volume;
    float pan;
    int priority;
};

struct SeqPlayEvent gPendingSeq;
struct PendingSound gPendingSounds[MAX_PENDING_SOUNDS];

#define MAX_SOUNDS 40

void soundPlayerInit() {
    ALSndpConfig sndConfig;

    sndConfig.maxEvents = MAX_EVENTS;
    sndConfig.maxSounds = MAX_SOUNDS;
    sndConfig.heap = &gAudioHeap;

    alSndpNew(&gSoundPlayer, &sndConfig);
}

void audioStopSequence() {
    if (alSeqpGetState(gSequencePlayer) != AL_STOPPED) {
        alSeqpStop(gSequencePlayer);
    }
}

void audioPlaySequence(struct SeqPlayEvent* playEvent) {
    if (alSeqpGetState(gSequencePlayer) != AL_STOPPED) {
        alSeqpStop(gSequencePlayer);

        gPendingSeq = *playEvent;
    } else {
        gSequenceLen[gNextSeq] = playEvent->romEnd - playEvent->romStart;
        gSequenceData[gNextSeq] = alHeapAlloc(&gAudioHeap, 1, gSequenceLen[gNextSeq]);
        romCopy(playEvent->romStart, (char *) gSequenceData[gNextSeq], gSequenceLen[gNextSeq]);

        gSequence[gNextSeq] = alHeapAlloc(&gAudioHeap, 1, sizeof(ALSeq));
        alSeqNew(gSequence[gNextSeq], gSequenceData[gNextSeq], gSequenceLen[gNextSeq]);
        alSeqNewMarker(gSequence[gNextSeq], &gSequenceStart[gNextSeq], playEvent->playbackStart);    
        alSeqNewMarker(gSequence[gNextSeq], &gSequenceLoopStart[gNextSeq], playEvent->loopStart);
        alSeqNewMarker(gSequence[gNextSeq], &gSequenceEnd[gNextSeq], playEvent->loopEnd);
        alSeqpLoop(gSequencePlayer, &gSequenceLoopStart[gNextSeq], &gSequenceEnd[gNextSeq], playEvent->loopCount);

        if (playEvent->playbackStart) {
            alSeqSetLoc(gSequence[gNextSeq], &gSequenceStart[gNextSeq]);
        }

        alSeqpSetSeq(gSequencePlayer, gSequence[gNextSeq]);
        alSeqpPlay(gSequencePlayer);

        gNextSeq = (gNextSeq + 1) % MAX_SEQUENCE_COUNT;
    }
}

void audioInit() 
{
    ALBankFile    *bankPtr;
    u32           bankLen;
    ALSynConfig   c;
    ALSeqpConfig  seqc;
    amConfig      amc;
    
    alHeapInit(&gAudioHeap, gAudioHeapBuffer, AUDIO_HEAP_SIZE);    

    bankLen = _bankSegmentRomEnd - _bankSegmentRomStart;
    bankPtr = alHeapAlloc(&gAudioHeap, 1, bankLen);
    romCopy(_bankSegmentRomStart, (char *)bankPtr, bankLen);
    
    alBnkfNew(bankPtr, (u8 *) _tableSegmentRomStart);

    c.maxVVoices = MAX_VOICES;
    c.maxPVoices = MAX_VOICES;
    c.maxUpdates = MAX_UPDATES;
    c.dmaproc    = 0;
    c.fxType	 = AL_FX_SMALLROOM;
    c.outputRate = 0;
    c.heap       = &gAudioHeap;
    
    amc.outputRate = 44100;
    amc.framesPerField = NUM_FIELDS;
    amc.maxACMDSize = MAX_RSP_CMDS;
 
    amCreateAudioMgr(&c, AUDIO_PRIORITY, &amc);
    
    seqc.maxVoices      = MAX_VOICES;
    seqc.maxEvents      = MAX_EVENTS;
    seqc.maxChannels    = 16;
    seqc.heap           = &gAudioHeap;
    seqc.initOsc        = 0;
    seqc.updateOsc      = 0;
    seqc.stopOsc        = 0;
#ifdef DEBUG
    seqc.debugFlags     = NO_VOICE_ERR_MASK | NOTE_OFF_ERR_MASK | NO_SOUND_ERR_MASK;
#endif
    gSequencePlayer = alHeapAlloc(&gAudioHeap, 1, sizeof(ALSeqPlayer));
    alSeqpNew(gSequencePlayer, &seqc);
    alSeqpSetBank(gSequencePlayer, bankPtr->bankArray[0]);

    soundPlayerInit();

    int i;
    for (i = 0; i < MAX_PENDING_SOUNDS; ++i) {
        gPendingSounds[i].snd = UNUSED_PENDING_SOUND;
    }
}

int audioPlayState(ALSndId snd) {
    alSndpSetSound(&gSoundPlayer, snd);
    return alSndpGetState(&gSoundPlayer);
}

void audioStopSound(ALSndId snd) {
    alSndpSetSound(&gSoundPlayer, snd);
    alSndpStop(&gSoundPlayer);
}

int audioPendsound(ALSndId snd, float pitch, float volume, float pan, int priority) {
    int i;

    for (i = 0; i < MAX_PENDING_SOUNDS; ++i) {
        if (gPendingSounds[i].snd == UNUSED_PENDING_SOUND) {
            gPendingSounds[i].snd = snd;
            gPendingSounds[i].pitch = pitch;
            gPendingSounds[i].priority = priority;
            return 1;
        }
    }

    for (i = 0; i < MAX_PENDING_SOUNDS; ++i) {
        if (gPendingSounds[i].priority < priority) {
            gPendingSounds[i].snd = snd;
            gPendingSounds[i].pitch = pitch;
            gPendingSounds[i].priority = priority;
            return 1;
        }
    }

    return 0;
}

void audioRestartPlaySound(ALSndId snd, float pitch, float volume, float pan, int priority) {
    alSndpSetSound(&gSoundPlayer, snd);
    if (alSndpGetState(&gSoundPlayer) != AL_STOPPED) {
        if (audioPendsound(snd, pitch, volume, pan, priority)) {
            alSndpStop(&gSoundPlayer);
        }
    } else {
        audioPlaySound(snd, pitch, volume, pan, priority);
    }
}

void audioPlaySound(ALSndId snd, float pitch, float volume, float pan, int priority) {
    if (volume > 0.0f && snd != UNUSED_PENDING_SOUND) {
        alSndpSetSound(&gSoundPlayer, snd);
        alSndpSetPitch(&gSoundPlayer, pitch);
        if (volume > 1.0f) {
            alSndpSetVol(&gSoundPlayer, 32767);
        } else {
            alSndpSetVol(&gSoundPlayer, (s16)(volume * 32767));
        }
        if (pan < -1.0f) {
            alSndpSetPan(&gSoundPlayer, 0);
        } else if (pan > 1.0f) {
            alSndpSetPan(&gSoundPlayer, 127);
        } else {
            alSndpSetPan(&gSoundPlayer, (u8)((pan + 1.0f) * 127));
        }
        alSndpSetPriority(&gSoundPlayer, snd, 10);

        if (alSndpGetState(&gSoundPlayer) == AL_STOPPED) {
            alSndpPlay(&gSoundPlayer);
        }
    }
}

void audioUpdate() {
    int i;

    for (i = 0; i < MAX_PENDING_SOUNDS; ++i) {
        if (gPendingSounds[i].snd != UNUSED_PENDING_SOUND) {
            if (audioPlayState(gPendingSounds[i].snd) == AL_STOPPED) {
                audioPlaySound(
                    gPendingSounds[i].snd, 
                    gPendingSounds[i].pitch, 
                    gPendingSounds[i].priority, 
                    gPendingSounds[i].pan, 
                    gPendingSounds[i].priority
                );

                gPendingSounds[i].snd = UNUSED_PENDING_SOUND;
            }
        }
    }

    if (gPendingSeq.romStart && alSeqpGetState(gSequencePlayer) == AL_STOPPED) {
        audioPlaySequence(&gPendingSeq);
        gPendingSeq.romStart = 0;
    }
}

#include <ultralog.h>
#include "audio.h"
#include "src/boot.h"

extern OSSched         gScheduler;

u8* gAudioHeapBuffer;

ALSeqPlayer	   *seqp;
static u8          *seqPtr;
static s32         seqLen;
static ALSeq       *seq;
static ALSeqMarker seqStart;
static ALSeqMarker seqEnd;

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

struct PendingSound gPendingSounds[MAX_PENDING_SOUNDS];

#define MAX_SOUNDS 40

void soundPlayerInit() {
    ALSndpConfig sndConfig;

    sndConfig.maxEvents = MAX_EVENTS;
    sndConfig.maxSounds = MAX_SOUNDS;
    sndConfig.heap = &gAudioHeap;

    alSndpNew(&gSoundPlayer, &sndConfig);
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

    seqLen = _seqSegmentRomEnd - _seqSegmentRomStart;
    seqPtr = alHeapAlloc(&gAudioHeap, 1, seqLen);
    romCopy(_seqSegmentRomStart, (char *) seqPtr, seqLen);

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
    seqc.debugFlags     = NO_VOICE_ERR_MASK |NOTE_OFF_ERR_MASK | NO_SOUND_ERR_MASK;
#endif
    seqp = alHeapAlloc(&gAudioHeap, 1, sizeof(ALSeqPlayer));
    alSeqpNew(seqp, &seqc);

    seq = alHeapAlloc(&gAudioHeap, 1, sizeof(ALSeq));
    alSeqNew(seq, seqPtr, seqLen);    
    alSeqNewMarker(seq, &seqStart, 0);
    alSeqNewMarker(seq, &seqEnd, -1);

    alSeqpLoop(seqp, &seqStart, &seqEnd, -1);
    alSeqpSetSeq(seqp, seq);
    alSeqpSetBank(seqp, bankPtr->bankArray[0]);
    alSeqpPlay(seqp);

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
}
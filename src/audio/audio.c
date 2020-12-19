
#include <ultralog.h>
#include "audio.h"
#include "src/boot.h"

extern OSSched         gScheduler;

#define MAX_SEQUENCE_COUNT      2

#define MAX_SEQ_LENGTH         (40 * 1024)

struct PendingSound {
    ALSndId snd;
    float pitch;
    float volume;
    float pan;
    int priority;
};


struct Sound3DState {
    ALSndId id;
    float distance;
    int flags;
    struct PendingSound params;
};

u8* gAudioHeapBuffer;

ALSeqPlayer	   *gSequencePlayer;
static u8          *gSequenceData[MAX_SEQUENCE_COUNT];
static s32         gSequenceLen[MAX_SEQUENCE_COUNT];
static ALSeq       *gSequence[MAX_SEQUENCE_COUNT];
static ALSeqMarker gSequenceStart[MAX_SEQUENCE_COUNT];
static ALSeqMarker gSequenceLoopStart[MAX_SEQUENCE_COUNT];
static ALSeqMarker gSequenceEnd[MAX_SEQUENCE_COUNT];
static int gNextSeq = 0;
static char* gCurrentSeq = 0;
struct BasicTransform* gListener;
struct Vector3 gListenerRight = {1.0f, 0.0f, 0.0f};
struct Sound3DState gSound3DStates[MAX_3D_SOUNDS];
static float gMusicVolume = 1.0f;
static float gSoundVolume = 1.0f;
static float gMusicFadeTime = 0.0f;
static float gMusicFadeDuration = 0.0f;

#define GLOBAL_VOLUME_SCALE      1.4f

ALHeap      gAudioHeap;
ALSndPlayer gSoundPlayer;

#define MAX_PENDING_SOUNDS  8
#define UNUSED_PENDING_SOUND -1
#define UNUSED_DISTANCE 1000000000.0f

struct SeqPlayEvent gPendingSeq;
struct PendingSound gPendingSounds[MAX_PENDING_SOUNDS];
struct PendingSound gPendingUpdate[MAX_PENDING_SOUNDS];

#define MAX_SOUNDS 40

void soundPlayerInit() {
    ALSndpConfig sndConfig;

    sndConfig.maxEvents = MAX_EVENTS;
    sndConfig.maxSounds = MAX_SOUNDS;
    sndConfig.heap = &gAudioHeap;

    int i;

    for (i = 0; i < MAX_3D_SOUNDS; ++i) {
        gSound3DStates[i].id = UNUSED_PENDING_SOUND;
        gSound3DStates[i].distance = UNUSED_DISTANCE;
    }

    alSndpNew(&gSoundPlayer, &sndConfig);
}

void audioStopSequence(float fadeTime) {
    if (fadeTime == 0.0f) {
        if (alSeqpGetState(gSequencePlayer) != AL_STOPPED) {
            alSeqpStop(gSequencePlayer);
            gCurrentSeq = 0;
            gMusicFadeDuration = 0.0f;
            gMusicFadeTime = 0.0f;
        }
    } else {
        gMusicFadeDuration = fadeTime;
    }
}

void audioSetSeqVolume(float value) {
    gMusicVolume = value;
    alSeqpSetVol(gSequencePlayer, (s16)(0x7fff * gMusicVolume));
}

void audioPlaySequence(struct SeqPlayEvent* playEvent) {
    if (gCurrentSeq == playEvent->romStart) {
        return;
    } else if (alSeqpGetState(gSequencePlayer) == AL_PLAYING) {
        alSeqpStop(gSequencePlayer);

        gPendingSeq = *playEvent;
    } else {
        gMusicFadeDuration = 0.0f;
        gMusicFadeTime = 0.0f;
        gSequenceLen[gNextSeq] = playEvent->romEnd - playEvent->romStart;
        romCopy(playEvent->romStart, (char *) gSequenceData[gNextSeq], gSequenceLen[gNextSeq]);

        alSeqNew(gSequence[gNextSeq], gSequenceData[gNextSeq], gSequenceLen[gNextSeq]);
        alSeqNewMarker(gSequence[gNextSeq], &gSequenceStart[gNextSeq], playEvent->playbackStart);    
        alSeqNewMarker(gSequence[gNextSeq], &gSequenceLoopStart[gNextSeq], playEvent->loopStart);
        alSeqNewMarker(gSequence[gNextSeq], &gSequenceEnd[gNextSeq], playEvent->loopEnd);
        alSeqpLoop(gSequencePlayer, &gSequenceLoopStart[gNextSeq], &gSequenceEnd[gNextSeq], playEvent->loopCount);
        alSeqpSetVol(gSequencePlayer, (s16)(0x7fff * gMusicVolume));

        if (playEvent->playbackStart) {
            alSeqSetLoc(gSequence[gNextSeq], &gSequenceStart[gNextSeq]);
        }

        alSeqpSetSeq(gSequencePlayer, gSequence[gNextSeq]);
        alSeqpPlay(gSequencePlayer);
        gCurrentSeq = playEvent->romStart;

        gNextSeq = (gNextSeq + 1) % MAX_SEQUENCE_COUNT;
    }
}

void audioInit() 
{
    ALBankFile    *bankPtr;
    u32           bankLen;
    ALSynConfig   synConfig;
    ALSeqpConfig  seqc;
    amConfig      amc;
    
    alHeapInit(&gAudioHeap, gAudioHeapBuffer, AUDIO_HEAP_SIZE);    

    bankLen = _bankSegmentRomEnd - _bankSegmentRomStart;
    bankPtr = alHeapAlloc(&gAudioHeap, 1, bankLen);
    romCopy(_bankSegmentRomStart, (char *)bankPtr, bankLen);
    
    alBnkfNew(bankPtr, (u8 *) _tableSegmentRomStart);

    synConfig.maxVVoices = MAX_VOICES;
    synConfig.maxPVoices = MAX_VOICES;
    synConfig.maxUpdates = MAX_UPDATES;
    synConfig.dmaproc    = 0;
    synConfig.fxType	 = AL_FX_SMALLROOM;
    synConfig.outputRate = 0;
    synConfig.heap       = &gAudioHeap;
    
    amc.outputRate = 44100;
    amc.framesPerField = NUM_FIELDS;
    amc.maxACMDSize = MAX_RSP_CMDS;
 
    amCreateAudioMgr(&synConfig, AUDIO_PRIORITY, &amc);
    
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
    alSeqpSetVol(gSequencePlayer, 0x7fff);

    soundPlayerInit();

    int i;

    for (i = 0; i < MAX_SEQUENCE_COUNT; ++i) {
        gSequenceData[i] = alHeapAlloc(&gAudioHeap, 1, MAX_SEQ_LENGTH);
        gSequence[i] = alHeapAlloc(&gAudioHeap, 1, sizeof(ALSeq));
    }


    for (i = 0; i < MAX_PENDING_SOUNDS; ++i) {
        gPendingSounds[i].snd = UNUSED_PENDING_SOUND;
    }
}

void audioSetSoundVolume(float value) {
    gSoundVolume = value;
}

int audioPlayState(ALSndId snd) {
    alSndpSetSound(&gSoundPlayer, snd);
    return alSndpGetState(&gSoundPlayer);
}

void audioStopSound(ALSndId snd) {
    alSndpSetSound(&gSoundPlayer, snd);
    alSndpStop(&gSoundPlayer);
}

int audioPendsound(struct PendingSound* into, ALSndId snd, float pitch, float volume, float pan, int priority) {
    int i;

    for (i = 0; i < MAX_PENDING_SOUNDS; ++i) {
        if (into[i].snd == UNUSED_PENDING_SOUND || into[i].snd == snd) {
            into[i].snd = snd;
            into[i].volume = volume;
            into[i].pitch = pitch;
            into[i].priority = priority;
            return 1;
        }
    }

    for (i = 0; i < MAX_PENDING_SOUNDS; ++i) {
        if (into[i].priority < priority) {
            into[i].snd = snd;
            into[i].volume = volume;
            into[i].pitch = pitch;
            into[i].priority = priority;
            return 1;
        }
    }

    return 0;
}

void audioRestartPlaySound(ALSndId snd, float pitch, float volume, float pan, int priority) {
    alSndpSetSound(&gSoundPlayer, snd);
    if (alSndpGetState(&gSoundPlayer) != AL_STOPPED) {
        if (audioPendsound(gPendingSounds, snd, pitch, volume, pan, priority)) {
            alSndpStop(&gSoundPlayer);
        }
    } else {
        audioPlaySound(snd, pitch, volume, pan, priority);
    }
}

void audioUpdateParameters(ALSndId snd, float pitch, float volume, float pan, int priority) {
    if (volume > 1.0f) {
        volume = 1.0f;
    }

    float floatAsShort = gSoundVolume * volume * (32767 * GLOBAL_VOLUME_SCALE);
    
    if (floatAsShort > 32767.0f) {
        alSndpSetVol(&gSoundPlayer, 32767);
    } else {
        alSndpSetVol(&gSoundPlayer, (s16)floatAsShort);
    }

    alSndpSetSound(&gSoundPlayer, snd);
    alSndpSetPitch(&gSoundPlayer, pitch);

    float panAs127 = (pan + 1.0f) * 64.0f;

    if (panAs127 <= 0.0f) {
        alSndpSetPan(&gSoundPlayer, 0);
    } else if (panAs127 >= 127.0f) {
        alSndpSetPan(&gSoundPlayer, 127);
    } else {
        alSndpSetPan(&gSoundPlayer, (u8)(panAs127));
    }
    alSndpSetPriority(&gSoundPlayer, snd, 10);
}

void audioPlaySound(ALSndId snd, float pitch, float volume, float pan, int priority) {
    if (volume > 0.0f && snd != UNUSED_PENDING_SOUND) {
        alSndpSetSound(&gSoundPlayer, snd);
        if (alSndpGetState(&gSoundPlayer) == AL_STOPPED) {
            audioUpdateParameters(snd, pitch, volume, pan, priority);
            alSndpPlay(&gSoundPlayer);
        } else {
            audioPendsound(gPendingUpdate, snd, pitch, volume, pan, priority);
        }
    }
}

#define FULL_VOLUME_RADIUS   20.0f
#define SOUND_VELOCITY      40.0f

struct Sound3DState* audioFindState(ALSndId snd, int flags) {
    int index;
    int emptySlot = -1;
    for (index = 0; index < MAX_3D_SOUNDS; ++index) {
        if (gSound3DStates[index].id == snd && gSound3DStates[index].flags == flags) {
            return &gSound3DStates[index];
        } else if (gSound3DStates[index].id == UNUSED_PENDING_SOUND && emptySlot == -1) {
            emptySlot = index;
        }
    }

    if (emptySlot == -1) {
        return 0;
    } else {
        gSound3DStates[emptySlot].id = snd;
        gSound3DStates[emptySlot].flags = flags;
        return &gSound3DStates[emptySlot];
    }
}

void audioPlaySound3D(ALSndId snd, float pitch, float volume, struct Vector3* source, struct Vector3* velocity, int flags, int priority) {
    if (!gListener) {
        audioPlaySound(snd, pitch, volume, 0.0f, priority);
    } else {
        struct Vector3 offset;
        vector3Sub(source, &gListener->position, &offset);
        struct Sound3DState* state = audioFindState(snd, flags);

        float distance = vector3MagSqrd(&offset);
        float pan;

        if (!state || distance >= state->distance) {
            return;
        }

        state->distance = distance;
        
        if (distance > 0.1f) {
            if (distance > FULL_VOLUME_RADIUS * FULL_VOLUME_RADIUS) {
                volume *= (FULL_VOLUME_RADIUS * FULL_VOLUME_RADIUS) / distance;
            }
            float invDistance = 1.0f / sqrtf(distance);
            pitch *= SOUND_VELOCITY / (SOUND_VELOCITY + vector3Dot(&offset, velocity) * invDistance);
            pan = vector3Dot(&gListenerRight, &offset) * invDistance;
        } else {
            volume *= 100.0f;
            pan = 0.0f;
        }

        state->flags = flags;
        state->params.snd = snd;
        state->params.pitch = pitch;
        state->params.volume = volume;
        state->params.pan = pan;
        state->params.priority = priority;
    }
}

void audioUpdate() {
    int i;

    if (gListener) {
        quatMultVector(&gListener->rotation, &gRight, &gListenerRight);
    }

    if (gMusicFadeDuration > 0.0f) {
        gMusicFadeTime += gTimeDelta;

        if (gMusicFadeTime > gMusicFadeDuration) {
            audioStopSequence(0.0f);
            gMusicFadeDuration = 0.0f;
            gMusicFadeTime = 0.0f;
        } else {
            alSeqpSetVol(gSequencePlayer, (s16)(0x7fff * gMusicVolume * (1.0f - gMusicFadeTime / gMusicFadeDuration)));
        }
    }

    for (i = 0; i < MAX_3D_SOUNDS; ++i) {
        if (gSound3DStates[i].id != UNUSED_PENDING_SOUND) {
            if (gSound3DStates[i].distance == UNUSED_DISTANCE) {
                if (gSound3DStates[i].flags & AUDIO_3D_FLAGS_LOOPED) {
                    audioStopSound(gSound3DStates[i].id);
                }
                gSound3DStates[i].id = UNUSED_PENDING_SOUND;
            } else {
                if (gSound3DStates[i].flags & AUDIO_3D_FLAGS_RESTART) {
                    audioRestartPlaySound(gSound3DStates[i].params.snd, gSound3DStates[i].params.pitch, gSound3DStates[i].params.volume, gSound3DStates[i].params.pan, gSound3DStates[i].params.priority);
                } else if (audioPlayState(gPendingUpdate[i].snd) == AL_PLAYING) {
                    audioUpdateParameters(gSound3DStates[i].params.snd, gSound3DStates[i].params.pitch, gSound3DStates[i].params.volume, gSound3DStates[i].params.pan, gSound3DStates[i].params.priority);
                } else {
                    audioPlaySound(gSound3DStates[i].params.snd, gSound3DStates[i].params.pitch, gSound3DStates[i].params.volume, gSound3DStates[i].params.pan, gSound3DStates[i].params.priority);
                }
            }

            gSound3DStates[i].distance = UNUSED_DISTANCE;
        }
    }

    for (i = 0; i < MAX_PENDING_SOUNDS; ++i) {
        if (gPendingSounds[i].snd != UNUSED_PENDING_SOUND) {
            if (audioPlayState(gPendingSounds[i].snd) == AL_STOPPED) {
                audioPlaySound(
                    gPendingSounds[i].snd, 
                    gPendingSounds[i].pitch, 
                    gPendingSounds[i].volume, 
                    gPendingSounds[i].pan, 
                    gPendingSounds[i].priority
                );

                gPendingSounds[i].snd = UNUSED_PENDING_SOUND;
            }
        }

        if (gPendingUpdate[i].snd != UNUSED_PENDING_SOUND) {
            if (audioPlayState(gPendingUpdate[i].snd) == AL_PLAYING) {
                audioUpdateParameters(
                    gPendingUpdate[i].snd, 
                    gPendingUpdate[i].pitch, 
                    gPendingUpdate[i].volume, 
                    gPendingUpdate[i].pan, 
                    gPendingUpdate[i].priority
                );
            }
            gPendingUpdate[i].snd = UNUSED_PENDING_SOUND;
        }
    }

    if (gPendingSeq.romStart && alSeqpGetState(gSequencePlayer) != AL_PLAYING) {
        audioPlaySequence(&gPendingSeq);
        gPendingSeq.romStart = 0;
    }
}
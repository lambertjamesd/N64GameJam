
#include <ultralog.h>
#include "audio.h"
#include "src/boot.h"

extern OSSched         gScheduler;


u8 audioHeap[AUDIO_HEAP_SIZE];

ALSeqPlayer	   *seqp;
static u8          *seqPtr;
static s32         seqLen;
static ALSeq       *seq;
static ALSeqMarker seqStart;
static ALSeqMarker seqEnd;

ALHeap             hp;

void initAudio(void) 
{
    ALBankFile    *bankPtr;
    u32           bankLen;
    ALSynConfig   c;
    ALSeqpConfig  seqc;
    amConfig      amc;
    
    alHeapInit(&hp, audioHeap, sizeof(audioHeap));    

    bankLen = _bankSegmentRomEnd - _bankSegmentRomStart;
    bankPtr = alHeapAlloc(&hp, 1, bankLen);
    romCopy(_bankSegmentRomStart, (char *)bankPtr, bankLen);
    
    alBnkfNew(bankPtr, (u8 *) _tableSegmentRomStart);

    seqLen = _seqSegmentRomEnd - _seqSegmentRomStart;
    seqPtr = alHeapAlloc(&hp, 1, seqLen);
    romCopy(_seqSegmentRomStart, (char *) seqPtr, seqLen);

    c.maxVVoices = MAX_VOICES;
    c.maxPVoices = MAX_VOICES;
    c.maxUpdates = MAX_UPDATES;
    c.dmaproc    = 0;
    c.fxType	 = AL_FX_SMALLROOM;
    c.outputRate = 0;
    c.heap       = &hp;
    
    amc.outputRate = 44100;
    amc.framesPerField = NUM_FIELDS;
    amc.maxACMDSize = MAX_RSP_CMDS;
 
    amCreateAudioMgr(&c, AUDIO_PRIORITY, &amc);
    
    seqc.maxVoices      = MAX_VOICES;
    seqc.maxEvents      = MAX_EVENTS;
    seqc.maxChannels    = 16;
    seqc.heap           = &hp;
    seqc.initOsc        = 0;
    seqc.updateOsc      = 0;
    seqc.stopOsc        = 0;
#ifdef DEBUG
    seqc.debugFlags     = NO_VOICE_ERR_MASK |NOTE_OFF_ERR_MASK | NO_SOUND_ERR_MASK;
#endif
    seqp = alHeapAlloc(&hp, 1, sizeof(ALSeqPlayer));
    alSeqpNew(seqp, &seqc);

    seq = alHeapAlloc(&hp, 1, sizeof(ALSeq));
    alSeqNew(seq, seqPtr, seqLen);    
    alSeqNewMarker(seq, &seqStart, 0);
    alSeqNewMarker(seq, &seqEnd, -1);

    alSeqpLoop(seqp, &seqStart, &seqEnd, -1);
    alSeqpSetSeq(seqp, seq);
    alSeqpSetBank(seqp, bankPtr->bankArray[0]);
    alSeqpPlay(seqp);
}


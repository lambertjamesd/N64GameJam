#ifndef __audio__
#define __audio__

#include <ultra64.h>
#include <libaudio.h>
#include <sched.h>
#include "src/math/basictransform.h"

#define MAX_UPDATES             32
#define MAX_EVENTS              64
#define AUDIO_HEAP_SIZE         400000

#define MAX_VOICES              26
#define EXTRA_SAMPLES           80
#define NUM_OUTPUT_BUFFERS      3      /* Need three of these */
#define OUTPUT_RATE             44100
#define MAX_AUDIO_MESGS         12
#define QUIT_MSG                10

#define DMA_BUFFER_LENGTH       0x800  /* Larger buffers result in fewer DMA' but more  */
                                       /* memory being used.  */


#define NUM_ACMD_LISTS          2      /* two lists used by this example                */
#define MAX_RSP_CMDS            4096   /* max number of commands in any command list.   */
                                       /* Mainly dependent on sequences used            */

#define NUM_DMA_BUFFERS         24     /* max number of dma buffers needed.             */
                                       /* Mainly dependent on sequences and sfx's       */

#define NUM_DMA_MESSAGES        32     /* The maximum number of DMAs any one frame can  */
                                       /* have.                                         */

#define FRAME_LAG               1      /* The number of frames to keep a dma buffer.    */
                                       /* Increasing this number causes buffers to not  */
                                       /* be deleted as quickly. This results in fewer  */
                                       /* DMA's but you need more buffers.              */


#define AUDIO_STACKSIZE         0x2000

#define MAX_3D_SOUNDS           4

#define AUDIO_3D_FLAGS_RESTART      (1 << 0)
#define AUDIO_3D_FLAGS_LOOPED       (1 << 1)


typedef struct {
    u32       outputRate;
    u32       framesPerField;
    u32       maxACMDSize;
} amConfig;

struct SeqPlayEvent {
    char* romStart;
    char* romEnd;
    int playbackStart;
    int loopStart;
    int loopEnd;
    int loopCount;
};

void    amCreateAudioMgr(ALSynConfig *c, OSPri priority, amConfig *amc, int fps);

extern u64 audYieldBuf[];
extern u8* gAudioHeapBuffer;

extern ALSndPlayer gSoundPlayer;
extern ALHeap gAudioHeap;
extern struct BasicTransform* gListener;

void audioSetSeqVolume(float value);
void audioPlaySequence(struct SeqPlayEvent* playEvent);
void audioStopSequence(float fadeTime);

void audioSetSoundVolume(float value);
int audioPlayState(ALSndId snd);
void audioStopSound(ALSndId snd);
void audioRestartPlaySound(ALSndId snd, float pitch, float volume, float pan, int priority);
void audioPlaySound(ALSndId snd, float pitch, float volume, float pan, int priority);
void audioPlaySound3D(ALSndId snd, float pitch, float volume, struct Vector3* source, struct Vector3* velocity, int flags, int priority);

void audioUpdate();

#endif


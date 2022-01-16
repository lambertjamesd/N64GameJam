OUTPUT_ARCH (mips)

#define BEGIN_SEG(name, addr) \
    _##name##SegmentStart = ADDR(.name); \
    _##name##SegmentRomStart = __romPos; \
    .name addr : AT(__romPos)

#define END_SEG(name) \
    _##name##SegmentEnd = ADDR(.name) + SIZEOF(.name); \
    _##name##SegmentRomEnd = __romPos + SIZEOF(.name); \
    __romPos += SIZEOF(.name);

#define BEGIN_NOLOAD(name) \
    _##name##SegmentBssStart = ADDR(.name.noload); \
    .name.noload (NOLOAD) :

#define END_NOLOAD(name) \
    _##name##SegmentBssEnd = ADDR(.name.noload) + SIZEOF(.name.noload); \
    _##name##SegmentBssSize = SIZEOF(.name.noload);

#define DEFINE_LEVEL(humanName, name, theme, maxPlayers, flags) \
   BEGIN_SEG(name, 0x04000000) \
   { \
      build/data/levels/name/level.o(.data); \
      build/data/levels/name/level.o(.bss); \
   } \
   END_SEG(name) \
   BEGIN_SEG(name##_wireframe, 0x07000000) \
   { \
      build/data/levels/name/level.wire.o(.data); \
      build/data/levels/name/level.wire.o(.bss); \
   } \
   END_SEG(name##_wireframe) \

#define DEFINE_THEME(theme) \
   BEGIN_SEG(theme, 0x05000000) \
   { \
      build/data/level_themes/theme/the##me.o(.data); \
      build/data/level_themes/theme/the##me.o(.bss); \
   } \
   END_SEG(theme) \


#define LEVEL_SEGMENT 0x03000000
#define LEVEL_THEME_SEGMENT 0x04000000

SECTIONS
{
   __romPos = 0;

   BEGIN_SEG(boot, 0x04000000)
   {
      build/asm/rom_header.o(.text);
      build/boot.6102.o(.data);
   }
   END_SEG(boot)

   BEGIN_SEG(code, 0x80000400) SUBALIGN(16)
   {
      build/asm/entry.o(.text);
      CODE_SEGMENT(.text);
      /usr/lib/n64/PR/rspboot.o(.text);
      /usr/lib/n64/PR/gspFast3D.o(.text);
      /usr/lib/n64/PR/gspFast3D.dram.o(.text);
      /usr/lib/n64/PR/aspMain.o(.text);

      /* data */
      CODE_SEGMENT(.data*);
      /usr/lib/n64/PR/rspboot.o(.data*);
      /usr/lib/n64/PR/gspFast3D.o(.data*);
      /usr/lib/n64/PR/gspFast3D.dram.o(.data*);
      /usr/lib/n64/PR/aspMain.o(.data*);

      /* rodata */
      CODE_SEGMENT(.rodata*);
   }
   END_SEG(code)
   BEGIN_NOLOAD(code)
   {
      CODE_SEGMENT(COMMON);
      CODE_SEGMENT(.scommon*);
      CODE_SEGMENT(.bss*);
      . = ALIGN(0x8);
   }
   END_NOLOAD(code)
   
   _codeSegmentBssEnd = .;
   
   gHeapStartPos = .;
   
   BEGIN_SEG(static, 0x01000000)
   {
      build/src/graphics/init.o(.data);
      build/src/graphics/init.o(.bss);
   }
   END_SEG(static)
   
   BEGIN_SEG(alienworld, LEVEL_THEME_SEGMENT)
   {
      build/src/levelthemes/alienworld/materials.o(.data);
      build/src/levelthemes/alienworld/materials.o(.bss);
   }
   END_SEG(alienworld)
   
   BEGIN_SEG(alienworldred, LEVEL_THEME_SEGMENT)
   {
      build/src/levelthemes/alienworldred/materials.o(.data);
      build/src/levelthemes/alienworldred/materials.o(.bss);
   }
   END_SEG(alienworldred)
   
   BEGIN_SEG(alienworldgrey, LEVEL_THEME_SEGMENT)
   {
      build/src/levelthemes/alienworldgrey/materials.o(.data);
      build/src/levelthemes/alienworldgrey/materials.o(.bss);
   }
   END_SEG(alienworldgrey)
   
   BEGIN_SEG(spinning_logo, LEVEL_SEGMENT)
   {
      build/src/menu/geo/spinninglogo.o(.data);
      build/src/menu/geo/spinninglogo.o(.bss);
   }
   END_SEG(spinning_logo)
   
   BEGIN_SEG(titlescreen, LEVEL_SEGMENT)
   {
      build/src/menu/geo/titlescreen.o(.data);
      build/src/menu/geo/titlescreen.o(.bss);
   }
   END_SEG(titlescreen)

#include "build/spec/level_segs"

   BEGIN_SEG(sound_data, __romPos)
   {
      build/asm/sound_data.o(.data);
      build/asm/sound_data.o(.bss);
   }
   END_SEG(sound_data)

   BEGIN_SEG(image_slide_data, __romPos)
   {
      build/asm/image_slide_data.o(.data);
      build/asm/image_slide_data.o(.bss);
   }
   END_SEG(image_slide_data)

   /* Discard everything not specifically mentioned above. */
   /DISCARD/ :
   {
      *(.eh_frame)
      *(.MIPS.abiflags)
   }
}
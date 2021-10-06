/* Copyright 2021 Jaakko Keränen <jaakko.keranen@iki.fi>

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#pragma once

#include <the_Foundation/archive.h>
#include <the_Foundation/ptrarray.h>
#include "stb_truetype.h"

#if defined (LAGRANGE_ENABLE_HARFBUZZ)
#   include <hb.h>
#endif

/* Fontpacks are ZIP archives that contain a configuration file and one of more font
files. The fontpack format is used instead of plain TTF/OTF because the text renderer
uses additional metadata about each font.

All the available fontpacks are loaded and used for looking up glyphs for rendering.
The user may install new fontpacks via the GUI. The user's fontpacks are stored inside
the config directory. There may also be fontpacks available from system-wide locations. */

enum iFontSize {
    uiNormal_FontSize, /* 1.000 -- keep at index 0 for convenience */
    uiMedium_FontSize, /* 1.125 */
    uiBig_FontSize,    /* 1.333 */
    uiLarge_FontSize,  /* 1.666 */
    uiTiny_FontSize,   /* 0.800 */
    uiSmall_FontSize,  /* 0.900 */
    contentRegular_FontSize,
    contentMedium_FontSize,
    contentBig_FontSize,
    contentLarge_FontSize,
    contentHuge_FontSize,
    contentMonoSmall_FontSize,
    contentMono_FontSize,
    contentSmall_FontSize,
    max_FontSize
};

enum iFontStyle {
    regular_FontStyle,
    italic_FontStyle,
    light_FontStyle,
    semiBold_FontStyle,
    bold_FontStyle,
    max_FontStyle,
    /* all permutations: */
    maxVariants_Fonts = max_FontStyle * max_FontSize
};

float   scale_FontSize  (enum iFontSize size);

iDeclareType(FontSpec)
iDeclareTypeConstruction(FontSpec)

enum iFontSpecFlags {
    override_FontSpecFlag  = iBit(1),
    monospace_FontSpecFlag = iBit(2), /* can be used in preformatted content */
    auxiliary_FontSpecFlag = iBit(3), /* only used for looking up glyphs missing from other fonts */
    arabic_FontSpecFlag    = iBit(4),
    fixNunitoKerning_FontSpecFlag = iBit(31), /* manual hardcoded kerning tweaks for Nunito */
};

iDeclareType(FontFile)
iDeclareTypeConstruction(FontFile)
    
struct Impl_FontFile {
    enum iFontStyle style;
    iBlock          sourceData;
    stbtt_fontinfo  stbInfo;
#if defined (LAGRANGE_ENABLE_HARFBUZZ)
    hb_blob_t *hbBlob;
    hb_face_t *hbFace;
    hb_font_t *hbFont;
#endif
    /* Metrics: */
    int ascent, descent, emAdvance;
};

float   scaleForPixelHeight_FontFile    (const iFontFile *, int pixelHeight);

iLocalDef uint32_t findGlyphIndex_FontFile(const iFontFile *d, iChar ch) {
    return stbtt_FindGlyphIndex(&d->stbInfo, ch);
}

uint8_t *   rasterizeGlyph_FontFile(const iFontFile *, float xScale, float yScale, float xShift,
                                    uint32_t glyphIndex, int *w, int *h); /* caller must free() the returned bitmap */
void        measureGlyph_FontFile  (const iFontFile *, uint32_t glyphIndex,
                                    float xScale, float yScale, float xShift,
                                    int *x0, int *y0, int *x1, int *y1);
struct Impl_FontSpec {
    iString id;   /* unique ID */
    iString name; /* human-readable label */
    int     flags;
    int     priority;
    float   scaling;
    float   vertOffset;
    const iFontFile *styles[max_FontStyle];
};
 
void    init_Fonts      (const char *userDir);
void    deinit_Fonts    (void);

const iFontSpec *   findSpec_Fonts              (const char *fontId);
const iPtrArray *   listSpecsByPriority_Fonts   (void);
#ifndef __TEXTWRAP_H__
#define __TEXTWRAP_H__


//
//  Text layout


// TODO: impl. some nice text layout functionality (legacy code below)


/*

inline
void ScaleTextInline(List<Quad> text, f32 scale, s32 x0, s32 y0, s32 w, s32 h) {
    if (scale != 1.0f) {
        for (u32 i = 0; i < text.len; ++i) {
            Quad *q = text.lst + i;

            for (u32 j = 0; j < 6; ++j) {
                Vector2f *pos = &(q->verts + j)->pos;
                pos->x = x0 + (pos->x - x0) * scale;
                pos->y = y0 + (pos->y - y0) * scale;
            }
        }
    }
}
inline
u32 WorldLen(Str s, List<u8> advance_x, s32 *w_adv) {
    u32 i = 0;
    *w_adv = 0;
    while (i < s.len && IsWhiteSpace(s.str[i]) == false) {
        *w_adv += advance_x.lst[s.str[i]];
        ++i;
    }
    return i;
}
inline
bool CanDoNewline(s32 pt_y, s32 ln_height, s32 ln_ascend, s32 ln_limit_y) {
    bool result = false;
    if (pt_y + ln_ascend + 2 * ln_height < ln_limit_y) {
        result = true;
    }
    return result;
}
inline
bool CanDoWhiteSpace(s32 pt_x, s32 w_space, s32 ln_limit_x) {
    bool result = pt_x + w_space <= ln_limit_x;
    return result;
}

inline
void DoNewLine(s32 ln_height, s32 left, s32 *pt_x, s32 *pt_y) {
    *pt_y += ln_height;
    *pt_x = left;
}
inline
void DoWhiteSpace(s32 space_width, s32 *pt_x) {
    *pt_x += space_width;
}

enum TextAlign {
    TAL_LEFT,
    TAL_CENTER,
    TAL_RIGHT,

    TAL_CNT,
};

List<Quad> LayoutTextAutowrap(MArena *a_dest, FontAtlas *plt, Str txt, s32 x0, s32 y0, s32 w, s32 h, Color color, TextAlign ta) {
    assert(g_current_font != NULL && "init text plotters first");


    //
    // TODO: un-retire this autowrap function


    s32 pt_x = x0;
    s32 pt_y = y0;
    s32 box_r = x0 + w;
    s32 box_b = y0 + h;
    s32 w_space = plt->advance_x.lst[' '];

    u32 i = 0;
    Str s = txt;

    List<Quad> quads = InitList<Quad>(a_dest, 0);
    u32 line_first_idx = 0;
    u32 line_len = 0;
    while (s.len > 0) {
        // while words

        if (IsNewLine(s)) {
            if (CanDoNewline(pt_y, plt->ln_height, plt->ln_ascend, box_b)) {
                DoNewLine(plt->ln_height, x0, &pt_x, &pt_y);

                //AlignQuadsH(List<Quad> { quads.lst + line_first_idx, line_len }, x0 + w / 2, ta);
                line_len = 0;
                line_first_idx = quads.len;
            }
            s = StrInc(s, 1);
        }
        if (IsWhiteSpace(s)) {
            if (CanDoWhiteSpace(pt_x, w_space, box_r)) {
                DoWhiteSpace(w_space, &pt_x);
            }
            s = StrInc(s, 1);
        }

        // lookahead word len (include leading whitespace)
        s32 w_adv = 0;
        u32 w_len = WorldLen(s, plt->advance_x, &w_adv);

        // word wrap
        if (pt_x + w_adv > box_r) {
            if (CanDoNewline(pt_y, plt->ln_height, plt->ln_ascend, box_b)) {
                DoNewLine(plt->ln_height, x0, &pt_x, &pt_y);
            
                //AlignQuadsH(List<Quad> { quads.lst + line_first_idx, line_len }, x0 + w / 2, ta);
                line_len = 0;
                line_first_idx = quads.len;
            }
            else {
                // ran out of space, exit
                break;
            }
        }

        // lay out word
        for (u32 j = 0; j < w_len; ++j) {
            char c = s.str[j];

            // TODO: re-impl.
            //Quad q = QuadOffset(plt->cooked.lst + c, pt_x, pt_y, color);
            Quad q = {};

            pt_x += plt->advance_x.lst[c];
            ArenaAlloc(a_dest, sizeof(Quad));
            quads.Add(q);

            line_len++;
        }
        s = StrInc(s, w_len);

        // dbg count
        ++i;
    }
    assert(quads.len <= txt.len); // quad len equals char count minus whitespaces

    // align the last line of the batch
    //AlignQuadsH(List<Quad> { quads.lst + line_first_idx, line_len }, x0 + w / 2, ta);

    // only scale if absolutely necessary
    f32 scale = 1.0f;
    if (scale != 1) {
        ScaleTextInline(quads, scale, x0, y0, w, h);
    }

    // TODO: update this hack to be more organized -> e.g. put assembling the drawcall outside of
    //      this function somehow, maybe in the UI_xxx calls.
    DrawCall dc = {};
    dc.tpe = DCT_TEXTURE_BYTE;
    dc.texture_key = plt->GetKey();
    dc.quads = quads;

    return quads;
}

*/


#endif

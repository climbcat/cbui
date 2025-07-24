#ifndef __IMUI_H__
#define __IMUI_H__


//
//  UI Panel quad layout
//


void PanelPlot( f32 l, f32 t, f32 w, f32 h, f32 thic_border, Color col_border = { RGBA_GRAY_75 }, Color col_pnl = { RGBA_WHITE } )
{
    if (thic_border >= w / 2 || thic_border >= w / 2) {
        return;
    }

    SpriteRender_PushQuad(QuadCookSolid(w, h, l, t, col_border));
    SpriteRender_PushQuad(QuadCookSolid(w - 2*thic_border, h - 2*thic_border, l + thic_border, t + thic_border, col_pnl));
}


//
//  IMUI system experiments
//


//
//  Tree structure is built every turn
//  
//  How the tree structure links:
//      - siblings are iterated by next
//      - sub-branches are created from a node using first
//      - all nodes (except root) have parent set
//


struct CollRect {
    s32 x0;
    s32 x1;
    s32 y0;
    s32 y1;

    inline
    bool DidCollide(s32 x, s32 y) {
        bool bx = (x >= x0 && x <= x1);
        bool by = (y >= y0 && y <= y1);
        return bx && by;
    }
};


enum WidgetAlignmentFLags {
    WA_PASSIVE = 0,

    WA_TOP_LEFT = 1 << 0,
    WA_TOP_RIGHT = 1 << 1,
    WA_BOTTOM_LEFT = 1 << 2,
    WA_BOTTOM_RIGHT = 1 << 3,

    WA_CENTV_LEFT = 1 << 4,
    WA_CENTV_RIGHT = 1 << 5,
    WA_TOP_CENTH = 1 << 6,
    WA_BOTTOM_CENTH = 1 << 7,

    WA_CENTER = 1 << 8
};


enum WidgetFlags {
    WF_PASSIVE = 0,

    WF_DRAW_BACKGROUND_AND_BORDER = 1 << 0,
    WF_DRAW_TEXT = 1 << 1,
    WF_CAN_COLLIDE = 1 << 2,

    WF_LAYOUT_CENTER = 1 << 10,
    WF_LAYOUT_HORIZONTAL = 1 << 11,
    WF_LAYOUT_VERTICAL = 1 << 12,
    WF_ALIGN_LEFT = 1 << 13,
    WF_ALIGN_RIGHT = 1 << 14,
    WF_ALIGN_CENTER = 1 << 15,

    WF_EXPAND_HORIZONTAL = 1 << 16,
    WF_EXPAND_VERTICAL = 1 << 17,

    WF_ABSREL_POSITION = 1 << 20
};
bool WidgetIsLayout(u32 features) {
    bool result =
        features & WF_LAYOUT_CENTER ||
        features & WF_LAYOUT_HORIZONTAL ||
        features & WF_LAYOUT_VERTICAL ||
    false;
    return result;
}


struct Widget {
    Widget *next;       // sibling in the branch
    Widget *first;      // child sub-branch first
    Widget *parent;     // parent of the branch

    u64 hash_key;       // hash for frame-boundary persistence
    u64 frame_touched;  // expiration date

    f32 x0;
    f32 y0;
    f32 w;
    f32 h;
    f32 w_max;
    f32 h_max;

    Str text;
    FontSize sz_font;
    s32 sz_border;
    Color col_bckgrnd;
    Color col_text;
    Color col_border;

    // DBG / experimental
    Color col_hot;
    Color col_active;
    bool hot;
    bool active;
    bool clicked;

    u32 features_flg;
    u32 alignment_flg;

    // everything below belongs in the layout algorithm
    CollRect rect;

    void CollRectClear() {
        rect = {};
    }
    void SetCollisionRectUsingX0Y0WH() {
        rect.x0 = x0;
        rect.x1 = x0 + w;
        rect.y0 = y0;
        rect.y1 = y0 + h;
    }
    void SetFeature(WidgetFlags f) {
        features_flg = features_flg |= f;
    }
};

void WidgetSetFlag(Widget *wgt, u32 flag) {
    wgt->features_flg |= flag;
}


//
//  Core


static MArena _g_a_imui;
static MArena *g_a_imui;
static MPoolT<Widget> _g_p_widgets;
static MPoolT<Widget> *g_p_widgets;
static Stack<Widget*> _g_s_widgets;
static Stack<Widget*> *g_s_widgets;

static HashMap _g_m_widgets;
static HashMap *g_m_widgets;

static Widget _g_w_root;
static Widget *g_w_layout;
static Widget *g_w_active;

static u64 *g_frameno_imui;


void TreeSibling(Widget *w) {
    if (g_w_layout->first != NULL) {
        Widget *sib = g_w_layout->first;
        while (sib->next != NULL) {
            sib = sib->next;
        }
        sib->next = w;
        w->parent = sib->parent;
    }
    else {
        g_w_layout->first = w;
        w->parent = g_w_layout;
    }
}

void TreeBranch(Widget *w) {
    if (g_w_layout->first != NULL) {
        Widget *sib = g_w_layout->first;
        while (sib->next != NULL) {
            sib = sib->next;
        }
        sib->next = w;
    }
    else {
        g_w_layout->first = w;
    }
    w->parent = g_w_layout;
    g_w_layout = w;
}

void TreePop() {
    Widget *parent = g_w_layout->parent;
    if (parent != NULL) {
        g_w_layout = parent;
    }
}


static f32 g_mouse_x;
static f32 g_mouse_y;
static bool g_mouse_down;
static bool g_mouse_pushed;


void InitImUi(u32 width, u32 height, u64 *frameno) {
    if (g_a_imui != NULL) {
        assert(1 == 0 && "don't re-initialize imui");
    }
    else {
        g_frameno_imui = frameno;

        MArena _g_a_imui = ArenaCreate();
        g_a_imui = &_g_a_imui;

        u32 max_widgets = 1024;
        _g_p_widgets = PoolCreate<Widget>(max_widgets);
        g_p_widgets = &_g_p_widgets;

        _g_s_widgets = InitStack<Widget*>(g_a_imui, max_widgets);
        g_s_widgets = &_g_s_widgets;

        _g_m_widgets = InitMap(g_a_imui, max_widgets);
        g_m_widgets = &_g_m_widgets;

        _g_w_root = {};
        _g_w_root.features_flg |= WF_LAYOUT_HORIZONTAL;
        _g_w_root.w_max = width;
        _g_w_root.h_max = height,
        _g_w_root.x0 = 0;
        _g_w_root.y0 = 0;

        g_w_layout = &_g_w_root;
    }

    SetDefaultFontSize(FS_24);
}


void WidgetTreeSizeWrap_Rec(Widget *w, s32 *w_sum, s32 *h_sum, s32 *w_max, s32 *h_max) {
    // Recursively determines widget sizes by wrapping in child widgets. 
    // Sizes will be the minimal, and expander sizes will be expanded elsewhere.

    // There is an accumulated child size and a max child size.
    // Depending on the layou of the current widget, its actual size
    // is set to either the maximum child widget.
    // Or, if a panel has the WF_LAYOUT_H or WR_LAYOUT_V features, the sum of
    // each child's actual size.
    //
    // max & sum sizes are determined on descent, actual sizes are set on ascent.


    //
    // Descent: determine child_max and child_sum sizes


    *w_sum = 0;
    *h_sum = 0;
    *w_max = 0;
    *h_max = 0;

    Widget *ch = w->first;
    while (ch != NULL) { // iterate child widgets
        s32 w_sum_ch;
        s32 h_sum_ch;
        s32 w_max_ch;
        s32 h_max_ch;

        WidgetTreeSizeWrap_Rec(ch, &w_sum_ch, &h_sum_ch, &w_max_ch, &h_max_ch);

        *w_sum += ch->w;
        *h_sum += ch->h;
        *w_max = MaxS32(*w_max, ch->w);
        *h_max = MaxS32(*h_max, ch->h);

        ch = ch->next;
    }


    //
    // Ascent: Assign actual size to current widget (where undefined)


    if (w->w == 0 && w->h == 0) {
        if (w->features_flg & WF_LAYOUT_CENTER) {
            w->w = *w_max;
            w->h = *h_max;
        }
        if (w->features_flg & WF_LAYOUT_HORIZONTAL) {
            w->w = *w_sum;
            w->h = *h_max;
        }
        else if (w->features_flg & WF_LAYOUT_VERTICAL) {
            w->w = *w_max;
            w->h = *h_sum;
        }
    }

    // or keep pre-sets
    else {
        *w_sum = w->w;
        *h_sum = w->h;
        *w_max = w->w;
        *h_max = w->h;
    }
}


void WidgetTreeExpand_Rec(Widget *w) {
    // expands one sub-widget using own dimensions

    Widget *ch = w->first;
    if (ch == NULL) {
        return;
    }

    // extract info
    bool expander_vert_found = false;
    bool expander_horiz_found = false;
    u32 width_other = 0;
    u32 height_other = 0;
    while (ch) {
        if ( ! (ch->features_flg & WF_EXPAND_VERTICAL)) {
            height_other += ch->h;
        }
        else {
            assert(w->features_flg & WF_LAYOUT_HORIZONTAL || expander_vert_found == false && "Expander not allowed (WF_EXPAND_VERTICAL)");
            expander_vert_found = true;
        }

        if ( ! (ch->features_flg & WF_EXPAND_HORIZONTAL)) {
            width_other += ch->w;
        }
        else {
            assert(w->features_flg & WF_LAYOUT_VERTICAL || expander_horiz_found == false && "Expander not allowed (WF_EXPAND_HORIZONTAL)");
            expander_horiz_found = true;
        }

        ch = ch->next;
    }

    // assign max possible size to the expander(s)
    ch = w->first;
    while (ch) {
        if (ch->features_flg & WF_EXPAND_VERTICAL) {
            ch->h = w->h - height_other;
        }
        if (ch->features_flg & WF_EXPAND_HORIZONTAL) {
            ch->w = w->w - width_other;
        }
        ch = ch->next;
    }

    // descend
    ch = w->first;
    while (ch) {
        WidgetTreeExpand_Rec(ch);

        ch = ch->next;
    }
}


List<Widget*> WidgetTreePositioning(MArena *a_tmp, Widget *w_root) {
    List<Widget*> all_widgets = InitList<Widget*>(a_tmp, 0);
    Widget *w = w_root;


    while (w != NULL) {
        ArenaAlloc(a_tmp, sizeof(Widget*));
        all_widgets.Add(w);

        s32 pt_x = 0;
        s32 pt_y = 0;

        // with all widget sizes known, widgets can position their children
        Widget *ch = w->first;
        while (ch != NULL) { // iterate child widgets

            // set child position - if not absolutely positioned
            if ((ch->features_flg & WF_ABSREL_POSITION) == false) {

                ch->x0 = w->x0;
                ch->y0 = w->y0;

                if (w->features_flg & WF_LAYOUT_CENTER) {
                    ch->y0 = w->y0 + (w->h - ch->h) / 2;
                    ch->x0 = w->x0 + (w->w - ch->w) / 2;
                }

                else if (w->features_flg & WF_LAYOUT_HORIZONTAL) {
                    ch->x0 = w->x0 + pt_x;
                    pt_x += ch->w;

                    if (w->features_flg & WF_ALIGN_CENTER) {
                        ch->y0 = w->y0 + (w->h - ch->h) / 2;
                    }
                    else if (w->features_flg & WF_ALIGN_RIGHT) {
                        ch->y0 = w->y0 + (w->h - ch->h);
                    }
                }

                else if (w->features_flg & WF_LAYOUT_VERTICAL) {
                    ch->y0 = w->y0 + pt_y;
                    pt_y += ch->h;

                    if (w->features_flg & WF_ALIGN_CENTER) {
                        ch->x0 = w->x0 + (w->w - ch->w) / 2;
                    }
                    else if (w->features_flg & WF_ALIGN_RIGHT) {
                        ch->x0 = w->x0 + (w->w - ch->w);
                    }
                }
            }

            if (ch->features_flg & WF_ABSREL_POSITION) {
                // basic offset wrt. parent
                if (ch->alignment_flg == 0) {
                    ch->x0 += w->x0;
                    ch->y0 += w->y0;
                }

                // alignment also specified
                else if (ch->alignment_flg & WA_TOP_LEFT) {
                    ch->x0 = w->x0 + ch->x0;
                    ch->y0 = w->y0 + ch->y0;
                }
                else if (ch->alignment_flg & WA_TOP_RIGHT) {
                    ch->x0 = w->x0 + ch->x0 + w->w - ch->w;
                    ch->y0 = w->y0 + ch->y0;
                }
                else if (ch->alignment_flg & WA_BOTTOM_LEFT) {
                    ch->x0 = w->x0 + ch->x0;
                    ch->y0 = w->y0 + ch->y0 + w->h - ch->h;
                }
                else if (ch->alignment_flg & WA_BOTTOM_RIGHT) {
                    ch->x0 = w->x0 + ch->x0 + w->w - ch->w;
                    ch->y0 = w->y0 + ch->y0 + w->h - ch->h;
                }
                else if (ch->alignment_flg & WA_CENTV_LEFT) {
                    ch->x0 = w->x0 + ch->x0;
                    ch->y0 = w->y0 + ch->y0 + (w->h - ch->h) / 2;
                }
                else if (ch->alignment_flg & WA_CENTV_RIGHT) {
                    ch->x0 = w->x0 + ch->x0 + w->w - ch->w;
                    ch->y0 = w->y0 + ch->y0 + (w->h - ch->h) / 2;
                }
                else if (ch->alignment_flg & WA_TOP_CENTH) {
                    ch->x0 = w->x0 + ch->x0 + (w->w - ch->w) / 2;
                    ch->y0 = w->y0 + ch->y0;
                }
                else if (ch->alignment_flg & WA_BOTTOM_CENTH) {
                    ch->x0 = w->x0 + ch->x0 + (w->w - ch->w) / 2;
                    ch->y0 = w->y0 + ch->y0 + w->h - ch->h;
                }
                else if (ch->alignment_flg & WA_CENTER) {
                    ch->x0 = w->x0 + ch->x0 + (w->w - ch->w) / 2;
                    ch->y0 = w->y0 + ch->y0 + (w->h - ch->h) / 2;
                }
            }


            // set the collision rect for next frame code-interleaved mouse collision
            ch->SetCollisionRectUsingX0Y0WH();

            ch->hot = false;
            ch->clicked = false;

            if (ch->features_flg & WF_CAN_COLLIDE) {
                if (ch->rect.DidCollide( (s32) g_mouse_x, (s32) g_mouse_y )) {
                    if (g_w_active == NULL) {
                        ch->hot = true;
                        if (g_mouse_down) {
                            // enable active mode
                            g_w_active = ch;
                            ch->active = true;
                        }
                    }
                    else {
                        // disable active mode
                        ch->active = false;
                    }
                }
                else {
                    ch->hot = false;
                }

                if (g_mouse_down == false) {
                    ch->active = false;
                    g_w_active = NULL;
                }

                if (g_mouse_pushed && ch->active) {
                    ch->clicked = true;
                }
            }

            // iter
            ch = ch->next;
        }

        // iter
        if (w->first != NULL) {
            if (w->next) {
                g_s_widgets->Push(w->next);
            }
            w = w->first;
        }
        else if (w->next) {
            w = w->next;
        }
        else {
            w = g_s_widgets->Pop();
        }
    }

    return all_widgets;
}


void WidgetTreeRenderToDrawcalls(List<Widget*> all_widgets) {


    // TODO: This should iterate the tree, and do something meaningful to figure out
    //      what should be rendered on top of what - proper interleaving of calls.


    for (u32 i = 0; i < all_widgets.len; ++i) {
        Widget *w = all_widgets.lst[i];

        if (w->features_flg & WF_DRAW_BACKGROUND_AND_BORDER) {
            PanelPlot(w->x0, w->y0, w->w, w->h, w->sz_border, w->col_border, w->col_bckgrnd);
        }

        if (w->features_flg & WF_DRAW_TEXT) {
            SetFontSize(w->sz_font);
            s32 w_out;
            s32 h_out;

            TextPlot(w->text, w->x0, w->y0, w->w, w->h, &w_out, &h_out, w->col_text);
        }
    }
}


void UI_FrameEnd(MArena *a_tmp, s32 width, s32 height) {
    if (g_mouse_down == false) {
        g_w_active = NULL;
    }

    Widget *w = &_g_w_root;
    w->w_max = width;
    w->h_max = height;
    w->w = w->w_max;
    w->h = w->h_max;

    // size widgets to wrap tightly
    s32 w_sum_ch;
    s32 h_sum_ch;
    s32 w_max_ch;
    s32 h_max_ch;
    WidgetTreeSizeWrap_Rec(w, &w_sum_ch, &h_sum_ch, &w_max_ch, &h_max_ch);

    // size pass
    WidgetTreeExpand_Rec(w);

    // position pass
    List<Widget*> all_widgets = WidgetTreePositioning(a_tmp, w);

    // render pass
    WidgetTreeRenderToDrawcalls(all_widgets);

    // clean up pass
    _g_w_root.frame_touched = *g_frameno_imui;
    g_w_layout = &_g_w_root;
    for (u32 i = 0; i < all_widgets.len; ++i) {
        Widget *w = all_widgets.lst[i];

        // prune
        if (w->frame_touched < *g_frameno_imui) {
            MapRemove(g_m_widgets, w->hash_key, w); 
            g_p_widgets->Free(w);
        }
        // clean
        else {
            if (w->hash_key != 0) {
                MapPut(g_m_widgets, w->hash_key, w);
            }
            w->parent = NULL;
            w->first = NULL;
            w->next = NULL;
        }
    }
}


//
//  Builder API


Widget *WidgetGetCached(const char *text, bool *was_new = NULL) {
    u64 key = HashStringValue(text);
    Widget *w = (Widget*) MapGet(g_m_widgets, key);

    if (w == NULL) {
        w = g_p_widgets->Alloc();
        MapPut(g_m_widgets, key, w);
        w->hash_key = key;

        w->text = Str { (char*) text, (u32) strlen( (char*) text) };
        if (was_new) *was_new = true;
    }
    else {
        assert(key == w->hash_key);
        assert(w->frame_touched != *g_frameno_imui);
        if (was_new) *was_new = false;
    }

    w->frame_touched = *g_frameno_imui;

    return w;
}

Widget *WidgetGetNew(const char *text = NULL) {
    Widget *w = g_p_widgets->Alloc();
    assert(w->frame_touched == 0);
    if (text) {
        w->text = Str { (char*) text, (u32) strlen( (char*) text) };
    }

    return w;
}


bool UI_Button(const char *text, Widget **w_out = NULL) {
    Widget *w  = WidgetGetCached(text);
    w->features_flg |= WF_DRAW_TEXT;
    w->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
    w->features_flg |= WF_CAN_COLLIDE;
    w->w = 120;
    w->h = 50;
    w->sz_font = FS_24;

    if (w->active) {
        w->sz_border = 3;
        w->col_bckgrnd = COLOR_GRAY_75;
        w->col_text = COLOR_BLACK;
        w->col_border = COLOR_BLACK;
    }
    else if (w->hot) {
        w->sz_border = 3;
        w->col_bckgrnd = COLOR_WHITE;
        w->col_text = COLOR_BLACK;
        w->col_border = COLOR_BLACK;
    }
    else {
        w->sz_border = 1;
        w->col_bckgrnd = COLOR_WHITE;
        w->col_text = COLOR_BLACK;
        w->col_border = COLOR_BLACK;
    }

    TreeSibling(w);

    if (w_out != NULL) {
        *w_out = w;
    }
    return w->clicked;
}


bool UI_ToggleButton(const char *text, bool *state, Widget **w_out = NULL) {
    Widget *w  = WidgetGetCached(text);
    w->features_flg |= WF_DRAW_TEXT;
    w->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
    w->features_flg |= WF_CAN_COLLIDE;
    w->w = 120;
    w->h = 50;
    w->sz_font = FS_24;

    if (w->clicked) {
        *state = !(*state);
    }

    if (*state == true) {
        w->sz_border = 3;
        w->col_bckgrnd = ColorGray(0.8f);
        w->col_text = ColorBlack();
        w->col_border = ColorBlack();
    }
    else if (w->hot) {
        w->sz_border = 3;
        w->col_bckgrnd = ColorWhite();
        w->col_text = ColorBlack();
        w->col_border = ColorBlack();
    }
    else {
        w->sz_border = 1;
        w->col_bckgrnd = COLOR_WHITE;
        w->col_text = ColorBlack();
        w->col_border = ColorBlack();
    }

    TreeSibling(w);

    if (w_out != NULL) {
        *w_out = w;
    }
    return w->clicked;
}


Widget *UI_CoolPanel(s32 width, s32 height, bool center_h = true) {
    Widget *w = WidgetGetNew();
    w->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
    w->features_flg |= WF_LAYOUT_VERTICAL;
    if (center_h) {
        w->features_flg |= WF_ALIGN_CENTER;
    }
    w->w = width;
    w->h = height;
    w->sz_border = 20;
    w->col_bckgrnd = ColorGray(0.9f);
    w->col_border = ColorGray(0.7f);

    TreeBranch(w);

    return w;
}


bool UI_CrossButton(const char *symbol, Widget **w_out = NULL) {
    Widget *x = WidgetGetCached(symbol);
    if (w_out) *w_out = x;

    x->features_flg |= WF_ABSREL_POSITION;
    x->features_flg |= WF_DRAW_TEXT;
    x->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
    x->features_flg |= WF_CAN_COLLIDE;
    x->alignment_flg |= WA_TOP_RIGHT;
    x->sz_border = 1;
    x->col_border = COLOR_BLACK;
    x->col_bckgrnd = COLOR_WHITE;
    if (x->hot) {
        x->col_bckgrnd = COLOR_GRAY_75;
    }
    x->col_text = COLOR_BLACK;
    x->text = Str { (char*) symbol, 1 };
    x->sz_font = FS_18;
    x->w = 25;
    x->h = 25;

    TreeSibling(x);

    return x->clicked;
}


Widget *UI_CoolPopUp(s32 width, s32 height, s32 padding = 20, bool *close = NULL) {
    Widget *w = WidgetGetNew();
    w->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
    w->features_flg |= WF_LAYOUT_CENTER;

    w->w = width;
    w->h = height;
    w->sz_border = 20;
    w->col_bckgrnd = ColorGray(0.9f);
    w->col_border = ColorGray(0.7f);

    TreeBranch(w);

    Widget *x;
    bool cross_clicked = UI_CrossButton("x", &x);
    if (close) *close = cross_clicked;
    x->w = 18;
    x->h = 18;
    x->x0 = -1;
    x->y0 = 1;
    x->sz_border = 0;
    if (x->hot) {
        x->col_bckgrnd = COLOR_GRAY_50;
    }
    else {
        x->col_bckgrnd = w->col_border;
    }

    Widget *i = WidgetGetNew();
    i->features_flg |= WF_LAYOUT_VERTICAL;
    i->w = width - padding * 2;
    i->h = height - padding * 2;

    TreeBranch(i);

    return i;
}

Widget *UI_Plain() {
    Widget *w = WidgetGetNew();

    TreeBranch(w);
    return w;
}

Widget *UI_LayoutExpandCenter() {
    Widget *w = WidgetGetNew();

    w->features_flg |= WF_EXPAND_VERTICAL;
    w->features_flg |= WF_EXPAND_HORIZONTAL;
    w->features_flg |= WF_LAYOUT_CENTER;

    TreeBranch(w);
    return w;
}

Widget *UI_LayoutHorizontal() {
    Widget *w = WidgetGetNew();
    w->features_flg |= WF_LAYOUT_HORIZONTAL;

    TreeBranch(w);
    return w;
}

Widget *UI_LayoutVertical(bool center_vertical = false) {
    Widget *w = WidgetGetNew();

    w->features_flg |= WF_LAYOUT_VERTICAL;
    if (center_vertical) {
        w->features_flg |= WF_ALIGN_CENTER;
    }

    TreeBranch(w);
    return w;
}

void UI_SpaceH(u32 width) {
    Widget *w = WidgetGetNew();
    w->w = width;

    TreeSibling(w);
}

void UI_SpaceV(u32 height) {
    Widget *w = WidgetGetNew();
    w->h = height;

    TreeSibling(w);
}

Widget *UI_Label(const char *text, Color color = Color { RGBA_BLACK }) {
    Widget *w = WidgetGetNew(text);
    w->features_flg |= WF_DRAW_TEXT;

    w->sz_font = GetFontSize();
    w->col_bckgrnd = ColorGray(0.9f);
    w->col_border = ColorBlack();
    w->col_text = color;

    w->w = TextLineWidth(g_text_plotter, w->text);
    w->h = g_text_plotter->ln_measured;

    TreeSibling(w);
    return w;
}

void UI_Pop() {
    TreePop();
}


#endif

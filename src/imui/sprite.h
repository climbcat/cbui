#ifndef __SPRITE_H__
#define __SPRITE_H__


struct Frame {
    s32 w;
    s32 h;
    f32 u0;
    f32 u1;
    f32 v0;
    f32 v1;
};

struct Animation {
    Str name;
    s32 top;
    s32 width;
    s32 height;
    Array<Frame> frames;
    Array<f32> durations;
};

struct SpriteSheet {
    Str name;
    Str filename;
    ImageRGBA sheet;
    Array<Animation> animations;

    // helper
    s32 top_accum;
};


static SpriteSheet *active_sheet;
static Animation *active_animation;


SpriteSheet *SS_Sheet(MArena *a_dest, HashMap *map_dest, Str filename, Str sheet_name, s32 data_width, s32 data_height, s32 animation_cnt) {
    assert(active_sheet == NULL);

    active_sheet = (SpriteSheet*) ArenaAlloc(a_dest, sizeof(SpriteSheet));

    active_sheet->filename = StrPush(a_dest, filename);
    active_sheet->name = StrPush(a_dest, sheet_name);
    active_sheet->sheet.width = data_width;
    active_sheet->sheet.height = data_height;
    active_sheet->sheet.img = (Color*) LoadFileFSeek(a_dest, filename);
    active_sheet->animations = InitArray<Animation>(a_dest, animation_cnt);

    MapPut(map_dest, active_sheet->name, active_sheet);
    return active_sheet;
}

void SS_Animation(MArena *a_dest, Str name, s32 width, s32 height, s32 frames_cnt) {
    assert(active_sheet);
    assert(active_animation == NULL);

    Animation animation = {};

    animation.name = StrPush(a_dest, name);
    animation.width = width;
    animation.height = height;
    animation.top = active_sheet->top_accum;
    animation.frames = InitArray<Frame>(a_dest, frames_cnt);
    animation.durations = InitArray<f32>(a_dest, frames_cnt);

    s32 y = animation.top;
    f32 v0 = y / (f32) height;
    f32 v1 = (y + height) / (f32) height;

    for (s32 i = 0; i < animation.frames.len; ++i) {
        s32 x = i * width;

        Frame f = {};
        f.w = width;
        f.h = height;
        f.u0 = x / (f32) width;
        f.u1 = (x + width) / (f32) width;
        f.v0 = y / (f32) height;
        f.v1 = (y + height) / (f32) height;

        animation.frames.Add(f);
    }

    active_animation = active_sheet->animations.Add(animation);
    active_sheet->top_accum += height;

    // safeguard - close the sprite sheet config
    if (active_sheet->animations.len == active_sheet->animations.max) {
        active_sheet = NULL;
    }
}

void SS_FrameDuration(f32 duration) {
    assert(active_animation);

    active_animation->durations.Add(duration);

    // safeguard - close the animation config
    if (active_animation->durations.len == active_animation->durations.max) {
        active_animation = NULL;
    }
}

void SS_CloseSheet() {
    assert(active_sheet == NULL);
    assert(active_animation == NULL);
}


void SS_Print(SpriteSheet *sheet) {
    StrPrint("loaded sheet: ", sheet->name, "");
    StrPrint(" (", sheet->filename, ")\n");

    for (s32 i = 0; i < sheet->animations.len; ++i) {
        Animation a = sheet->animations.arr[i];
        StrPrint("    ", a.name, ": ");

        for (s32 i = 0; i < a.durations.len; ++i) {
            printf("%.0f ", a.durations.arr[i]);
        }
        printf("\n");
    }
}


// TODO: data access API
/*
access:

Frame GetAnimationFrame(u64 sheet_id, u64 animation_id, s32 frame_idx)
*/


// TODO: reference API
/*
reference:
struct AnimatedEntity {
    s32 animation_idx;
    s32 frame_idx;
    f32 t_frame_elapsed;
};
*/



/*

 void Update(PlayerEntity entity) {
    if (KeyIsPressed(SPACE)) {
        entity.CurrentAnimationIndex = JUMP_ANIMATION_INDEX; // Hardcoded, but you can also replace with a lookup by name.
        entity.CurrentAnimationFrameIndex = 0;
        entity.CurrentAnimationFrameElapsedTime = 0;
    }

    Animation animation = entity.Animations[entity.CurrentAnimationIndex];
    AnimationFrame frame = animation.Frames[entity.CurrentAnimationFrameIndex];
    entity.CurrentAnimationFrameElapsedTime += deltaTime;

    if (entity.CurrentAnimationFrameElapsedTime > frame.Duration) {

        entity.CurrentAnimationFrameIndex++;
        entity.CurrentAnimationFrameElapsedTime = 0;
        if (entity.CurrentAnimationFrameIndex == animation.NumFrames) {
            entity.CurrentAnimationFrameIndex = 0; // Animation ended, loop it from beginning, or do something else.   
        }
    }
}

*/


//
//  Legacy Sprite type


struct Sprite {
    s32 w;
    s32 h;
    f32 u0;
    f32 u1;
    f32 v0;
    f32 v1;
};


#endif

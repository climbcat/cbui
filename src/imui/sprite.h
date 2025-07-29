#ifndef __SPRITE_H__
#define __SPRITE_H__


struct Frame {
    s32 w;
    s32 h;
    f32 u0;
    f32 u1;
    f32 v0;
    f32 v1;
    f32 duration;
};

struct Animation {
    Str name;
    Array<Frame> frames;
};

struct Spriteheet {
    Str name;
    ImageRGBA sheet;
    Array<Animation> animations;
};

// referencing it:
struct AnimatedEntity {
    s32 animation_idx;
    s32 frame_idx;
    f32 t_frame_elapsed;
};


/*
layout:

[Sheet]  [Animation, Animation, ...]  [[Frame, Frame, ...], [Frame, Frame, ...], ... ]  [data]

This leads to some amount of pointer chasing, but here's an alternative:

[Sheet]  mapped[ani_id, frm_id] -> [Animation]  [Frame, Frame, ...]  ->  [Animation] [Frame, Frame ...]   [data]

Interleaving the Animation struct by having an elaborate mapping system, using animation names / ids.
*/


/*
construction:

a) Given png file: A .h file would compile this information in runtime.
b) Given png file: A .h code would compile a sheet in binary form ; requires a loaded as well

SS_Sheet(MArena *a_dest, Str filename, Str sheet_name) -> u64 [sprite_sheet_id]
SS_Animation(MArena *a_dest, Str animation_name, s32 y_start, s32 row_height, s32 frame_width) -> u64 [animation_id]
SS_Frame(f32 duration) -> s32 [frame_idx];
*/


/*
access:

Frame GetAnimationFrame(u64 sheet_id, u64 animation_id, s32 frame_idx)
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

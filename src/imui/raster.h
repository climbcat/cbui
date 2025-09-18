#ifndef __RASTER_H__
#define __RASTER_H__


//
//  Line rasterization
//


inline
bool _CullScreenCoords(u32 pos_x, u32 pos_y, u32 w, u32 h) {
    // returns true if the coordinate is out of range
    bool not_result = pos_x >= 0 && pos_x < w && pos_y >= 0 && pos_y < h;
    return !not_result;
}


Color SampleTexture(f32 x, f32 y, Color col_default, s32 src_width, s32 src_height, Color *src_buffer);


inline
Color BlendColors(Color background, Color color) {
    Color color_blended = {};
    if (color.a != 0) {
        f32 alpha = (1.0f * color.a) / 255;
        color_blended.r = (u8) (floor( alpha*color.r ) + floor( (1-alpha)*background.r ));
        color_blended.g = (u8) (floor( alpha*color.g ) + floor( (1-alpha)*background.g ));
        color_blended.b = (u8) (floor( alpha*color.b ) + floor( (1-alpha)*background.b ));
        color_blended.a = 255;
    }
    return color_blended;
}


void RenderLineRGBA(u8* image_buffer, u16 w, u16 h, s16 ax, s16 ay, s16 bx, s16 by, Color color) {

    // initially working from a to b
    // there are four cases:
    // 1: slope <= 1, ax < bx
    // 2: slope <= 1, ax > bx 
    // 3: slope > 1, ay < by
    // 4: slope > 1, ay > by 

    f32 slope_ab = (f32) (by - ay) / (bx - ax);
    Color *buff = (Color*) image_buffer;

    if (abs(slope_ab) <= 1) {
        // draw by x
        f32 slope = slope_ab;

        // swap?
        if (ax > bx) {
            u16 swapx = ax;
            u16 swapy = ay;

            ax = bx;
            ay = by;
            bx = swapx;
            by = swapy;
        }

        s16 x, y;
        u32 pix_idx;
        for (s32 i = 0; i <= bx - ax; ++i) {
            x = ax + i;
            y = ay + (s16) floor(slope * i);

            if (_CullScreenCoords(x, y, w, h)) {
                continue;
            }            

            pix_idx = x + y*w;
            buff[pix_idx] = BlendColors(buff[pix_idx], color);
        }
    }
    else {
        // draw by y
        f32 slope_inv = 1 / slope_ab;

        // swap a & b ?
        if (ay > by) {
            u16 swapx = ax;
            u16 swapy = ay;

            ax = bx;
            ay = by;
            bx = swapx;
            by = swapy;
        }

        s16 x, y;
        u32 pix_idx;
        for (u16 i = 0; i <= by - ay; ++i) {
            y = ay + i;
            x = ax + (s16) floor(slope_inv * i);

            if (_CullScreenCoords(x, y, w, h)) {
                continue;
            }

            pix_idx = x + y*w;
            buff[pix_idx] = BlendColors(buff[pix_idx], color);
        }
    }
}

inline
u32 GetXYIdx(f32 x, f32 y, u32 stride) {
    u32 idx = floor(x) + stride * floor(y);
    return idx;
}

void RenderPoint(u8 *image_buffer, Vector3f point_ndc, u32 w, u32 h, Color color = COLOR_RED) {
    f32 x = (point_ndc.x + 1) / 2 * w;
    f32 y = (point_ndc.y + 1) / 2 * h;
    ((Color*) image_buffer)[ GetXYIdx(x, y, w) ] = color;
}

void RenderFatPoint3x3(u8 *image_buffer, Matrix4f view, Matrix4f proj, Vector3f point, u32 w, u32 h, Color color = COLOR_RED) {
    Vector3f point_cam = TransformInversePoint(view, point);

    Ray view_plane = { Vector3f { 0, 0, 0.1 }, Vector3f { 0, 0, 1 } };
    Ray view_plane_far = { Vector3f { 0, 0, 1 }, Vector3f { 0, 0, 1 } };

    if (PointSideOfPlane(point_cam, view_plane) == false) {
        return;
    }
    Vector3f point_ndc = TransformPerspective(proj, point_cam);

    f32 x = (point_ndc.x + 1) / 2 * w;
    f32 y = (point_ndc.y + 1) / 2 * h;

    for (s32 i = -1; i < 2; ++i) {
        for (s32 j = -1; j < 2; ++j) {
            if (x + i < 0 || y + j < 0) {
                continue;;
            }
            if (x + i >= w || y + j >= h) {
                continue;;
            }
            ((Color*) image_buffer)[ GetXYIdx(x + i, y + j, w) ] = color;
        }
    }
}


bool PlaneBooleanOnLineSegment(Ray plane, Vector3f *p1, Vector3f *p2) {
    Vector3f segment_dir = *p2 - *p1;
    segment_dir.Normalize();
    Ray segment_ray = { *p1, segment_dir };

    bool p1_behind = (*p1 - plane.pos).Dot(plane.dir) < 0;
    bool p2_behind = (*p2 - plane.pos).Dot(plane.dir) < 0;

    if (p1_behind && p2_behind) {
        return false;
    }
    else if (p1_behind) {
        // intersect p1

        *p1 = RayPlaneIntersect(segment_ray, plane.pos, plane.dir);
    }
    else if (p2_behind) {
        // intersect p2

        *p2 = RayPlaneIntersect(segment_ray, plane.pos, plane.dir);
    }
    return true;
}


inline
void RenderLineSegment(u8 *image_buffer, Matrix4f view, Perspective persp, Vector3f p1, Vector3f p2, u32 w, u32 h, Color color, bool do_fat_style = false) {

    Vector3f p1_cam = TransformPoint(view, p1);
    Vector3f p2_cam = TransformPoint(view, p2);

    bool is_visible_n = PlaneBooleanOnLineSegment(persp.PlaneNear(), &p1_cam, &p2_cam);
    bool is_visible_l = PlaneBooleanOnLineSegment(persp.PlaneLeft(), &p1_cam, &p2_cam);
    bool is_visible_r = PlaneBooleanOnLineSegment(persp.PlaneRight(), &p1_cam, &p2_cam);
    bool is_visible_t = PlaneBooleanOnLineSegment(persp.PlaneTop(), &p1_cam, &p2_cam);
    bool is_visible_b = PlaneBooleanOnLineSegment(persp.PlaneBottom(), &p1_cam, &p2_cam);
    bool is_visible = is_visible_n && is_visible_l && is_visible_r && is_visible_t && is_visible_b;

    if (is_visible) {
        Vector3f p1_ndc = TransformPerspective(persp.proj, p1_cam);
        Vector3f p2_ndc = TransformPerspective(persp.proj, p2_cam);

        Vector2f a = {};
        a.x = (p1_ndc.x + 1) / 2 * w;
        a.y = (p1_ndc.y + 1) / 2 * h;
        Vector2f b = {};
        b.x = (p2_ndc.x + 1) / 2 * w;
        b.y = (p2_ndc.y + 1) / 2 * h;

        RenderLineRGBA(image_buffer, w, h, a.x, a.y, b.x, b.y, color);



        if (do_fat_style == false) {
            RenderLineRGBA(image_buffer, w, h, a.x, a.y, b.x, b.y, color);
        }
        else {
            RenderLineRGBA(image_buffer, w, h, a.x, a.y, b.x, b.y, color);
            RenderLineRGBA(image_buffer, w, h, a.x+1, a.y, b.x+1, b.y, color);
            RenderLineRGBA(image_buffer, w, h, a.x, a.y+1, b.x, b.y+1, color);
        }
    }
}


//
//  Blitting


inline
Color SampleTexture(f32 x, f32 y, Color col_default, s32 src_width, s32 src_height, Color *src_buffer) {
    s32 i = (s32) round(src_width * x);
    s32 j = (s32) round(src_height * y);
    if (i < 0 || i >= src_width || j < 0 || j >= src_height) {
        return col_default;
    }
    u32 idx = src_width * j + i;
    Color b = src_buffer[idx];
    return b;
}

inline
void Blit32Bit(s32 width, s32 height, s32 left, s32 top, f32 u0, f32 u1, f32 v0, f32 v1, s32 src_width, s32 src_height, Color *src_buffer, s32 dest_width, s32 dest_height, Color *dest) {

    assert(dest_height >= width);
    assert(dest_width >= height);

    f32 q_scale_x = (u1 - u0) / width;
    f32 q_scale_y = (v1 - v0) / height;

    // i,j          : target coords
    // i_img, j_img : img coords

    for (s32 j = 0; j < height; ++j) {
        s32 j_img = j + top;
        if (j_img < 0 || j_img > dest_height) {
            continue;
        }

        for (s32 i = 0; i < width; ++i) {
            s32 i_img = left + i;
            if (i_img < 0 || i_img > dest_width) {
                continue;
            }
            f32 x = u0 + i * q_scale_x;
            f32 y = v0 + j * q_scale_y;

            // TODO: how do we regularize this code?
            Color color_src = SampleTexture(x, y, Color { 0, 0, 0, 255 }, src_width, src_height, src_buffer);

            if (color_src.a != 0) {
                // rudimentary alpha-blending
                s32 idx = j_img * dest_width + i_img;
                Color color_background = dest[idx];

                f32 alpha = (1.0f * color_src.a) / 255;
                Color color_blended;
                color_blended.r = (u8) (floor( alpha*color_src.r ) + floor( (1-alpha)*color_background.r ));
                color_blended.g = (u8) (floor( alpha*color_src.g ) + floor( (1-alpha)*color_background.g ));
                color_blended.b = (u8) (floor( alpha*color_src.b ) + floor( (1-alpha)*color_background.b ));
                color_blended.a = 255;

                dest[idx] = color_blended;
            }
        }
    }
}

inline
void BlitFill(s32 q_w, s32 q_h, f32 q_x0, f32 q_y0, Color q_color, s32 dest_width, s32 dest_height, Color* dest) {
    s32 j_img;
    s32 i_img;
    u32 idx;
    for (s32 j = 0; j < q_h; ++j) {
        j_img = j + q_y0;
        if (j_img < 0 || j_img > dest_height) {
            continue;
        }

        for (s32 i = 0; i < q_w; ++i) {
            i_img = q_x0 + i;
            if (i_img < 0 || i_img > dest_width) {
                continue;
            }

            idx = j_img * dest_width + i_img;
            Color color_src = q_color;


            if (q_color.a == 123) {
                printf("her\n");
            }

            if (color_src.a != 0) {
                // rudimentary alpha-blending
                s32 idx = j_img * dest_width + i_img;
                Color color_background = dest[idx];

                f32 alpha = (1.0f * color_src.a) / 255;
                Color color_blended;
                color_blended.r = (u8) (floor( alpha*color_src.r ) + floor( (1-alpha)*color_background.r ));
                color_blended.g = (u8) (floor( alpha*color_src.g ) + floor( (1-alpha)*color_background.g ));
                color_blended.b = (u8) (floor( alpha*color_src.b ) + floor( (1-alpha)*color_background.b ));
                color_blended.a = 255;

                dest[idx] = color_blended;
            }
        }
    }
}


inline
u8 SampleTexture(f32 x, f32 y, s32 src_width, s32 src_height, u8 *src_buffer) {
    u32 i = (s32) round(src_width * x);
    u32 j = (s32) round(src_height * y);
    u32 idx = src_width * j + i;
    u8 b = src_buffer[idx];
    return b;
}

inline
void Blit8Bit(s32 width, s32 height, f32 x0, f32 y0, f32 q_u0, f32 v0, f32 u1, f32 v1, Color color, s32 src_width, s32 src_height, u8 *src_buffer, s32 dest_width, s32 dest_height, Color* dest_buffer) {
    // i,j          : target coords
    // i_img, j_img : img coords

    f32 q_scale_x = (u1 - q_u0) / width;
    f32 q_scale_y = (v1 - v0) / height;

    s32 stride_dest = dest_width;

    for (s32 j = 0; j < height; ++j) {
        s32 j_dest = j + y0;
        if (j_dest < 0 || j_dest > dest_height) {
            continue;
        }

        for (s32 i = 0; i < width; ++i) {
            s32 i_dest = x0 + i;
            if (i_dest < 0 || i_dest > dest_width) {
                continue;
            }
            f32 x = q_u0 + i * q_scale_x;
            f32 y = v0 + j * q_scale_y;
            if (u8 alpha_byte = SampleTexture(x, y, src_width, src_height, src_buffer)) {
                // rudimentary alpha-blending
                u32 idx = (u32) (j_dest * dest_width + i_dest);
                Color color_background = dest_buffer[idx];

                f32 alpha = (1.0f * alpha_byte) / 255;
                Color color_blended;
                color_blended.r = (u8) (floor( alpha*color.r ) + floor( (1-alpha)*color_background.r ));
                color_blended.g = (u8) (floor( alpha*color.g ) + floor( (1-alpha)*color_background.g ));
                color_blended.b = (u8) (floor( alpha*color.b ) + floor( (1-alpha)*color_background.b ));
                color_blended.a = 255;

                dest_buffer[idx] = color_blended;
            }
        }
    }
}


//
//  Render / control buffer API


Array<Sprite> g_sprite_buffer;


void SpriteBufferInit(MArena *a_dest, u32 max_quads = 2048) {
    g_sprite_buffer = InitArray<Sprite>(a_dest, max_quads);
}

void SpriteBufferPush(Sprite sprite) {
    g_sprite_buffer.Add(sprite);
}

void SpriteBufferBlitAndClear(HashMap map_textures, s32 dest_width, s32 dest_height, u8 *dest_buffer) {

    for (s32 i = 0; i < g_sprite_buffer.len; ++i) {

        Sprite s = g_sprite_buffer.arr[i];
        Texture *s_texture = (Texture*) MapGet(&map_textures, s.tex_id);

        if (s_texture == NULL) {

            BlitFill(s.w, s.h, s.x0, s.y0, s.color, dest_width, dest_height, (Color*) dest_buffer);
        }

        else if (s_texture && s_texture->tpe == TT_8BIT ) {

            Blit8Bit(s.w, s.h, s.x0, s.y0, s.u0, s.v0, s.u1, s.v1, s.color, s_texture->width, s_texture->height, s_texture->data, dest_width, dest_height, (Color*) dest_buffer);
        }

        else if (s_texture && s_texture->tpe == TT_RGBA) {

            Blit32Bit(s.w, s.h, s.x0, s.y0, s.u0, s.u1, s.v0, s.v1, s_texture->width, s_texture->height, (Color*) s_texture->data, dest_width, dest_height, (Color*) dest_buffer);
        }

        else {
            printf("WARN: Attempt to blit unknown texture type\n");
        }
    }

    g_sprite_buffer.len = 0;
}


#endif

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

void RenderLineRGBA(u8* image_buffer, u16 w, u16 h, s16 ax, s16 ay, s16 bx, s16 by, Color color) {

    // initially working from a to b
    // there are four cases:
    // 1: slope <= 1, ax < bx
    // 2: slope <= 1, ax > bx 
    // 3: slope > 1, ay < by
    // 4: slope > 1, ay > by 

    f32 slope_ab = (f32) (by - ay) / (bx - ax);

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
            image_buffer[4 * pix_idx + 0] = color.r;
            image_buffer[4 * pix_idx + 1] = color.g;
            image_buffer[4 * pix_idx + 2] = color.b;
            image_buffer[4 * pix_idx + 3] = color.a;
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
            image_buffer[4 * pix_idx + 0] = color.r;
            image_buffer[4 * pix_idx + 1] = color.g;
            image_buffer[4 * pix_idx + 2] = color.b;
            image_buffer[4 * pix_idx + 3] = color.a;
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

inline
void RenderLineSegment(u8 *image_buffer, Matrix4f view, Matrix4f proj, Vector3f p1, Vector3f p2, u32 w, u32 h, Color color) {
    Vector3f p1_cam = TransformInversePoint(view, p1);
    Vector3f p2_cam = TransformInversePoint(view, p2);

    Ray view_plane = { Vector3f { 0, 0, 0.1 }, Vector3f { 0, 0, 1 } };

    bool visible1 = PointSideOfPlane(p1_cam, view_plane);
    bool visible2 = PointSideOfPlane(p2_cam, view_plane);

    if (visible1 == true || visible2 == true) {
        if (visible1 == false && visible2 == true) {
            Ray segment = { p2_cam, p1_cam - p2_cam };
            f32 t = 0;
            p1_cam = RayPlaneIntersect(segment, view_plane.pos, view_plane.dir, &t);
        }
        else if (visible1 == true && visible2 == false) {
            Ray segment = { p1_cam, p2_cam - p1_cam };
            f32 t = 0;
            p2_cam = RayPlaneIntersect(segment, view_plane.pos, view_plane.dir, &t);
        }

        Vector3f p1_ndc = TransformPerspective(proj, p1_cam);
        Vector3f p2_ndc = TransformPerspective(proj, p2_cam);

        Vector2f a = {};
        a.x = (p1_ndc.x + 1) / 2 * w;
        a.y = (p1_ndc.y + 1) / 2 * h;
        Vector2f b = {};
        b.x = (p2_ndc.x + 1) / 2 * w;
        b.y = (p2_ndc.y + 1) / 2 * h;

        RenderLineRGBA(image_buffer, w, h, a.x, a.y, b.x, b.y, color);
    }
}


//
//  Blitting
//


inline
Color SampleTexture(Texture *tex, f32 x, f32 y, Color col_default) {
    s32 i = (s32) round(tex->width * x);
    s32 j = (s32) round(tex->height * y);
    if (i < 0 || i >= tex->width || j < 0 || j >= tex->height) {
        return col_default;
    }
    u32 idx = tex->width * j + i;
    Color *tex_colors = (Color*) tex->data;
    Color b = tex_colors[idx];
    return b;
}


void BlitSprite(s32 width, s32 height, s32 left, s32 top, f32 u0, f32 u1, f32 v0, f32 v1, Texture *dest, Texture *src) {

    assert(dest->height >= width);
    assert(dest->width >= height);

    Color *color_src = (Color*) src->data;
    Color *color_dest = (Color*) dest->data;

    u32 stride_img = dest->width;

    f32 q_scale_x = (u1 - u0) / width;
    f32 q_scale_y = (v1 - v0) / height;

    // i,j          : target coords
    // i_img, j_img : img coords

    for (s32 j = 0; j < height; ++j) {
        s32 j_img = j + top;
        if (j_img < 0 || j_img > dest->height) {
            continue;
        }

        for (s32 i = 0; i < width; ++i) {
            s32 i_img = left + i;
            if (i_img < 0 || i_img > dest->width) {
                continue;
            }
            f32 x = u0 + i * q_scale_x;
            f32 y = v0 + j * q_scale_y;

            // TODO: how do we regularize this code?
            Color color_src = SampleTexture(src, x, y, Color { 0, 0, 0, 255 });

            if (color_src.a != 0) {
                // rudimentary alpha-blending
                s32 idx = j_img * stride_img + i_img;
                Color color_background = color_dest[idx];

                f32 alpha = (1.0f * color_src.a) / 255;
                Color color_blended;
                color_blended.r = (u8) (floor( alpha*color_src.r ) + floor( (1-alpha)*color_background.r ));
                color_blended.g = (u8) (floor( alpha*color_src.g ) + floor( (1-alpha)*color_background.g ));
                color_blended.b = (u8) (floor( alpha*color_src.b ) + floor( (1-alpha)*color_background.b ));
                color_blended.a = 255;

                color_dest[idx] = color_blended;
            }
        }
    }
}



//
//  NOTE: imported from the "quad" blitter


inline
u8 SampleTexture(ImageB *tex, f32 x, f32 y) {
    u32 i = (s32) round(tex->width * x);
    u32 j = (s32) round(tex->height * y);
    u32 idx = tex->width * j + i;
    u8 b = tex->img[idx];
    return b;
}

void BlitMonoColor(s32 q_w, s32 q_h, f32 q_x0, f32 q_y0, Color q_color, ImageRGBA img) {
    s32 j_img;
    s32 i_img;
    u32 idx;
    for (s32 j = 0; j < q_h; ++j) {
        j_img = j + q_y0;
        if (j_img < 0 || j_img > img.height) {
            continue;
        }

        for (s32 i = 0; i < q_w; ++i) {
            i_img = q_x0 + i;
            if (i_img < 0 || i_img > img.width) {
                continue;
            }

            idx = j_img * img.width + i_img;
            img.img[idx] = q_color;
        }
    }

}

void BlitGlyph(s32 q_w, s32 q_h, f32 q_x0, f32 q_y0, f32 q_u0, f32 q_v0, f32 q_scale_x, f32 q_scale_y, Color q_color, ImageB *src, ImageRGBA dest) {
    // i,j          : target coords
    // i_img, j_img : img coords

    s32 stride_dest = dest.width;

    for (s32 j = 0; j < q_h; ++j) {
        s32 j_dest = j + q_y0;
        if (j_dest < 0 || j_dest > dest.height) {
            continue;
        }

        for (s32 i = 0; i < q_w; ++i) {
            s32 i_dest = q_x0 + i;
            if (i_dest < 0 || i_dest > dest.width) {
                continue;
            }
            f32 x = q_u0 + i * q_scale_x;
            f32 y = q_v0 + j * q_scale_y;
            if (u8 alpha_byte = SampleTexture(src, x, y)) {
                // rudimentary alpha-blending
                u32 idx = (u32) (j_dest * stride_dest + i_dest);
                Color color_background = dest.img[idx];

                f32 alpha = (1.0f * alpha_byte) / 255;
                Color color_blended;
                color_blended.r = (u8) (floor( alpha*q_color.r ) + floor( (1-alpha)*color_background.r ));
                color_blended.g = (u8) (floor( alpha*q_color.g ) + floor( (1-alpha)*color_background.g ));
                color_blended.b = (u8) (floor( alpha*q_color.b ) + floor( (1-alpha)*color_background.b ));
                color_blended.a = 255;

                dest.img[idx] = color_blended;
            }
        }
    }
}

void BlitGlyph2(s32 w, s32 h, f32 x0, f32 y0, f32 u0, f32 u1, f32 v0, f32 v1, Color color, ImageB src, ImageRGBA dest) {

    f32 scale_x = (u1 - u0) / w;
    f32 scale_y = (v1 - v0) / h;

    // i,j          : target coords
    // i_dest, j_dest : dest coords

    s32 stride_dest = dest.width;

    for (s32 j = 0; j < h; ++j) {
        s32 j_dest = j + y0;
        if (j_dest < 0 || j_dest > dest.height) {
            continue;
        }

        for (s32 i = 0; i < w; ++i) {
            s32 i_dest = x0 + i;
            if (i_dest < 0 || i_dest > dest.width) {
                continue;
            }
            f32 x = u0 + i * scale_x;
            f32 y = v0 + j * scale_y;
            if (u8 alpha_byte = SampleTexture(&src, x, y)) {
                // rudimentary alpha-blending
                u32 idx = (u32) (j_dest * stride_dest + i_dest);
                Color color_background = dest.img[idx];

                f32 alpha = (1.0f * alpha_byte) / 255;
                Color color_blended;
                color_blended.r = (u8) (floor( alpha*color.r ) + floor( (1-alpha)*color_background.r ));
                color_blended.g = (u8) (floor( alpha*color.g ) + floor( (1-alpha)*color_background.g ));
                color_blended.b = (u8) (floor( alpha*color.b ) + floor( (1-alpha)*color_background.b ));
                color_blended.a = 255;

                dest.img[idx] = color_blended;
            }
        }
    }
}


//
//  Sprite render / control buffer API


Array<Frame> g_sprite_buffer;


void SpriteBufferInit(MArena *a_dest, u32 max_quads = 2048) {
    g_sprite_buffer = InitArray<Frame>(a_dest, max_quads);
}

void SpriteBufferPush(Frame sprite) {
    g_sprite_buffer.Add(sprite);
}

void SpriteBufferBlitAndClear(HashMap map_textures, s32 dest_width, s32 dest_height, u8 *dest_buffer) {


    // TODO: do code compression / cleanup here


    for (s32 i = 0; i < g_sprite_buffer.len; ++i) {
        Frame s = g_sprite_buffer.arr[i];

        Texture *texture = (Texture*) MapGet(&map_textures, s.tex_id);

        if (texture == NULL) {
            ImageRGBA _dest = { dest_width, dest_height, (Color*) dest_buffer };

            BlitMonoColor(s.w, s.h, s.x0, s.y0, s.color, _dest);
        }

        else if (texture && texture->tpe == TT_8BIT ) {
            ImageRGBA _dest = { dest_width, dest_height, (Color*) dest_buffer };

            ImageB _tex = {};
            _tex.img = texture->data;
            _tex.width = texture->width;
            _tex.height = texture->height;

            f32 scale_x = (s.u1 - s.u0) / s.w;
            f32 scale_y = (s.v1 - s.v0) / s.h;
            BlitGlyph(s.w, s.h, s.x0, s.y0, s.u0, s.v0, scale_x, scale_y, s.color, &_tex, _dest);
        }

        else if (texture && texture->tpe == TT_RGBA) {
            Texture _dest_tex = { TT_RGBA, dest_width, dest_height, 4, dest_buffer };

            BlitSprite(s.w, s.h, s.x0, s.y0, s.u0, s.u1, s.v0, s.v1, &_dest_tex, texture);
        }

        else {
            printf("WARN: Attempt to blit unknown texture type\n");
        }
    }

    g_sprite_buffer.len = 0;
}


#endif

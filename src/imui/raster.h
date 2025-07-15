#ifndef __RASTER_H__
#define __RASTER_H__


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


#endif

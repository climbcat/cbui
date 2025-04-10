#ifndef __WIREFRAME_H__
#define __WIREFRAME_H__


#include "../geometry/geometry.h"
#include "../geometry/gtypes.h"


#define COLOR_RED (( Color { 255, 0, 0, 255 } ))
#define COLOR_GREEN (( Color { 0, 255, 0, 255 } ))
#define COLOR_YELLOW (( Color { 255, 0, 255, 255 } ))
#define COLOR_BLUE (( Color {  0, 0, 255, 255 } ))
#define COLOR_BLACK (( Color { 0, 0, 0, 255 } ))
#define COLOR_WHITE (( Color { 255, 255, 255, 255 } ))
#define COLOR_GRAY (( Color { 128, 128, 128, 255 } ))


enum WireFrameType {
    // do not collide
    WFT_AXIS,
    WFT_PLANE,

    // do collide
    WFT_EYE,
    WFT_BOX,
    WFT_CYLINDER,
    WFT_SPHERE,

    WFT_COUNT,
};

enum WireFrameRenderStyle {
    WFR_SLIM,
    WFR_FAT,
};

struct Wireframe {
    Matrix4f transform;
    Vector3f dimensions;
    WireFrameType type;
    WireFrameRenderStyle style;
    Color color;
    u32 nsegments;
};


Wireframe CreatePlane(f32 size) {
    Wireframe box = {};
    box.transform = Matrix4f_Identity();
    box.type = WFT_PLANE;
    box.dimensions = { 0.5f*size, 5.0f /* xlines cnt */, 0.5f*size };
    box.color = COLOR_GRAY;

    return box;
}

Wireframe CreateCylinder(f32 r, f32 h) {
    Wireframe box = {};
    box.transform = Matrix4f_Identity();
    box.type = WFT_CYLINDER;
    box.dimensions = { r, h, r };
    box.color = COLOR_GREEN;

    return box;
}

Wireframe CreateSphere(f32 r) {
    Wireframe box = {};
    box.transform = Matrix4f_Identity();
    box.type = WFT_CYLINDER;
    box.dimensions = { r, r, r };
    box.color = COLOR_YELLOW;

    return box;
}

Wireframe CreateEye(f32 width, f32 depth) {
    Wireframe box = {};
    box.transform = Matrix4f_Identity();
    box.type = WFT_CYLINDER;
    box.dimensions = { width, width, depth };
    box.color = COLOR_YELLOW;

    return box;
}

Wireframe CreateAABox(f32 w, f32 h, f32 d) {
    Wireframe box = {};
    box.transform = Matrix4f_Identity();
    box.type = WFT_BOX;
    box.dimensions = { 0.5f*w, 0.5f*h, 0.5f*d };
    box.color = COLOR_BLUE;

    return box;
}

Wireframe CreateAAAxes(f32 len = 1.0f) {
    Wireframe axis = {};
    axis.transform = Matrix4f_Identity();
    axis.type = WFT_AXIS;
    axis.dimensions = { len, len, len };
    axis.color = COLOR_BLACK;

    return axis;
}

bool WireFrameCollide(Ray global, Wireframe wf, Vector3f *hit_in = NULL, Vector3f *hit_out = NULL) {

    if (wf.type == WFT_BOX) {
        // TODO: slab method; handle axis-aligned rays

        Ray local = TransformInverseRay(wf.transform, global);

        Vector3f o = local.position;
        Vector3f d = local.direction;

        f32 tl_x = (-wf.dimensions.x - o.x) / d.x;
        f32 th_x = (wf.dimensions.x - o.x) / d.x;
        f32 tl_y = (- wf.dimensions.y - o.y) / d.y;
        f32 th_y = (wf.dimensions.y - o.y) / d.y;
        f32 tl_z = (- wf.dimensions.z - o.z) / d.z;
        f32 th_z = (wf.dimensions.z - o.z) / d.z;

        f32 t_cls_x = MinF32(tl_x, th_x);
        f32 t_far_x = MaxF32(tl_x, th_x);
        f32 t_cls_y = MinF32(tl_y, th_y);
        f32 t_far_y = MaxF32(tl_y, th_y);
        f32 t_cls_z = MinF32(tl_z, th_z);
        f32 t_far_z = MaxF32(tl_z, th_z);

        f32 t_cls = MaxF32(MaxF32(t_cls_x, t_cls_y), t_cls_z);
        f32 t_far = MinF32(MinF32(t_far_x, t_far_y), t_far_z);

        bool intersect = t_cls <= t_far;
        if (intersect && hit_in) {
            *hit_in = TransformPoint(wf.transform, local.position + t_cls * local.direction);
        }
        if (intersect && hit_out) {
            *hit_out = TransformPoint(wf.transform, local.position + t_far * local.direction);
        }

        return intersect;
    }
    else if (wf.type == WFT_CYLINDER) {
        // TODO: impl.

        return false;
    }
    else if (wf.type == WFT_EYE) {
        // TODO: impl.

        return false;
    }
    else if (wf.type == WFT_SPHERE) {
        // TODO: impl.

        return false;
    }
    else {
        return false;
    }
}

Array<Vector3f> WireframeLineSegments(MArena *a_dest, Array<Wireframe> wf_lst, Matrix4f vp) {

    // anchors
    List<Vector3f> segment_anchs = {};
    segment_anchs.lst = (Vector3f*) ArenaOpen(a_dest);

    for (u32 j = 0; j < wf_lst.len; ++j) {

        Wireframe *wf = wf_lst.arr + j;
        Matrix4f mvp = vp * wf->transform;
        Vector3f sz = wf->dimensions;
        u32 points_per_line_segment = 2;

        if (wf->type == WFT_AXIS) {

            Vector3f origo = TransformPerspective(mvp, {0.0f, 0.0f, 0.0f} );
            Vector3f x = TransformPerspective(mvp, {sz.x, 0.0f, 0.0f} );
            Vector3f y = TransformPerspective(mvp, {0.0f, sz.y, 0.0f} );
            Vector3f z = TransformPerspective(mvp, {0.0f, 0.0f, sz.z} );

            segment_anchs.Add(origo);
            segment_anchs.Add(x);
            segment_anchs.Add(origo);
            segment_anchs.Add(y);
            segment_anchs.Add(origo);
            segment_anchs.Add(z);

            wf->nsegments = 3;
        }

        else if (wf->type == WFT_PLANE) {

            // local coordinates is the x-z plane at y == 0 with nbeams internal cross-lines x and z
            f32 rx = wf->dimensions.x;
            f32 rz = wf->dimensions.z;
            s32 nbeams = floor(wf->dimensions.y);

            Vector3f urc = { rx, 0, rz };
            Vector3f ulc = { -rx, 0, rz };
            Vector3f lrc = { rx, 0, -rz };
            Vector3f llc = { -rx, 0, -rz };
            Vector3f urc_ndc = TransformPerspective(mvp, urc);
            Vector3f ulc_ndc = TransformPerspective(mvp, ulc);
            Vector3f lrc_ndc = TransformPerspective(mvp, lrc);
            Vector3f llc_ndc = TransformPerspective(mvp, llc);

            // the outer square
            segment_anchs.Add(urc_ndc);
            segment_anchs.Add(ulc_ndc);
            segment_anchs.Add(ulc_ndc);
            segment_anchs.Add(llc_ndc);
            segment_anchs.Add(llc_ndc);
            segment_anchs.Add(lrc_ndc);
            segment_anchs.Add(lrc_ndc);
            segment_anchs.Add(urc_ndc);

            // insider beams
            Vector3f xhat = 1.0f / (nbeams + 1) * (urc - ulc);
            Vector3f zhat = 1.0f / (nbeams + 1) * (llc - ulc);

            for (u32 i = 0; i < nbeams + 1; ++i) {
                Vector3f v1_ndc = TransformPerspective(mvp, ulc + i* xhat);
                Vector3f v2_ndc = TransformPerspective(mvp, llc + i* xhat);

                Vector3f h1_ndc = TransformPerspective(mvp, ulc + i* zhat);
                Vector3f h2_ndc = TransformPerspective(mvp, urc + i* zhat);

                segment_anchs.Add(v1_ndc);
                segment_anchs.Add(v2_ndc);
                segment_anchs.Add(h1_ndc);
                segment_anchs.Add(h2_ndc);
            }

            wf->nsegments = 4 + 2*(nbeams + 1);
        }

        else if (wf->type == WFT_BOX) {

            Vector3f ppp = TransformPerspective(mvp, { sz.x, sz.y, sz.z } );
            Vector3f ppm = TransformPerspective(mvp, { sz.x, sz.y, -sz.z } );
            Vector3f pmp = TransformPerspective(mvp, { sz.x, -sz.y, sz.z } );
            Vector3f pmm = TransformPerspective(mvp, { sz.x, -sz.y, -sz.z } );
            Vector3f mpp = TransformPerspective(mvp, { -sz.x, sz.y, sz.z } );
            Vector3f mpm = TransformPerspective(mvp, { -sz.x, sz.y, -sz.z } );
            Vector3f mmp = TransformPerspective(mvp, { -sz.x, -sz.y, sz.z } );
            Vector3f mmm = TransformPerspective(mvp, { -sz.x, -sz.y, -sz.z } );

            segment_anchs.Add(ppp);
            segment_anchs.Add(ppm);
            segment_anchs.Add(ppm);
            segment_anchs.Add(pmm);
            segment_anchs.Add(pmm);
            segment_anchs.Add(pmp);
            segment_anchs.Add(pmp);
            segment_anchs.Add(ppp);

            segment_anchs.Add(mpp);
            segment_anchs.Add(mpm);
            segment_anchs.Add(mpm);
            segment_anchs.Add(mmm);
            segment_anchs.Add(mmm);
            segment_anchs.Add(mmp);
            segment_anchs.Add(mmp);
            segment_anchs.Add(mpp);

            segment_anchs.Add(ppp);
            segment_anchs.Add(mpp);
            segment_anchs.Add(ppm);
            segment_anchs.Add(mpm);
            segment_anchs.Add(pmm);
            segment_anchs.Add(mmm);
            segment_anchs.Add(pmp);
            segment_anchs.Add(mmp);

            wf->nsegments = 12;
        }

        else if (wf->type == WFT_SPHERE) {

            // TODO: impl.
        }

        else if (wf->type == WFT_CYLINDER) {

            f32 r = sz.x;
            f32 h2 = sz.y / 2;

            s32 nbars = 8;

            Vector3f up_prev = {};
            Vector3f lw_prev = {};
            Vector3f up_first = {};
            Vector3f lw_first = {};
            for (u32 i = 0; i < nbars; ++i) {
                f32 theta = 2 * 3.14159265f / 8 * i;
                Vector3f up = TransformPerspective(mvp, { r * cos(theta), h2, r * sin(theta) });
                Vector3f lw = TransformPerspective(mvp, { r * cos(theta), - h2, r * sin(theta) });

                segment_anchs.Add(up);
                segment_anchs.Add(lw);

                if (i == 0) {
                    up_first = up;
                    lw_first = lw;
                }
                else if (i > 0) {
                    segment_anchs.Add(up_prev);
                    segment_anchs.Add(up);
                    segment_anchs.Add(lw_prev);
                    segment_anchs.Add(lw);
                }
                if (i == (nbars - 1)) {
                    segment_anchs.Add(up);
                    segment_anchs.Add(up_first);
                    segment_anchs.Add(lw);
                    segment_anchs.Add(lw_first);
                }
                up_prev = up;
                lw_prev = lw;
            }
            wf->nsegments = 24;
        }

        else if (wf->type == WFT_EYE) {

            /*
            float radius_xy = campos->dims.x;
            float length_z = campos->dims.z;

            campos->verts_low = vertex_buffer->len;
            campos->lines_low = index_buffer->len;

            u16 vertex_offset = vertex_buffer->len;
            *(index_buffer->lst + index_buffer->len++) = Vector2_u16 { (u16) (vertex_offset + 0), (u16) (vertex_offset + 1) };
            *(index_buffer->lst + index_buffer->len++) = Vector2_u16 { (u16) (vertex_offset + 0), (u16) (vertex_offset + 2) };
            *(index_buffer->lst + index_buffer->len++) = Vector2_u16 { (u16) (vertex_offset + 0), (u16) (vertex_offset + 3) };
            *(index_buffer->lst + index_buffer->len++) = Vector2_u16 { (u16) (vertex_offset + 0), (u16) (vertex_offset + 4) };
            *(index_buffer->lst + index_buffer->len++) = Vector2_u16 { (u16) (vertex_offset + 1), (u16) (vertex_offset + 2) };
            *(index_buffer->lst + index_buffer->len++) = Vector2_u16 { (u16) (vertex_offset + 2), (u16) (vertex_offset + 4) };
            *(index_buffer->lst + index_buffer->len++) = Vector2_u16 { (u16) (vertex_offset + 4), (u16) (vertex_offset + 3) };
            *(index_buffer->lst + index_buffer->len++) = Vector2_u16 { (u16) (vertex_offset + 3), (u16) (vertex_offset + 1) };

            *(vertex_buffer->lst + vertex_buffer->len++) = campos->origo;
            *(vertex_buffer->lst + vertex_buffer->len++) = Vector3f { campos->origo.x - radius_xy, campos->origo.y - radius_xy, campos->origo.z + length_z };
            *(vertex_buffer->lst + vertex_buffer->len++) = Vector3f { campos->origo.x - radius_xy, campos->origo.y + radius_xy, campos->origo.z + length_z };
            *(vertex_buffer->lst + vertex_buffer->len++) = Vector3f { campos->origo.x + radius_xy, campos->origo.y - radius_xy, campos->origo.z + length_z };
            *(vertex_buffer->lst + vertex_buffer->len++) = Vector3f { campos->origo.x + radius_xy, campos->origo.y + radius_xy, campos->origo.z + length_z };
            */
        }

        else {
            printf("WARN: Unknown wireframe type\n");
        }
    }

    ArenaClose(a_dest, 3 * sizeof(Vector3f) * segment_anchs.len);
    Array<Vector3f> result = {};
    result.max = segment_anchs.len;
    result.len = result.max;
    result.arr = segment_anchs.lst;

    return result;
}



#endif

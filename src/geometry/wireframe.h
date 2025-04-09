#ifndef __WIREFRAME_H__
#define __WIREFRAME_H__


#include "../geometry/geometry.h"
#include "../geometry/gtypes.h"


#define COLOR_RED (( Color { 255, 0, 0, 255 } ))
#define COLOR_GREEN (( Color { 0, 255, 0, 255 } ))
#define COLOR_BLUE (( Color {  0, 0, 255, 255 } ))
#define COLOR_BLACK (( Color { 0, 0, 0, 255 } ))
#define COLOR_WHITE (( Color { 255, 255, 255, 255 } ))
#define COLOR_GRAY (( Color { 128, 128, 128, 255 } ))


enum WireFrameType {
    WFT_AXIS,
    WFT_BOX,
    WFT_EYE,
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
            *hit_in = TransformInversePoint(wf.transform, local.position + t_cls * local.direction);
        }
        if (intersect && hit_out) {
            *hit_out = TransformInversePoint(wf.transform, local.position + t_far * local.direction);
        }

        return intersect;
    }
    else
    {
        return false;
    }
}


Array<Vector3f> WireframeLineSegments(MArena *a_dest, Array<Wireframe> wf_lst, Matrix4f vp) {
    List<Vector3f> segments = {};
    segments.lst = (Vector3f*) ArenaOpen(a_dest);

    for (u32 j = 0; j < wf_lst.len; ++j) {

        Wireframe *wf = wf_lst.arr + j;
        Matrix4f mvp = vp * wf->transform;
        Vector3f sz = wf->dimensions;
        u32 points_per_line_segment = 2;

        if (wf->type == WFT_AXIS) {
            wf->nsegments = 3;

            Vector3f origo = TransformPerspective(mvp, {0.0f, 0.0f, 0.0f} );
            Vector3f x = TransformPerspective(mvp, {sz.x, 0.0f, 0.0f} );
            Vector3f y = TransformPerspective(mvp, {0.0f, sz.y, 0.0f} );
            Vector3f z = TransformPerspective(mvp, {0.0f, 0.0f, sz.z} );

            segments.Add(origo);
            segments.Add(x);
            segments.Add(origo);
            segments.Add(y);
            segments.Add(origo);
            segments.Add(z);
        }

        else if (wf->type == WFT_BOX) {
            wf->nsegments = 12;

            Vector3f ppp = TransformPerspective(mvp, { sz.x, sz.y, sz.z } );
            Vector3f ppm = TransformPerspective(mvp, { sz.x, sz.y, -sz.z } );
            Vector3f pmp = TransformPerspective(mvp, { sz.x, -sz.y, sz.z } );
            Vector3f pmm = TransformPerspective(mvp, { sz.x, -sz.y, -sz.z } );
            Vector3f mpp = TransformPerspective(mvp, { -sz.x, sz.y, sz.z } );
            Vector3f mpm = TransformPerspective(mvp, { -sz.x, sz.y, -sz.z } );
            Vector3f mmp = TransformPerspective(mvp, { -sz.x, -sz.y, sz.z } );
            Vector3f mmm = TransformPerspective(mvp, { -sz.x, -sz.y, -sz.z } );

            segments.Add(ppp);
            segments.Add(ppm);
            segments.Add(ppm);
            segments.Add(pmm);
            segments.Add(pmm);
            segments.Add(pmp);
            segments.Add(pmp);
            segments.Add(ppp);

            segments.Add(mpp);
            segments.Add(mpm);
            segments.Add(mpm);
            segments.Add(mmm);
            segments.Add(mmm);
            segments.Add(mmp);
            segments.Add(mmp);
            segments.Add(mpp);

            segments.Add(ppp);
            segments.Add(mpp);
            segments.Add(ppm);
            segments.Add(mpm);
            segments.Add(pmm);
            segments.Add(mmm);
            segments.Add(pmp);
            segments.Add(mmp);
        }

        else {
            printf("WARN: Unknown wireframe type\n");
        }
    }

    ArenaClose(a_dest, 3 * sizeof(Vector3f) * segments.len);
    Array<Vector3f> result = {};
    result.max = segments.len;
    result.len = result.max;
    result.arr = segments.lst;

    return result;
}



#endif

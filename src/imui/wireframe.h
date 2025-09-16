#ifndef __WIREFRAME_H__
#define __WIREFRAME_H__


#include "math.h"


enum WireFrameType {
    // do not collide
    WFT_AXIS,
    WFT_PLANE,

    // do collide
    WFT_EYE,
    WFT_BOX,
    WFT_CYLINDER,
    WFT_SPHERE,

    WFT_SEGMENTS,

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
    Array<Vector3f> segments;
    bool disabled;
};



Wireframe CreatePlaneDetailed(f32 size_x, f32 size_z, s32 nbeams_x) {
    Wireframe box = {};

    box.transform = Matrix4f_Identity();
    box.type = WFT_PLANE;
    box.dimensions = { size_x, (f32) nbeams_x, size_z };
    box.color = COLOR_GRAY;

    return box;
}

Wireframe CreatePlane(f32 size) {
    /*
    s32 size_x = 10;
    s32 size_z = 15;
    s32 nbeams_x = 11;

    return CreatePlaneDetailed(size_x, size_z, nbeams_x);
    */

    return CreatePlaneDetailed(size, size, 6);
}

Wireframe CreateCylinder(f32 radius, f32 height) {
    Wireframe box = {};
    box.transform = Matrix4f_Identity();
    box.type = WFT_CYLINDER;
    f32 diameter = 2*radius;
    box.dimensions = { radius, 0.5f*height, radius };
    box.color = COLOR_GREEN;

    return box;
}

Wireframe CreateSphere(f32 radius) {
    Wireframe box = {};
    box.transform = Matrix4f_Identity();
    box.type = WFT_SPHERE;
    box.dimensions = { radius, radius, radius };
    box.color = COLOR_BLUE;

    return box;
}

Wireframe CreateEye(f32 width, f32 depth) {
    Wireframe box = {};
    box.transform = Matrix4f_Identity();
    box.type = WFT_EYE;
    box.dimensions = { 0.5f*width, 0.5f*width, depth };
    box.color = COLOR_BLACK;

    return box;
}

Wireframe CreateAABox(f32 width, f32 height, f32 depth) {
    Wireframe box = {};
    box.transform = Matrix4f_Identity();
    box.type = WFT_BOX;
    box.dimensions = { 0.5f*width, 0.5f*height, 0.5f*depth };
    box.color = COLOR_BLUE;

    return box;
}

Wireframe CreateAAAxes(f32 len = 1.0f) {
    Wireframe axis = {};
    axis.transform = Matrix4f_Identity();
    axis.type = WFT_AXIS;
    axis.dimensions = { len, len, len };
    axis.color = COLOR_GRAY;

    return axis;
}


inline
bool FZero(f32 f) {
    bool result = abs(f) < 0.0001f;
    return result;
}
inline
bool FRange(f32 val, f32 min, f32 max) {
    bool result = val >= min && val <= max;
    return result;
}
bool BoxCollideSLAB(Ray global, Wireframe wf, Vector3f *hit_in = NULL, Vector3f *hit_out = NULL) {
    TimeFunction;

    Ray loc = TransformInverseRay(wf.transform, global);
    Vector3f p = loc.pos;
    Vector3f d = loc.dir;
    Vector3f dims = wf.dimensions;

    bool intersect;
    f32 t_close;
    f32 t_far;

    bool zero_x = FZero(d.x);
    bool zero_y = FZero(d.y);
    bool zero_z = FZero(d.z);

    f32 t_low_x = (-dims.x - p.x) / d.x;
    f32 t_high_x = (dims.x - p.x) / d.x;
    f32 t_low_y = (- dims.y - p.y) / d.y;
    f32 t_high_y = (dims.y - p.y) / d.y;
    f32 t_low_z = (- dims.z - p.z) / d.z;
    f32 t_high_z = (dims.z - p.z) / d.z;

    f32 t_close_x = MinF32(t_low_x, t_high_x);
    f32 t_far_x = MaxF32(t_low_x, t_high_x);
    f32 t_close_y = MinF32(t_low_y, t_high_y);
    f32 t_far_y = MaxF32(t_low_y, t_high_y);
    f32 t_close_z = MinF32(t_low_z, t_high_z);
    f32 t_far_z = MaxF32(t_low_z, t_high_z);

    t_close = MaxF32(MaxF32(t_close_x, t_close_y), t_close_z);
    t_far = MinF32(MinF32(t_far_x, t_far_y), t_far_z);
    intersect = t_close <= t_far;

    if (intersect && hit_in) { *hit_in = TransformPoint(wf.transform, loc.pos + t_close * loc.dir); }
    if (intersect && hit_out) { *hit_out = TransformPoint(wf.transform, loc.pos + t_far * loc.dir); }

    return intersect;
}

bool WireFrameCollide(Ray global, Wireframe wf, Vector3f *hit_in = NULL, Vector3f *hit_out = NULL) {
    if (wf.disabled) {
        return false;
    }

    Ray loc = TransformInverseRay(wf.transform, global);
    Vector3f sz = wf.dimensions;

    if (wf.type == WFT_BOX) {

        return BoxCollideSLAB(global, wf, hit_in, hit_out);
    }

    else if (wf.type == WFT_CYLINDER) {
        // TODO: improve cylinder intersection to be exact

        bool boxed = BoxCollideSLAB(global, wf, hit_in, hit_out);
        bool cylhit = true;
        if (hit_in && hit_out) {
            f32 dist;
            LineToLineDist(global.pos, global.dir, {0, 1, 0}, {}, &dist);
            f32 radius = sz.x;
            cylhit = dist < radius;
        }

        return boxed && cylhit;
    }

    else if (wf.type == WFT_EYE) {
        // TODO: impl. triangle-based scheme

        return BoxCollideSLAB(global, wf, hit_in, hit_out);
    }

    else if (wf.type == WFT_SPHERE) {

        Vector3f center = {};
        Vector3f closest = PointToLine(center, loc.pos, loc.dir);
        f32 dist = (center - closest).Norm();
        f32 radius = wf.dimensions.x;
        if (dist <= radius) {

            //  Consider the triangle [center, closest, hit_in], then:
            //  dist^2 + surf_2^2 == radius^2

            f32 surf_2 = sqrt(radius*radius - dist*dist);
            if (hit_in) {
                Vector3f hit_in_loc = closest - surf_2 * loc.dir;
                *hit_in = TransformPoint(wf.transform, hit_in_loc);
            }
            if (hit_out) {
                Vector3f hit_out_loc = closest + surf_2 * loc.dir;
                *hit_out = TransformPoint(wf.transform, hit_out_loc);
            }

            return true;
        }
        else {
            return false;
        }
    }

    else {
        return false;
    }
}

Array<Vector3f> WireframeRawSegments(MArena *a_dest, Wireframe *wf) {
    TimeFunction;

    Array<Vector3f> anchors = {};
    Vector3f sz = wf->dimensions;

    if (wf->type == WFT_SEGMENTS) {

        // don't touch the wireframe
    }

    else if (wf->type == WFT_AXIS) {

        anchors = InitArray<Vector3f>(a_dest, 6);

        Vector3f origo = {0.0f, 0.0f, 0.0f};
        Vector3f x = {sz.x, 0.0f, 0.0f};
        Vector3f y = {0.0f, sz.y, 0.0f};
        Vector3f z = {0.0f, 0.0f, sz.z};

        anchors.Add(origo);
        anchors.Add(x);
        anchors.Add(origo);
        anchors.Add(y);
        anchors.Add(origo);
        anchors.Add(z);

        wf->segments = anchors;
    }

    else if (wf->type == WFT_PLANE) {

        // local coordinates is the x-z plane at y == 0 with nbeams internal cross-lines x and z
        f32 rx = 0.5f * sz.x;
        s32 nbeams_x = sz.y;
        f32 cell_sz = sz.x / (nbeams_x - 1);
        s32 nbeams_z = floor(sz.z / cell_sz + 1);
        f32 rz = 0.5f * (nbeams_z - 1) * cell_sz;

        Vector3f ulc = { -rx, 0, rz };
        Vector3f lrc = { rx, 0, -rz };
        Vector3f llc = { -rx, 0, -rz };

        // insider beams
        Vector3f xhat = { cell_sz, 0, 0 };
        Vector3f zhat = { 0, 0, cell_sz };

        s32 nanchors = (nbeams_x + nbeams_z) * 2; // num_beams * num_dimensions * anchs_per_point
        anchors = InitArray<Vector3f>(a_dest, nanchors);

        for (s32 i = 0; i < nbeams_x; ++i) {
            anchors.Add(ulc + i* xhat);
            anchors.Add(llc + i* xhat);
        }
        for (s32 i = 0; i < nbeams_z; ++i) {
            anchors.Add(llc + i * zhat);
            anchors.Add(lrc + i * zhat);
        }
        assert(anchors.len == nanchors);

        wf->segments = anchors;
    }

    else if (wf->type == WFT_BOX) {

        anchors = InitArray<Vector3f>(a_dest, 24);

        Vector3f ppp = { sz.x, sz.y, sz.z };
        Vector3f ppm = { sz.x, sz.y, -sz.z };
        Vector3f pmp = { sz.x, -sz.y, sz.z };
        Vector3f pmm = { sz.x, -sz.y, -sz.z };
        Vector3f mpp = { -sz.x, sz.y, sz.z };
        Vector3f mpm = { -sz.x, sz.y, -sz.z };
        Vector3f mmp = { -sz.x, -sz.y, sz.z };
        Vector3f mmm = { -sz.x, -sz.y, -sz.z };

        anchors.Add(ppp);
        anchors.Add(ppm);
        anchors.Add(ppm);
        anchors.Add(pmm);
        anchors.Add(pmm);
        anchors.Add(pmp);
        anchors.Add(pmp);
        anchors.Add(ppp);

        anchors.Add(mpp);
        anchors.Add(mpm);
        anchors.Add(mpm);
        anchors.Add(mmm);
        anchors.Add(mmm);
        anchors.Add(mmp);
        anchors.Add(mmp);
        anchors.Add(mpp);

        anchors.Add(ppp);
        anchors.Add(mpp);
        anchors.Add(ppm);
        anchors.Add(mpm);
        anchors.Add(pmm);
        anchors.Add(mmm);
        anchors.Add(pmp);
        anchors.Add(mmp);

        wf->segments = anchors;
    }

    else if (wf->type == WFT_SPHERE) {

        u32 len_prev = anchors.len;
        f32 r = sz.x;

        u32 nlatt = 6;
        u32 nlong = 6;

        u32 cnt = (nlatt * 2 + nlatt * nlong * 2) + (nlong * 2 + nlong * nlatt * 2);
        anchors = InitArray<Vector3f>(a_dest, cnt);

        Vector3f center = {};
        Vector3f north = { 0, r, 0 };
        Vector3f south = { 0, -r, 0 };

        for (u32 i = 0; i < nlatt; ++i) {
            f32 theta = PI / nlatt * i;

            Vector3f pt_0 = SphericalCoordsY(theta, 0, r);
            anchors.Add(pt_0);
            for (u32 j = 0; j < nlong; ++j) {
                f32 phi = 2 * PI / nlong * (j % nlong);

                Vector3f pt = SphericalCoordsY(theta, phi, r);
                anchors.Add(pt);
                anchors.Add(pt);
            } 
            anchors.Add(pt_0);
        }

        for (u32 j = 0; j < nlong; ++j) {
            f32 phi = 2 * PI / nlong * j;

            anchors.Add(north);
            for (u32 i = 0; i < nlatt; ++i) {
                f32 theta = PI / nlatt * i;

                Vector3f pt = SphericalCoordsY(theta, phi, r);
                anchors.Add(pt);
                anchors.Add(pt);

            }
            anchors.Add(south);
        }

        wf->segments = anchors;
    }

    else if (wf->type == WFT_CYLINDER) {

        f32 r = sz.x;
        f32 h2 = sz.y;

        s32 nbars = 8;
        u32 cnt = nbars * 2 + nbars * 4;
        anchors = InitArray<Vector3f>(a_dest, cnt);

        Vector3f up_prev = {};
        Vector3f lw_prev = {};
        Vector3f up_first = {};
        Vector3f lw_first = {};
        for (u32 i = 0; i < nbars; ++i) {
            f32 theta = 2 * 3.14159265f / 8 * i;
            Vector3f up = { r * cos(theta), h2, r * sin(theta) };
            Vector3f lw = { r * cos(theta), - h2, r * sin(theta) };

            anchors.Add(up);
            anchors.Add(lw);

            if (i == 0) {
                up_first = up;
                lw_first = lw;
            }
            else if (i > 0) {
                anchors.Add(up_prev);
                anchors.Add(up);
                anchors.Add(lw_prev);
                anchors.Add(lw);
            }
            if (i == (nbars - 1)) {
                anchors.Add(up);
                anchors.Add(up_first);
                anchors.Add(lw);
                anchors.Add(lw_first);
            }
            up_prev = up;
            lw_prev = lw;
        }

        wf->segments = anchors;
    }

    else if (wf->type == WFT_EYE) {

        anchors = InitArray<Vector3f>(a_dest, 16);

        f32 w2 = sz.x;
        f32 d = sz.z;

        Vector3f urc = { w2, w2, d };
        Vector3f ulc = { - w2, w2, d };
        Vector3f lrc = { w2, - w2, d };
        Vector3f llc = { - w2, - w2, d };
        Vector3f point = {};

        anchors.Add(urc);
        anchors.Add(ulc);
        anchors.Add(ulc);
        anchors.Add(llc);
        anchors.Add(llc);
        anchors.Add(lrc);
        anchors.Add(lrc);
        anchors.Add(urc);

        anchors.Add(urc);
        anchors.Add(point);
        anchors.Add(ulc);
        anchors.Add(point);
        anchors.Add(llc);
        anchors.Add(point);
        anchors.Add(lrc);
        anchors.Add(point);

        wf->segments = anchors;
    }

    else {
        printf("WARN: Unknown wireframe type\n");
    }

    return anchors;
}


Array<Vector3f> WireframeLineSegments(MArena *a_dest, Array<Wireframe> wireframes) {
    Array<Vector3f> anchors_all = InitArray<Vector3f>(a_dest, 0);

    for (u32 i = 0; i < wireframes.len; ++i) {
        Array<Vector3f> anchors;
        anchors = WireframeRawSegments(a_dest, wireframes.arr + i);
        anchors_all.len += anchors.len;

        // points delivered in local coords
    }

    anchors_all.max = anchors_all.len;
    return anchors_all;
}


inline
s32 _GetNextNonDisabledWireframeIndex(u32 idx_prev, Array<Wireframe> wireframes) {
    idx_prev++;
    if (wireframes.len <= idx_prev) {
        return -1;
    }

    while ((wireframes.arr + idx_prev)->disabled == true) {
        idx_prev++;

        if (wireframes.len <= idx_prev) {
            return -1;
        }
    }
    return idx_prev;
}


void RenderWireframe(u8 *image_buffer, Matrix4f view, Perspective persp, u32 w, u32 h, Wireframe wf) {
    Ray view_plane = { Vector3f { 0, 0, 0.1 }, Vector3f { 0, 0, 1 } };
    Matrix4f view_inv = TransformGetInverse(view);
    Matrix4f l2v = view_inv * wf.transform;

    for (u32 i = 0; i < wf.segments.len / 2; ++i) {

        if (true) {
            Vector3f a = wf.segments.arr[2*i];
            Vector3f b = wf.segments.arr[2*i + 1];

            RenderLineSegment(image_buffer, l2v, persp, a, b, w, h, wf.color);
        }

        else {
            // line clipping starts
            Vector3f p1_cam = TransformPoint(l2v, wf.segments.arr[2*i]);
            Vector3f p2_cam = TransformPoint(l2v, wf.segments.arr[2*i + 1]);

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
                // line clipping is done

                Vector3f p1_ndc = TransformPerspective(persp.proj, p1_cam);
                Vector3f p2_ndc = TransformPerspective(persp.proj, p2_cam);

                Vector2f a = {};
                a.x = (p1_ndc.x + 1) / 2 * w;
                a.y = (p1_ndc.y + 1) / 2 * h;
                Vector2f b = {};
                b.x = (p2_ndc.x + 1) / 2 * w;
                b.y = (p2_ndc.y + 1) / 2 * h;

                if (wf.style == WFR_SLIM) {
                    RenderLineRGBA(image_buffer, w, h, a.x, a.y, b.x, b.y, wf.color);
                }
                else if (wf.style == WFR_FAT) {
                    RenderLineRGBA(image_buffer, w, h, a.x, a.y, b.x, b.y, wf.color);
                    RenderLineRGBA(image_buffer, w, h, a.x+1, a.y, b.x+1, b.y, wf.color);
                    RenderLineRGBA(image_buffer, w, h, a.x, a.y+1, b.x, b.y+1, wf.color);
                }
            }
        }
    }
}


#endif

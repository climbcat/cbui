#ifndef __WIREFRAME_H__
#define __WIREFRAME_H__


#include "../geometry/geometry.h"
#include "../geometry/gtypes.h"


enum WireFrameType {
    WFT_AXIS,
    WFT_BOX,
    WFT_EYE,
    WFT_CYLINDER,
    WFT_SPHERE,

    WFT_COUNT,
};

struct Wireframe {
    Matrix4f transform;
    Vector3f dimensions;
    WireFrameType type;
    Color color;
};


Array<Vector3f> WireframeLineSegments(MArena *a_dest, Array<Wireframe> wf_lst, Matrix4f vp) {

    List<Vector3f> segments = {};
    segments.lst = (Vector3f*) ArenaOpen(a_dest);

    for (u32 j = 0; j < wf_lst.len; ++j) {

        Wireframe wf = wf_lst.arr[j];
        Matrix4f mvp = vp * wf.transform;
        Vector3f sz = wf.dimensions;
        u32 points_per_line_segment = 2;

        if (wf.type == WFT_AXIS) {

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

        else if (wf.type == WFT_BOX) {

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

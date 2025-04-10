#ifndef __CAMERA_H__
#define __CAMERA_H__


struct OrbitCamera {
    PerspectiveFrustum frustum;
    Vector3f center;
    f32 theta;
    f32 phi;
    f32 radius;
    f32 mouse2rot = 0.4f;
    f32 mouse2pan = 0.01f;
    Matrix4f view;
    Matrix4f proj;
    Matrix4f vp;

    void SetAspect(u32 width, u32 height) {
        f32 aspect_new = width / (f32) height;

        if (aspect_new != frustum.aspect) {
            frustum.aspect = aspect_new;
            proj = PerspectiveMatrixOpenGL(frustum, false, true, false);
        }
    }

    Ray GetRay(f32 x_frac, f32 y_frac) {
        // get the shoot-ray from the camera in world coordinates

        // TODO: we shouldn't need to have a factor -1 here, in front of the x component.
        //      Check the signs of the projection and all this, comparing to the OGL convention

        f32 fov2 = sin(deg2rad * frustum.fov * 0.5f);
        Vector3f dir = {};
        dir.x = - 2.0f * fov2 * x_frac;
        dir.y = - 2.0f * fov2 / frustum.aspect * y_frac;
        dir.z = 1;
        dir.Normalize();

        Ray shoot = {};
        shoot.position = TransformPoint(view, Vector3f_Zero());
        shoot.direction = TransformDirection(view, dir);

        return shoot;
    }

    Ray CameraRay() {
        Ray forward = GetRay(0.0f, 0.0f);
        return forward;
    }

    Vector3f GetPointAtDepth(f32 x_frac, f32 y_frac, Vector3f at_depth) {
        f32 depth_loc = TransformInversePoint(view, at_depth).z;
        Vector3f plane_origo = { 0.0f, 0.0f, depth_loc };
        Vector3f plane_normal = { 0.0f, 0.0f, 1.0f };

        Vector3f its_world = RayPlaneIntersect(GetRay(x_frac, y_frac), TransformPoint(view, plane_origo), TransformPoint(view, plane_normal));
        return its_world;
    }
};

OrbitCamera OrbitCameraInit(float aspect) {
    OrbitCamera cam = {};

    cam.frustum.fov = 90; 
    cam.frustum.aspect = aspect;
    cam.frustum.dist_near = 0.01f;
    cam.frustum.dist_far = 10.0f;

    cam.center = Vector3f_Zero();
    cam.theta = 60;
    cam.phi = 35;
    cam.radius = 4;
    cam.view = Matrix4f_Identity();
    cam.proj = PerspectiveMatrixOpenGL(cam.frustum, false, true, false);

    return cam;
}

inline f32 _PositiveSqrtMultiplier(f32 value) {
    if (value == 0) {
        value = 1;
    }
    else if (value < 0) {
        value = -1 * value;
    }
    return sqrt(value);
}
static f32 _ClampTheta(f32 theta_degs, f32 min = 0.0001f, f32 max = 180 - 0.0001f) {
    f32 clamp_up = MinF32(theta_degs, max);
    f32 result = MaxF32(clamp_up, min);
    return result;
}
void OrbitCameraUpdate(OrbitCamera *cam, f32 dx, f32 dy, bool do_rotate, bool do_pan, f32 scroll_y_offset) {
    f32 sign_x = 1;

    // why
    bool invert_x = true;
    if (invert_x) {
        sign_x = - 1;
    }

    if (do_rotate) {
        // orbit
        cam->theta = _ClampTheta(cam->theta - dy * cam->mouse2rot);
        cam->phi += sign_x * dx * cam->mouse2rot;
    }
    else if (scroll_y_offset < 0) {
        // zoom in
        f32 mult = _PositiveSqrtMultiplier((f32) scroll_y_offset);
        cam->radius *= 1.1f * mult;
    }
    else if (scroll_y_offset > 0) {
        // zoom out
        f32 mult = _PositiveSqrtMultiplier((f32) scroll_y_offset);
        cam->radius /= 1.1f * mult;
    }
    else if (do_pan) {
        // pan
        Vector3f forward = - SphericalCoordsY(cam->theta*deg2rad, cam->phi*deg2rad, cam->radius);
        forward.Normalize();
        Vector3f left = y_hat.Cross(forward);
        left.Normalize();
        Vector3f right = - left;
        Vector3f up = forward.Cross(left);
        up.Normalize();
        cam->center = cam->center + cam->mouse2pan * dx * right;
        cam->center = cam->center + cam->mouse2pan * dy * up;
    }

    // build orbit transform
    cam->view = TransformBuildOrbitCam(cam->center, cam->theta, cam->phi, cam->radius);
    cam->vp = TransformBuildViewProj(cam->view, cam->proj);
}

#endif

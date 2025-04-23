#ifndef __CAMERA_H__
#define __CAMERA_H__



Ray CameraGetRay(Matrix4f view, f32 fov, f32 aspect, f32 x_frac = 0, f32 y_frac = 0) {
    // get the shoot-ray from the camera in world coordinates

    // TODO: we shouldn't need to have a factor -1 here, in front of the x component.
    //      Check the signs of the projection and all this, comparing to the OGL convention

    f32 fov2 = sin(deg2rad * fov * 0.5f);
    Vector3f dir = {};
    dir.x = - 2.0f * fov2 * x_frac;
    dir.y = - 2.0f * fov2 / aspect * y_frac;
    dir.z = 1;
    dir.Normalize();

    Ray shoot = {};
    shoot.pos = TransformPoint(view, Vector3f_Zero());
    shoot.dir = TransformDirection(view, dir);

    return shoot;
}

Vector3f CameraGetPointAtDepth(Matrix4f view, f32 fov, f32 aspect, Vector3f at_depth, f32 x_frac = 0, f32 y_frac = 0) {
    f32 depth_loc = TransformInversePoint(view, at_depth).z;
    Vector3f plane_origo = { 0.0f, 0.0f, depth_loc };
    Vector3f plane_normal = { 0.0f, 0.0f, 1.0f };
    Vector3f world = RayPlaneIntersect(CameraGetRay(view, fov, aspect, x_frac, y_frac), TransformPoint(view, plane_origo), TransformPoint(view, plane_normal));

    return world;
}


struct OrbitCamera {
    Vector3f center;
    Vector3f position;
    Vector3f position_world;
    f32 theta;
    f32 phi;
    f32 radius;
    f32 mouse2rot = 0.4f;
    f32 mouse2pan = 0.01f;
    Matrix4f view;
};

OrbitCamera OrbitCameraInit(f32 aspect) {
    OrbitCamera cam = {};

    cam.center = Vector3f_Zero();
    cam.theta = 60;
    cam.phi = 35;
    cam.radius = 4;
    cam.view = Matrix4f_Identity();

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
    
    cam->view = TransformBuildOrbitCam(cam->center, cam->theta, cam->phi, cam->radius, &cam->position);
    cam->position_world = TransformPoint(cam->view, {});
}

#endif

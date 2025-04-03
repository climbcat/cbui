#ifndef __CAMERA_H__
#define __CAMERA_H__



inline f32 PositiveSqrtMultiplier(f32 value) {
    if (value == 0) {
        value = 1;
    }
    else if (value < 0) {
        value = -1 * value;
    }
    return sqrt(value);
}


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

    static f32 ClampTheta(f32 theta_degs, f32 min = 0.0001f, f32 max = 180 - 0.0001f) {
        f32 clamp_up = MinF32(theta_degs, max);
        f32 result = MaxF32(clamp_up, min);
        return result;
    }

    void Update(f32 dx, f32 dy, bool do_rotate, bool do_pan, f32 scroll_y_offset) {
        f32 sign_x = 1;

        // why
        bool invert_x = true;
        if (invert_x) {
            sign_x = - 1;
        }

        if (do_rotate) {
            // orbit
            theta = OrbitCamera::ClampTheta(theta - dy * mouse2rot);
            phi += sign_x * dx * mouse2rot;
        }
        else if (scroll_y_offset < 0) {
            // zoom in
            f32 mult = PositiveSqrtMultiplier((f32) scroll_y_offset);
            radius *= 1.1f * mult;
        }
        else if (scroll_y_offset > 0) {
            // zoom out
            f32 mult = PositiveSqrtMultiplier((f32) scroll_y_offset);
            radius /= 1.1f * mult;
        }
        else if (do_pan) {
            // pan
            Vector3f forward = - SphericalCoordsY(theta*deg2rad, phi*deg2rad, radius);
            forward.Normalize();
            Vector3f left = y_hat.Cross(forward);
            left.Normalize();
            Vector3f right = - left;
            Vector3f up = forward.Cross(left);
            up.Normalize();
            center = center + mouse2pan * dx * right;
            center = center + mouse2pan * dy * up;
        }

        // build orbit transform
        view = TransformBuildOrbitCam(center, theta, phi, radius);
        vp = TransformBuildViewProj(view, proj);
    }
    /*
    void Update(MouseTrap m) {
        f32 sign_x = 1;

        // why
        bool invert_x = true;
        if (invert_x) {
            sign_x = - 1;
        }

        if (m.l) {
            // orbit
            theta = OrbitCamera::ClampTheta(theta - m.dy * mouse2rot);
            phi += sign_x * m.dx * mouse2rot;
        }
        else if (m.mwheel_y_delta < 0) {
            // zoom in
            f32 mult = PositiveSqrtMultiplier((f32) m.mwheel_y_delta);
            radius *= 1.1f * mult;
        }
        else if (m.mwheel_y_delta > 0) {
            // zoom out
            f32 mult = PositiveSqrtMultiplier((f32) m.mwheel_y_delta);
            radius /= 1.1f * mult;
        }
        else if (m.r) {
            // pan
            Vector3f forward = - SphericalCoordsY(theta*deg2rad, phi*deg2rad, radius);
            forward.Normalize();
            Vector3f left = y_hat.Cross(forward);
            left.Normalize();
            Vector3f right = - left;
            Vector3f up = forward.Cross(left);
            up.Normalize();
            center = center + mouse2pan * m.dx * right;
            center = center + mouse2pan * m.dy * up;
        }

        // build orbit transform
        view = TransformBuildOrbitCam(center, theta, phi, radius);
        vp = TransformBuildViewProj(view, proj);
    }
    */
    Vector3f Forward() {
        Vector3f forward = - SphericalCoordsY(theta*deg2rad, phi*deg2rad, radius);
        return forward;
    }
    Vector3f Position() {
        Vector3f position = center + SphericalCoordsY(theta*deg2rad, phi*deg2rad, radius);
        return position;
    }
    Ray CameraRay() {
        Vector3f forward = - SphericalCoordsY(theta*deg2rad, phi*deg2rad, radius);
        Vector3f position = center + SphericalCoordsY(theta*deg2rad, phi*deg2rad, radius);
        Ray camray { position, forward };
        return camray;
    }
};
OrbitCamera InitOrbitCamera(float aspect) {
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


#endif

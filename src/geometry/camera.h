#ifndef __CAMERA_H__
#define __CAMERA_H__


Ray CameraGetRayWorld(Matrix4f view, f32 fov, f32 aspect, f32 x_frac = 0, f32 y_frac = 0) {
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
    Vector3f world = RayPlaneIntersect(CameraGetRayWorld(view, fov, aspect, x_frac, y_frac), TransformPoint(view, plane_origo), TransformPoint(view, plane_normal));

    return world;
}

struct OrbitCamera {
    f32 theta;
    f32 phi;
    f32 radius;
    f32 mouse2rot = 0.4f;
    f32 mouse2pan = 0.01f;
    Matrix4f view;
    Matrix4f parent;
    bool is_parented;

    // pan
    Vector3f drag_anchor;
    Vector3f center_anchor;
    Matrix4f view_anchor;
    bool drag;

    void SetRelativeTo(Matrix4f transform) {
        parent = transform;
        is_parented = true;
    }

    void SetRelativeWorld() {
        is_parented = false;
    }

    Vector3f Center() {
        Vector3f position = TransformGetTranslation(view);
        Vector3f cam_forward_w = TransformDirection(view, z_hat);
        Vector3f center = position + radius * cam_forward_w;
        return center;
    }

    void Update(Vector3f center) {

        Vector3f campos_relative = SphericalCoordsY(theta*deg2rad, phi*deg2rad, radius);
        view = TransformBuildTranslationOnly(center + campos_relative) * TransformBuildLookRotationYUp(center, center + campos_relative);

        if (is_parented) {
            view = TransformSetTranslation(view, campos_relative);
            view =  parent * view;
        }
    }
};

OrbitCamera OrbitCameraInit() {
    OrbitCamera cam = {};

    cam.theta = 60;
    cam.phi = 35;
    cam.radius = 4;

    cam.parent = Matrix4f_Identity();
    cam.view = TransformBuildOrbitCam(Vector3f_Zero(), cam.theta, cam.phi, cam.radius);
    return cam;
}

inline f32 _ScrollMult(f32 value) {
    if (value == 0) {
        return 1.0;
    }
    else {
        return sqrt(abs(value));
    }
}

Vector3f CameraGetPointInPlane(Matrix4f view, f32 fov, f32 aspect, Vector3f plane_origo_w, Vector3f plane_normal_w, f32 x_frac = 0, f32 y_frac = 0) {
    Ray m_w = CameraGetRayWorld(view, fov, aspect, x_frac, y_frac);
    Vector3f hit_w = RayPlaneIntersect(m_w, plane_origo_w, plane_normal_w);

    return hit_w;
}

static f32 _ClampTheta(f32 theta_degs, f32 min = 0.0001f, f32 max = 180 - 0.0001f) {
    f32 clamp_up = MinF32(theta_degs, max);
    f32 result = MaxF32(clamp_up, min);
    return result;
}

void OrbitCameraRotateZoom(OrbitCamera *cam, f32 dx, f32 dy, bool do_rotate, f32 scroll_y_offset) {
    Vector3f initial_center = cam->Center();
    if (do_rotate) {
        cam->theta = _ClampTheta(cam->theta - dy * cam->mouse2rot);
        cam->phi += - dx * cam->mouse2rot;
    }
    else if (scroll_y_offset < 0) {
        f32 mult = _ScrollMult(scroll_y_offset);
        cam->radius *= 1.1f * mult;
    }
    else if (scroll_y_offset > 0) {
        f32 mult = _ScrollMult(scroll_y_offset);
        cam->radius /= 1.1f * mult;
    }
    cam->Update(initial_center);
}

void OrbitCameraPanInPlane(OrbitCamera *cam, f32 fov, f32 aspect, f32 cursor_x_frac, f32 cursor_y_frac, bool enable, bool disable) {
    Vector3f plane_origo_w = { 0, 0, 0 };
    Vector3f plane_normal_w = { 0, 1, 0 };

    if (disable) {
        cam->view_anchor = {};
        cam->drag_anchor = {};
        cam->center_anchor = {};
        cam->drag = false;
    }
    else if (enable) {
        cam->view_anchor = cam->view;
        cam->center_anchor = cam->Center();
        cam->drag_anchor = CameraGetPointInPlane(cam->view_anchor, fov, aspect, plane_origo_w, plane_normal_w, cursor_x_frac, cursor_y_frac);
        cam->drag = true;
    }
    else if (cam->drag == true) {
        Vector3f cam_drag = CameraGetPointInPlane(cam->view_anchor, fov, aspect, plane_origo_w, plane_normal_w, cursor_x_frac, cursor_y_frac);
        Vector3f new_center = cam->center_anchor - (cam_drag - cam->drag_anchor);
        cam->Update(new_center);
    }
    cam->Update(cam->Center());
}

/*
void OrbitCameraPan(OrbitCamera *cam, f32 fov, f32 aspect, f32 cursor_x_frac, f32 cursor_y_frac, bool enable, bool disable) {
    if (disable) {
        cam->view_anchor = {};
        cam->drag_anchor = {};
        cam->center_anchor = {};
        cam->drag = false;
    }
    else if (enable) {
        cam->view_anchor = cam->view;
        cam->drag_anchor = CameraGetPointAtDepth(cam->view_anchor, fov, aspect, Vector3f_Zero(), cursor_x_frac, cursor_y_frac);
        //cam->center_anchor = cam->center;
        cam->center_anchor = cam->Center();
        cam->drag = true;
    }
    else if (cam->drag == true) {
        Vector3f cam_drag = CameraGetPointAtDepth(cam->view_anchor, fov, aspect, Vector3f_Zero(), cursor_x_frac, cursor_y_frac);
        //cam->center = cam->center_anchor - (cam_drag - cam->drag_anchor);
        cam->SetCenterTranslation(cam->center_anchor - (cam_drag - cam->drag_anchor));
    }

    cam->UpdateMatrices();
}
*/


/*
void OrbitCameraPanAlongZ(OrbitCamera *cam, f32 fov, f32 aspect, f32 cursor_x_frac, f32 cursor_y_frac, bool enable, bool disable) {
    Vector3f plane_origo_w = { 0, 0, 0 };
    Vector3f plane_normal_w = { 0, 1, 0 };

    if (disable) {
        cam->view_anchor = {};
        cam->drag_anchor = {};
        cam->center_anchor = {};
        cam->drag = false;
    }
    else if (enable) {
        cam->view_anchor = cam->view;
        //cam->center_anchor = cam->center;
        cam->center_anchor = cam->Center();
        cam->drag_anchor = CameraGetPointInPlane(cam->view_anchor, fov, aspect, plane_origo_w, plane_normal_w, cursor_x_frac, cursor_y_frac);
        cam->drag = true;
    }
    else if (cam->drag == true) {
        Vector3f cam_drag = CameraGetPointInPlane(cam->view_anchor, fov, aspect, plane_origo_w, plane_normal_w, cursor_x_frac, cursor_y_frac);
        //cam->center = cam->center_anchor - (cam_drag - cam->drag_anchor).z * z_hat;
        cam->SetCenterTranslation(cam->center_anchor - (cam_drag - cam->drag_anchor).z * z_hat);
    }

    cam->UpdateMatrices();
}
*/

#endif

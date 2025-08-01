/*
MIT License

Copyright (c) 2025 Jakob Garde

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#ifndef __JG_CBUI_H__
#define __JG_CBUI_H__


#include <math.h>
#include "jg_baselayer.h"
#ifndef __CBUI_H__
#define __CBUI_H__


#define CBUI_VERSION_MAJOR 0
#define CBUI_VERSION_MINOR 2
#define CBUI_VERSION_PATCH 1


void CbuiAssertVersion(u32 major, u32 minor, u32 patch) {
    if (
        CBUI_VERSION_MAJOR != major ||
        CBUI_VERSION_MINOR != minor ||
        CBUI_VERSION_PATCH != patch
    ) {
        assert(1 == 0 && "cbui version check failed");
    }
}

void CbuiPrintVersion() {
    printf("%d.%d.%d\n", CBUI_VERSION_MAJOR, CBUI_VERSION_MINOR, CBUI_VERSION_PATCH);
}


#endif


#ifndef __GTYPES_H__
#define __GTYPES_H__


//
// Colors


#define RGBA_BLACK      0, 0, 0, 255
#define RGBA_WHITE      255, 255, 255, 255
#define RGBA_GRAY_75    192, 192, 192, 255
#define RGBA_GRAY_50    128, 128, 128, 255
#define RGBA_GRAY_25    64, 64, 64, 255
#define RGBA_RED        255, 0, 0, 255
#define RGBA_GREEN      0, 255, 0, 255
#define RGBA_BLUE       0, 0, 255, 255
#define RGBA_YELLOW     255, 255, 0, 255


#define BYTES_RGB 3
#define BYTES_RGBA 4


struct Color {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
    u32 GetAsU32() {
        return * (u32*) this;
    }
    inline
    bool IsZero() {
        return r == 0 && g == 0 && b == 0 && a == 0;
    }
    inline
    bool IsNonZero() {
        return ! IsZero();
    }
};
void PrintColorInline(Color c) {
    printf("%hhx %hhx %hhx %hhx", c.r, c.g, c.b, c.a);
}
Color ColorRandom() {
    Color c;
    c.r = RandMinMaxU(100, 255);
    c.g = RandMinMaxU(100, 255);
    c.b = RandMinMaxU(100, 255);
    c.a = 255;
    return c;
}
Color ColorWhite() {
    return Color { RGBA_WHITE };
}
Color ColorBlack() {
    return Color { RGBA_BLACK };
}
Color ColorGray(f32 grayness) {
    u8 g = (u8) floor(grayness * 255);
    return Color { g, g, g, 255 };
}
Color ColorRed() {
    return Color { RGBA_RED };
}
Color ColorGreen() {
    return Color { RGBA_GREEN };
}
Color ColorBlue() {
    return Color { RGBA_BLUE };
}
Color ColorYellow() {
    return Color { RGBA_YELLOW };
}


//
// Rectangles


struct Rect {
    u16 width;
    u16 height;
    s16 left;
    s16 top;

    void Print() {
        printf("rect: w: %u, h: %u, left: %d, top: %d\n", width, height, left, top);
    }
};


Rect InitRectangle(u16 width, u16 height, u16 left = 0, u16 top = 0) {
    Rect r;
    r.width = width;
    r.height = height;
    r.left = left;
    r.top = top;
    return r;
}
Rect RectangleCrop(Rect us, Rect other) {
    Rect rect = InitRectangle(other.width, other.height, other.left, other.top);
    bool occluded = false;
    bool partially_occluded = false;

    // cases where other is completely outside of us
    if (other.left > us.left + us.width) { // to the right of us
        rect.left = us.left + us.width;
        rect.width = 0;
        occluded = true;
    }
    if (other.left + other.width < us.left) { // to the left of us
        rect.left = us.left;
        rect.width = 0;
        occluded = true;
    }
    if (other.top > us.top + us.height) { // above us
        rect.top = us.top + us.height;
        rect.height = 0;
        occluded = true;
    }
    if (other.top + other.height < us.top) { // below us
        rect.top = us.top;
        rect.height = 0;
        occluded = true;
    }
    if (occluded) {
        return rect;
    }

    // at least partially visible
    if (other.left < us.left) {
        rect.left = us.left;
        s16 diff = us.left - other.left;
        rect.width = rect.width - diff;
    }
    if (other.top < us.top) {
        rect.top = us.top;
        s16 diff = us.top - other.top;
        rect.height = rect.height - diff;
    }
    if (other.left + other.width > us.left + us.width) {
        rect.width = us.top + us.width;
    }
    if (other.top + other.height > us.top + us.height) {
        rect.height = us.top + us.height;
    }
    return rect;
}


//
//  Image type structs


struct ImageB {
    s32 width;
    s32 height;
    u8 *img;
};

struct ImageRGBX {
    u32 width;
    u32 height;
    u32 pixel_size;
    u8 *img;
};
ImageRGBX InitImageRGBX(void *data, u32 width, u32 height, u32 pixel_size) {
    ImageRGBX img;
    img.width = width;
    img.height = height;
    img.pixel_size = pixel_size;
    img.img = (u8*) data;
    return img;
}

struct ImageRGBA {
    s32 width;
    s32 height;
    Color *img;
};
ImageRGBA InitImageRGBA(s32 w, s32 h, void *buffer) {
    return ImageRGBA { w, h, (Color*) buffer };
}

struct ImageF32 {
    s32 width;
    s32 height;
    f32 *data;
};


#endif


#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__


//
// Vector


struct Vector2_u16 {
    u16 x;
    u16 y;
};

struct Vector2_s16 {
    s16 x;
    s16 y;
};

struct Vector3i {
    u32 i1;
    u32 i2;
    u32 i3;
};

struct Vector2i {
    s32 i1;
    s32 i2;
};

struct Vector2u {
    u32 i1;
    u32 i2;
};

struct Vector2s {
    u32 x;
    u32 y;
};

struct Vector2f {
    f32 x;
    f32 y;
    inline
    static Vector2f Add(Vector2f *a, Vector2f *b) {
        return Vector2f { a->x + b->x, a->y + b->y };
    }
};

inline
Vector2f operator+(Vector2f u, Vector2f v) {
    return Vector2f::Add(&u, &v);
}

Vector2f Vector2f_Zero() {
    return {};
}

struct Matrix4f {
    float m[4][4];
};

struct Vector4f {
    float x;
    float y;
    float z;
    float w;
};

struct Vector3f {
    float x;
    float y;
    float z;

    inline
    bool IsNonZero() {
        return abs(x) > 0.0001f || abs(y) > 0.0001f || abs(z) > 0.0001f;
    }
    inline bool IsZero() {
        return ! IsNonZero();
    }
    inline
    float Norm() {
        return sqrt(x*x + y*y + z*z);
    }
    inline
    float NormSquared() {
        return x*x + y*y + z*z;
    }
    inline
    void ScalarProductOn(float f) {
        x *= f;
        y *= f;
        z *= f;
    }
    inline
    void AddBy(Vector3f v) {
        x += v.x;
        y += v.y;
        z += v.z;
    }
    inline
    void SubtractBy(Vector3f v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }
    inline
    void Normalize() {
        float f = 1 / Norm();
        x *= f;
        y *= f;
        z *= f;
    }
    inline
    Vector3f Unit() {
        float f = 1 / Norm();
        Vector3f unit = {};
        unit.x = x * f;
        unit.y = y * f;
        unit.z = z * f;
        return unit;
    }
    inline
    void Invert() {
        x *= -1;
        y *= -1;
        z *= -1;
    }
    inline
    float Dot(Vector3f v) {
        return x*v.x + y*v.y + z*v.z;
    }
    inline
    Vector3f Cross(Vector3f v) {
        return Vector3f { y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x };
    }

    // static versions
    inline
    static Vector3f Zero() {
        return Vector3f { 0, 0, 0 };
    }
    inline
    // left-handed: Left, Up, Forward
    static Vector3f Left() {
        return Vector3f { 1, 0, 0 };
    }
    inline
    static Vector3f Up() {
        return Vector3f { 0, 1, 0 };
    }
    inline
    static Vector3f Forward() {
        return Vector3f { 0, 0, 1 };
    }
    inline
    static Vector3f X() {
        return Vector3f { 1, 0, 0 };
    }
    inline
    static Vector3f Y() {
        return Vector3f { 0, 1, 0 };
    }
    inline
    static Vector3f Z() {
        return Vector3f { 0, 0, 1 };
    }
    inline
    static float NormSquared(Vector3f a) {
        return a.x*a.x + a.y*a.y + a.z*a.z;
    }
    inline
    static float Norm(Vector3f a) {
        return sqrt(Vector3f::NormSquared(a));
    }
    inline
    static Vector3f ScalarProduct(float f, Vector3f *a) {
        return Vector3f { f*a->x, f*a->y, f*a->z };
    }
    inline
    static Vector3f Normalize(Vector3f a) {
        float norm_inv = 1 / Vector3f::Norm(a);
        return Vector3f { norm_inv * a.x, norm_inv * a.y, norm_inv * a.z };
    }
    inline
    static Vector3f Subtract(Vector3f *a, Vector3f *b) {
        return Vector3f { a->x - b->x, a->y - b->y, a->z - b->z };
    }
    inline
    static Vector3f Add(Vector3f *a, Vector3f *b) {
        return Vector3f { a->x + b->x, a->y + b->y, a->z + b->z };
    }
    inline
    static float Dot(Vector3f *a, Vector3f *b) {
        return a->z*b->z + a->z*b->z + a->z*b->z;
    }
    inline
    static Vector3f Cross(Vector3f *a, Vector3f *b) {
        return Vector3f { a->y*b->z - a->z*b->y, a->z*b->x - a->x*b->z, a->x*b->y - a->y*b->x };
    }
};

inline
Vector3f operator+(Vector3f u, Vector3f v) {
    return Vector3f::Add(&u, &v);
}

inline
Vector3f operator-(Vector3f u, Vector3f v) {
    return Vector3f::Subtract(&u, &v);
}

inline
bool operator==(Vector3f u, Vector3f v) {
    return (u.x == v.x) && (u.y == v.y) && (u.z == v.z);
}

inline
Vector3f operator*(float f, Vector3f v) {
    return Vector3f::ScalarProduct(f, &v);
}

inline
Vector3f operator-(Vector3f v) {
    return Vector3f::ScalarProduct(-1, &v);
}

Vector3f x_hat { 1, 0, 0 };
Vector3f y_hat { 0, 1, 0 };
Vector3f z_hat { 0, 0, 1 };

Vector3f Vector3f_Zero() {
    return Vector3f { 0, 0, 0 };
}

Vector3f Vector3f_Ones() {
    return Vector3f { 1, 1, 1 };
}

Vector3f SphericalCoordsY(float theta, float phi, float radius) {
    Vector3f v;
    v.x = radius * sin(theta) * cos(phi);
    v.y = radius * cos(theta);
    v.z = radius * sin(theta) * sin(phi);
    return v;
}


//
// Matrix


Matrix4f Matrix4f_Zero() {
    Matrix4f m;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            m.m[i][j] = 0;
        }
    }
    return m;
}

Matrix4f Matrix4f_One() {
    Matrix4f m;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            m.m[i][j] = 1;
        }
    }
    return m;
}

Matrix4f Matrix4f_Identity() {
    Matrix4f m;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (i == j) {
                m.m[i][j] = 1;
            }
            else {
                m.m[i][j] = 0;
            }
        }
    }
    return m;
}

bool Matrix4f_IsIdentity(Matrix4f m) {
    bool is_zero =
        m.m[0][0] == 1 &&
        m.m[0][1] == 0 &&
        m.m[0][2] == 0 &&
        m.m[0][3] == 0 &&

        m.m[1][0] == 0 &&
        m.m[1][1] == 1 &&
        m.m[1][2] == 0 &&
        m.m[1][3] == 0 &&

        m.m[2][0] == 0 &&
        m.m[2][1] == 0 &&
        m.m[2][2] == 1 &&
        m.m[2][3] == 0 &&

        m.m[3][0] == 0 &&
        m.m[3][1] == 0 &&
        m.m[3][2] == 0 &&
        m.m[3][3] == 1;
    return is_zero;
}

Matrix4f Matrix4f_Diagonal(Vector4f d) {
    Matrix4f m = Matrix4f_Zero();
    m.m[0][0] = d.x;
    m.m[1][1] = d.y;
    m.m[2][2] = d.z;
    m.m[3][3] = d.w;
    return m;
}

Matrix4f Matrix4f_Transpose(Matrix4f *m) {
    Matrix4f result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = m->m[j][i];
        }
    }
    return result;
}

Matrix4f Matrix4f_Transpose(Matrix4f m) {
    Matrix4f result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = m.m[j][i];
        }
    }
    return result;
}

Matrix4f Matrix4f_Multiply(Matrix4f *a, Matrix4f *b) {
    Matrix4f result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = 0;
            for (int k = 0; k < 4; ++k) {
                result.m[i][j] += a->m[i][k]*b->m[k][j];
            }
        }
    }
    return result;
}

Vector4f Matrix4f_MultVector(Matrix4f *a, Vector4f *v) {
    Vector4f result;
    result.x = a->m[0][0]*v->x + a->m[0][1]*v->y + a->m[0][2]*v->z + a->m[0][3]*v->w;
    result.y = a->m[1][0]*v->x + a->m[1][1]*v->y + a->m[1][2]*v->z + a->m[1][3]*v->w;
    result.z = a->m[2][0]*v->x + a->m[2][1]*v->y + a->m[2][2]*v->z + a->m[2][3]*v->w;
    result.w = a->m[3][0]*v->x + a->m[3][1]*v->y + a->m[3][2]*v->z + a->m[3][3]*v->w;
    return result;
}

bool Matrix4f_Equals(Matrix4f *a, Matrix4f *b) {
    bool result = true;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result &= a->m[i][j] == b->m[i][j];
        }
    }
    return result;
}

inline
Matrix4f operator*(Matrix4f a, Matrix4f b) {
    return Matrix4f_Multiply(&a, &b);
}

inline
Vector4f operator*(Matrix4f m, Vector4f v) {
    return Matrix4f_MultVector(&m, &v);
}

inline
bool operator==(Matrix4f a, Matrix4f b) {
    return Matrix4f_Equals(&a, &b);
}

inline
void MatrixNf_Transpose(float *dest, float *src, u32 dims) {
    for (u32 row = 0; row < dims; ++row) {
        for (u32 col = 0; col < dims; ++col) {
            dest[row*dims + col] = src[col*dims + row];
        }
    }
}

inline
void MatrixNf_Multiply(float *dest, float *a, float *b, u32 dims) {
    for (u32 i = 0; i < dims; ++i) {
        for (u32 j = 0; j < dims; ++j) {
            u32 I = i*dims;
            dest[I + j] = 0;
            for (u32 k = 0; k < dims; ++k) {
                dest[I + j] += a[I + k]*b[k*dims + j];
            }
        }
    }
}

inline
void MatrixNf_MultVector(float *dest, float *a, float *v, u32 dims) {
    for (u32 i = 0; i < dims; ++i) {
        dest[i] = 0;
        for (u32 k = 0; k < dims; ++k) {
            dest[i] += a[i*dims + k]*v[k];
        }
    }
}

Matrix4f Matrix4f_FlipX() {
    Matrix4f flip = Matrix4f_Identity();
    flip.m[0][0] = -1;
    return flip;
}

Matrix4f Matrix4f_FlipY() {
    Matrix4f flip = Matrix4f_Identity();
    flip.m[1][1] = -1;
    return flip;
}

Matrix4f Matrix4f_FlipZ() {
    Matrix4f flip = Matrix4f_Identity();
    flip.m[2][2] = -1;
    return flip;
}


//
// Transform


Matrix4f TransformBuild(Vector3f axis, float angle_rads, Vector3f translate = {0, 0, 0}) {
    Matrix4f result = Matrix4f_Zero();

    float epsilon_f = 0.0000001f;
    assert( abs(axis.Norm() - 1) < epsilon_f );

    // build rot from [axis, angle] - see https://en.wikipedia.org/wiki/Rotation_matrix#Axis_and_angle
    float x = axis.x;
    float y = axis.y;
    float z = axis.z;
    float c = cos(angle_rads);
    float s = sin(angle_rads);

    result.m[0][0] = x*x*(1 - c) + c;
    result.m[0][1] = x*y*(1 - c) - z*s;
    result.m[0][2] = x*z*(1 - c) + y*s;

    result.m[1][0] = y*x*(1 - c) + z*s;
    result.m[1][1] = y*y*(1 - c) + c;
    result.m[1][2] = y*z*(1 - c) - x*s;

    result.m[2][0] = z*x*(1 - c) - y*s;
    result.m[2][1] = z*y*(1 - c) + x*s;
    result.m[2][2] = z*z*(1 - c) + c;

    // translation
    result.m[0][3] = translate.x;
    result.m[1][3] = translate.y;
    result.m[2][3] = translate.z;
    result.m[3][3] = 1;

    return result;
}

Matrix4f TransformBuildRotateX(float angle_rads) {
    return TransformBuild(x_hat, angle_rads);
}

Matrix4f TransformBuildRotateY(float angle_rads) {
    return TransformBuild(y_hat, angle_rads);
}

Matrix4f TransformBuildRotateZ(float angle_rads) {
    return TransformBuild(z_hat, angle_rads);
}

Matrix4f TransformBuildTranslationOnly(Vector3f translate) {
    return TransformBuild(Vector3f {1, 0, 0}, 0, translate);
}

Matrix4f TransformBuildTranslation(Vector3f translate) {
    return TransformBuild(Vector3f {1, 0, 0}, 0, translate);
}

Matrix4f TransformGetInverse(Matrix4f *a) {
    // M^{-1}: row 0-2: R^{-1}, - R^{-1} * t; row 3: 0^, 1

    // 0^, 1
    Matrix4f result = Matrix4f_Zero();
    result.m[3][3] = 1;

    // R^{-1}
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            result.m[i][j] = a->m[j][i];
        }
    }

    // R^{-1} * t
    Vector3f t_inv;
    t_inv.x = result.m[0][0]*a->m[0][3] + result.m[0][1]*a->m[1][3] + result.m[0][2]*a->m[2][3];
    t_inv.y = result.m[1][0]*a->m[0][3] + result.m[1][1]*a->m[1][3] + result.m[1][2]*a->m[2][3];
    t_inv.z = result.m[2][0]*a->m[0][3] + result.m[2][1]*a->m[1][3] + result.m[2][2]*a->m[2][3];

    // *= -1 and copy
    result.m[0][3] = - t_inv.x;
    result.m[1][3] = - t_inv.y;
    result.m[2][3] = - t_inv.z;

    return result;
}

inline
Matrix4f TransformGetInverse(Matrix4f a) {
    return TransformGetInverse(&a);
}

inline
Vector3f TransformGetTranslation(Matrix4f transform) {
    Vector3f result { transform.m[0][3], transform.m[1][3], transform.m[2][3] };
    return result;
}

inline
Matrix4f TransformSetTranslation(const Matrix4f transform, const Vector3f translation) {
    Matrix4f result = transform;
    result.m[0][3] = translation.x;
    result.m[1][3] = translation.y;
    result.m[2][3] = translation.z;
    return result;
}

inline
Vector3f TransformPoint(Matrix4f *a, Vector3f *v) {
    Vector3f result;

    // rot / trans
    result.x = a->m[0][0]*v->x + a->m[0][1]*v->y + a->m[0][2]*v->z + a->m[0][3];
    result.y = a->m[1][0]*v->x + a->m[1][1]*v->y + a->m[1][2]*v->z + a->m[1][3];
    result.z = a->m[2][0]*v->x + a->m[2][1]*v->y + a->m[2][2]*v->z + a->m[2][3];

    return result;
}

inline
Vector3f TransformPoint(Matrix4f a, Vector3f v) {
    Vector3f result;

    // rot / trans
    result.x = a.m[0][0]*v.x + a.m[0][1]*v.y + a.m[0][2]*v.z + a.m[0][3];
    result.y = a.m[1][0]*v.x + a.m[1][1]*v.y + a.m[1][2]*v.z + a.m[1][3];
    result.z = a.m[2][0]*v.x + a.m[2][1]*v.y + a.m[2][2]*v.z + a.m[2][3];

    return result;
}

// TODO: how do I build a vector that does this ?
inline
Vector3f TransformInversePoint(Matrix4f *a, Vector3f *v) {
    Vector3f r;
    Vector3f tmp;

    // translate back
    tmp.x = v->x - a->m[0][3];
    tmp.y = v->y - a->m[1][3];
    tmp.z = v->z - a->m[2][3];

    // rotate back (transpose)
    r.x = a->m[0][0]*tmp.x + a->m[1][0]*tmp.y + a->m[2][0]*tmp.z;
    r.y = a->m[0][1]*tmp.x + a->m[1][1]*tmp.y + a->m[2][1]*tmp.z;
    r.z = a->m[0][2]*tmp.x + a->m[1][2]*tmp.y + a->m[2][2]*tmp.z;

    return r;
}

inline
Vector3f TransformInversePoint(Matrix4f a, Vector3f v) {
    Vector3f r;
    Vector3f tmp;

    // translate back
    tmp.x = v.x - a.m[0][3];
    tmp.y = v.y - a.m[1][3];
    tmp.z = v.z - a.m[2][3];

    // rotate back (transpose)
    r.x = a.m[0][0]*tmp.x + a.m[1][0]*tmp.y + a.m[2][0]*tmp.z;
    r.y = a.m[0][1]*tmp.x + a.m[1][1]*tmp.y + a.m[2][1]*tmp.z;
    r.z = a.m[0][2]*tmp.x + a.m[1][2]*tmp.y + a.m[2][2]*tmp.z;

    return r;
}

inline
Vector3f TransformDirection(Matrix4f *a, Vector3f *d) {
    Vector3f result;

    // rotate
    result.x = a->m[0][0]*d->x + a->m[0][1]*d->y + a->m[0][2]*d->z;
    result.y = a->m[1][0]*d->x + a->m[1][1]*d->y + a->m[1][2]*d->z;
    result.z = a->m[2][0]*d->x + a->m[2][1]*d->y + a->m[2][2]*d->z;

    return result;
}

inline
Vector3f TransformDirection(Matrix4f a, Vector3f d) {
    Vector3f result;

    // just rotate
    result.x = a.m[0][0]*d.x + a.m[0][1]*d.y + a.m[0][2]*d.z;
    result.y = a.m[1][0]*d.x + a.m[1][1]*d.y + a.m[1][2]*d.z;
    result.z = a.m[2][0]*d.x + a.m[2][1]*d.y + a.m[2][2]*d.z;

    return result;
}

inline
Vector3f TransformInverseDirection(Matrix4f *a, Vector3f *d) {
    Vector3f result;

    // rotate back
    result.x = a->m[0][0]*d->x + a->m[1][0]*d->y + a->m[2][0]*d->z;
    result.y = a->m[0][1]*d->x + a->m[1][1]*d->y + a->m[2][1]*d->z;
    result.z = a->m[0][2]*d->x + a->m[1][2]*d->y + a->m[2][2]*d->z;

    // TODO: scale back
    return result;
}

inline
Vector3f TransformInverseDirection(Matrix4f a, Vector3f d) {
    Vector3f result;

    // rotate back
    result.x = a.m[0][0]*d.x + a.m[1][0]*d.y + a.m[2][0]*d.z;
    result.y = a.m[0][1]*d.x + a.m[1][1]*d.y + a.m[2][1]*d.z;
    result.z = a.m[0][2]*d.x + a.m[1][2]*d.y + a.m[2][2]*d.z;

    // TODO: scale back
    return result;
}

Matrix4f TransformBuildLookRotationYUp(Vector3f at, Vector3f from) {
    Vector3f forward = at - from;
    forward.Normalize();
    Vector3f left = y_hat.Cross(forward);
    left.Normalize();
    Vector3f right = - left;
    Vector3f up = forward.Cross(left);
    up.Normalize();

    Matrix4f lookrot = Matrix4f_Identity();
    lookrot.m[0][0] = right.x;
    lookrot.m[1][0] = right.y;
    lookrot.m[2][0] = right.z;
    lookrot.m[0][1] = up.x;
    lookrot.m[1][1] = up.y;
    lookrot.m[2][1] = up.z;
    lookrot.m[0][2] = forward.x;
    lookrot.m[1][2] = forward.y;
    lookrot.m[2][2] = forward.z;

    return lookrot;
}


//
//  Quaternions


struct Quat {
    float w;
    float x;
    float y;
    float z;
};

Quat Quat_Identity() {
    Quat i { 1, 0, 0, 0 };
    return i;
}
Quat QuatAxisAngle(Vector3f axis, float angle) {
    float c = cos(angle * 0.5f);
    float s = sin(angle * 0.5f);
    Quat q;
    q.w = c;
    q.x = axis.x * s;
    q.y = axis.y * s;
    q.z = axis.z * s;
    return q;
}
inline
Quat QuatFromVector(Vector3f v) {
    Quat t { 0.0f, v.x, v.y, v.z };
    return t;
}
inline
Quat QuatInverse(Quat q) {
    Quat t { q.w, -q.x, -q.y, -q.z };
    return t;
}
Quat QuatMult(Quat q1, Quat q2) {
    Quat t;
    t.w = q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z;
    t.x = q1.w*q2.x + q1.x*q2.w - q1.y*q2.z + q1.z*q2.y;
    t.y = q1.w*q2.y + q1.x*q2.z + q1.y*q2.w - q1.z*q2.x;
    t.z = q1.w*q2.z - q1.x*q2.y + q1.y*q2.x + q1.z*q2.w;
    return t;
}
Vector3f QuatRotate(Quat q, Vector3f v) {
    Quat q_inv = QuatInverse(q);
    Quat q_v = QuatFromVector(v);
    Quat q_v_rot = QuatMult(q_inv, QuatMult(q, q_v));

    Vector3f v_rot { q_v_rot.x, q_v_rot.y, q_v_rot.z };
    return v_rot;
}
Matrix4f TransformQuaternion(Quat q) {
    Matrix4f m = Matrix4f_Identity();
    m.m[0][0] = q.w*q.w + q.x*q.x - q.y*q.y - q.z*q.z;
    m.m[0][1] = 2*q.x*q.y - 2*q.w*q.z;
    m.m[0][2] = 2*q.x*q.z + 2*q.w*q.y;
    m.m[1][0] = 2*q.x*q.y + 2*q.w*q.z;
    m.m[1][1] = q.w*q.w - q.x*q.x + q.y*q.y - q.z*q.z;
    m.m[1][2] = 2*q.y*q.z - 2*q.w*q.x;
    m.m[2][0] = 2*q.x*q.z - 2*q.w*q.y;
    m.m[2][1] = 2*q.y*q.z + 2*q.w*q.x;
    m.m[2][2] = q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z;
    m.m[3][3] = 1;
    return m;
}
inline
float Matrix4f_Trace(Matrix4f m) {
    float trace = m.m[0][0] + m.m[1][1] + m.m[2][2] + 1;
    return trace;
}
Quat QuatFromTransform(Matrix4f m) {
    assert(Matrix4f_Trace(m) > 0.0f);

    Quat q;
    q.w = sqrt( Matrix4f_Trace(m) ) * 0.5f;
    q.x = (m.m[2][1] - m.m[1][2]) / (4 * q.w);
    q.y = (m.m[0][2] - m.m[2][0]) / (4 * q.w);
    q.z = (m.m[1][0] - m.m[0][1]) / (4 * q.w);
    return q;
}
inline
Quat QuatScalarMult(Quat q, float s) {
    Quat t { s * q.w, s * q.x, s * q.y, s * q.z };
    return t;
}
inline
Quat QuatSum(Quat q1, Quat q2) {
    Quat t { q1.w + q2.w, q1.x + q2.x, q1.y + q2.y, q1.z + q2.z };
    return t;
}
inline
float QuatInnerProduct(Quat q1, Quat q2) {
    float dotprod = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
    return dotprod;
}
Quat Slerp(Quat q1, Quat q2, float t) {
    // TODO: not robust with q1 == q2, please fix
    assert(t >= 0.0f && t <= 1.0f);

    float theta = acos( QuatInnerProduct(q1, q2) );
    float f1 = sin((1 - t)*theta) / sin(theta);
    float f2 = sin(t*theta) / sin(theta);

    Quat q = QuatSum( QuatScalarMult(q1, f1), QuatScalarMult(q2, f2) );
    return q;
}


//
//  Projection & camera model


struct LensParams {
    float fL; // focal length, typically 24 - 200 [mm]
    float N; // f-number, 1.4 to 60 dimensionless []
    float c; // circle of confusion (diameter), 0.03 [mm]
    float w; // sensor width, 35.9 [mm]
    float h; // sensor height, 24 [mm]
};

struct Perspective {
    float fov; // [degs] (horizontal field of view)
    float aspect; // [1] (width divided by height)
    float dist_near; // [m]
    float dist_far; // [m]
    Matrix4f proj;
};

Matrix4f PerspectiveMatrixOpenGL(f32 farr, f32 nearr, f32 fov, f32 aspect, bool flip_x = true, bool flip_y = false, bool flip_z = true) {
    // gather values
    float f = farr;
    float n = nearr;
    float r = nearr * sin(fov / 2 * deg2rad);
    float l = -r;
    float b = r / aspect;
    float t = -b;

    // populate
    Matrix4f m = Matrix4f_Zero();
    m.m[0][0] = 2 * n / (r - l);
    m.m[0][2] = (r + l) / (r - l);
    m.m[1][1] = 2 * n / (t - b);
    m.m[1][2] = (t + b) / (t - b);
    m.m[2][2] = -(f + n) / (f - n);
    m.m[2][3] = -2 * f * n / (f - n);
    m.m[3][2] = -1;

    // flip the axes (flip to suit desired axis configurations)
    Matrix4f flip = Matrix4f_Identity();
    if (flip_x) {
        flip.m[0][0] = -1 * flip.m[0][0];
        m = flip * m;
    }
    if (flip_y) {
        flip.m[1][1] = -1 * flip.m[1][1];
        m = flip * m;
    }
    if (flip_z) {
        flip.m[2][2] = -1 * flip.m[2][2];
        m = flip * m;
    }

    return m;
}

void PerspectiveSetAspectAndP(Perspective *proj, u32 width = 0, u32 height = 0) {
    if (width != 0 && height != 0) {
        f32 aspect_new = width / (f32) height;

        if (aspect_new != proj->aspect) {
            proj->aspect = aspect_new;
            proj->proj = PerspectiveMatrixOpenGL(proj->dist_near, proj->dist_far, proj->fov, proj->aspect, false, true, false);
        }
    }
}

Perspective ProjectionInit(u32 width, u32 height) {
    Perspective proj = {};
    proj.fov = 90;
    proj.dist_near = 0.01f;
    proj.dist_far = 10.0f;
    PerspectiveSetAspectAndP(&proj, width, height);

    return proj;
}

inline
Matrix4f TransformBuildMVP(Matrix4f model, Matrix4f view, Matrix4f proj) {
    Matrix4f mvp = proj * TransformGetInverse( view ) * model;
    return mvp;
}

inline
Matrix4f TransformBuildMVP(Matrix4f model, Matrix4f vp) {
    Matrix4f mvp = vp * model;
    return mvp;
}

inline
Matrix4f TransformBuildViewProj(Matrix4f view, Matrix4f proj) {
    Matrix4f mvp = proj * TransformGetInverse( view );
    return mvp;
}

inline
Matrix4f TransformBuildOrbitCam(Vector3f center, float theta_degs, float phi_degs, float radius, Vector3f *campos_out) {
    Vector3f campos = center + SphericalCoordsY(theta_degs*deg2rad, phi_degs*deg2rad, radius);
    Matrix4f view = TransformBuildTranslationOnly(campos) * TransformBuildLookRotationYUp(center, campos);

    if (campos_out) {
        *campos_out = campos;
    }
    return view;
}

inline
Vector3f TransformPerspective(Matrix4f p, Vector3f v) {
    Vector4f v_hom { v.x, v.y, v.z, 1 }; // homogeneous coordinates
    Vector4f v_clip = p * v_hom; // clipping space
    Vector3f result { v_clip.x / v_clip.w, v_clip.y / v_clip.w, v_clip.z / v_clip.w }; // ndc coordinates

    return result;
}


//
//  Ray


struct Ray {
    // points: (x, y, z, 1)
    // directions: (x, y, z, 0)
    Vector3f pos;
    Vector3f dir;

    inline
    static Ray Zero() {
        return Ray { Vector3f::Zero(), Vector3f::Zero() };
    }
};
Ray TransformRay(Matrix4f *a, Ray *r) {
    return Ray { TransformPoint(a, &r->pos), TransformDirection(a, &r->dir) };
}
inline
Ray TransformRay(Matrix4f a, Ray r) {
    return Ray { TransformPoint(a, r.pos), TransformDirection(a, r.dir) };
}
inline
Ray TransformInverseRay(Matrix4f a, Ray r) {
    return Ray { TransformInversePoint(a, r.pos), TransformInverseDirection(a, r.dir) };
}


//
// Plane / Line / Point / Triangle Helpers


bool PointSideOfPlane(Vector3f point, Ray plane) {
    // returns true if point is in the R3-halfspace defined by plane normal

    Vector3f diff = (plane.pos - point);
    diff.Normalize();
    f32 cos_angle = diff.Dot(plane.dir);

    if (cos_angle <= 0) {
        return true;
    }
    else {
        return false;
    }
}

Vector3f RayPlaneIntersect(Ray ray, Vector3f plane_origo, Vector3f plane_normal, f32 *t_at = NULL) {
    f32 dot = plane_normal.Dot(ray.dir);
    if (abs(dot) > 0.0001f) {
        f32 t = (plane_origo - ray.pos).Dot(plane_normal) / dot;
        if (t_at) {
            *t_at = t;
        }

        Vector3f result = ray.pos + t * ray.dir;
        return result;
    }
    else {
        return {};
    }
}

Vector3f PointToLine(Vector3f point, Vector3f line_origo, Vector3f line_direction) {
    Vector3f diff = point - line_origo;
    f32 coeff = diff.Dot(line_direction);
    Vector3f proj = line_origo + coeff * line_direction;

    return proj;
}

f32 PointToLineDist(Vector3f point, Vector3f line_origo, Vector3f line_direction) {
    Vector3f diff = point - line_origo;
    f32 coeff = diff.Dot(line_direction);
    Vector3f proj = line_origo + coeff * line_direction;

    return proj.Norm();
}

bool PerpendicularUnitVectors(Vector3f v1, Vector3f v2) {
    bool perpendicular = abs(v1.Dot(v2) - 1) > 0.000f;

    return perpendicular;
}

bool LineToLineDist(Vector3f line1_origo, Vector3f line1_dir, Vector3f line2_origo, Vector3f line2_dir, f32 *dist) {
    if (PerpendicularUnitVectors(line1_dir, line2_dir)) {
        if (dist) {
            *dist = PointToLineDist(line1_origo, line2_origo, line2_dir);
        }

        return false;
    }
    else {
        Vector3f n = line1_dir.Cross(line2_dir);
        n.Normalize();
        if (dist) {
            *dist = n.Dot(line1_origo - line2_origo);
        }

        return true;
    }
}

Vector3f PointToPlane(Vector3f point, Vector3f plane_origo, Vector3f plane_normal) {
    Vector3f delta = point - plane_origo;
    f32 dot = delta.Dot(plane_normal);
    Vector3f result = point - dot * plane_normal;

    return result;
}


bool RayCastTriangle(Ray r, Vector3f v1, Vector3f v2, Vector3f v3, Vector3f *hit)
{
    Vector3f plane_hit = RayPlaneIntersect(r, v1, (v2 - v1).Cross(v3 - v1));

    Vector3f v1h = plane_hit - v1;
    Vector3f v2h = plane_hit - v2;
    Vector3f v3h = plane_hit - v3;
    v1h.Normalize();
    v2h.Normalize();
    v3h.Normalize();

    float a1 = acos(v1h.Dot(v2h));
    float a2 = acos(v2h.Dot(v3h));
    float a3 = acos(v3h.Dot(v1h));

    bool did_hit = abs(a1 + a2 + a3 - 2 * PI) < 0.0001f;
    if (did_hit && hit) {
        *hit = plane_hit;
    }
    return did_hit;
}


//
// Utility


void PrintTransform(Matrix4f m) {
    printf("[");
    for (int i = 0; i < 4; ++i) {
        if (i > 0) {
            printf(" ");
        }
        for (int j = 0; j < 4; ++j) {
            printf(" %f", m.m[i][j]);
        }
        if (i < 3) {
            printf("\n");
        }
    }
    printf(" ]\n\n");
}

void PopulateMatrixRandomly(Matrix4f *m) {
    RandInit();
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            m->m[i][j] = RandMinMaxI_f32(0, 9);
        }
    }
}


#endif


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

    // pan
    Vector3f anchor;
    Vector3f center_anchor;
    Matrix4f view_anchor;
    bool drag;
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

inline f32 _ScrollMult(f32 value) {
    if (value == 0) {
        return 1.0;
    }
    else {
        return sqrt(abs(value));
    }
}
static f32 _ClampTheta(f32 theta_degs, f32 min = 0.0001f, f32 max = 180 - 0.0001f) {
    f32 clamp_up = MinF32(theta_degs, max);
    f32 result = MaxF32(clamp_up, min);
    return result;
}

void OrbitCameraUpdate(OrbitCamera *cam, f32 dx, f32 dy, bool do_rotate, f32 scroll_y_offset) {
    if (do_rotate) {
        cam->theta = _ClampTheta(cam->theta - dy * cam->mouse2rot);
        cam->phi += - dx * cam->mouse2rot;
    }
    else if (scroll_y_offset < 0) {
        f32 mult = _ScrollMult((f32) scroll_y_offset);
        cam->radius *= 1.1f * mult;
    }
    else if (scroll_y_offset > 0) {
        f32 mult = _ScrollMult((f32) scroll_y_offset);
        cam->radius /= 1.1f * mult;
    }

    // build orbit transform
    
    cam->view = TransformBuildOrbitCam(cam->center, cam->theta, cam->phi, cam->radius, &cam->position);
    cam->position_world = TransformPoint(cam->view, {});
}
void OrbitCameraPan(OrbitCamera *cam, f32 fov, f32 aspect, f32 cursor_x_frac, f32 cursor_y_frac, bool enable, bool disable) {
    if (disable) {
        cam->view_anchor = {};
        cam->anchor = {};
        cam->center_anchor = {};
        cam->drag = false;
    }
    else if (enable) {
        cam->view_anchor = cam->view;
        cam->anchor = CameraGetPointAtDepth(cam->view_anchor, fov, aspect, Vector3f_Zero(), cursor_x_frac, cursor_y_frac);
        cam->center_anchor = cam->center;
        cam->drag = true;
    }
    else if (cam->drag == true) {
        Vector3f cam_drag = CameraGetPointAtDepth(cam->view_anchor, fov, aspect, Vector3f_Zero(), cursor_x_frac, cursor_y_frac);
        cam->center = cam->center_anchor - (cam_drag - cam->anchor);
    }
}


#endif


#ifndef __SCENEGRAPH_H__
#define __SCENEGRAPH_H__


struct SGNode;


static MPool g_p_sgnodes;
static void *g_sg_root_addr;


struct SGNode {
    Matrix4f t_loc;
    Matrix4f t_world;
    u16 next;
    u16 first;
    u16 parent;
    u16 index;

    inline
    SGNode *Next() {
        return (SGNode *) PoolIdx2Ptr(&g_p_sgnodes, next);
    }

    inline
    SGNode *First() {
        return (SGNode *) PoolIdx2Ptr(&g_p_sgnodes, first);
    }

    inline
    SGNode *Parent() {
        if (parent) {
            return (SGNode *) PoolIdx2Ptr(&g_p_sgnodes, parent);
        }
        else{
            return (SGNode*) g_sg_root_addr;
        }
    }

    void AppendChild(SGNode *c) {
        if (first == 0) {
            first = c->index;
        }
        else {
            SGNode *n = First();
            while (n->next) {
                n = n->Next();
            }
            n->next = c->index;
        }
        c->parent = index;
    }

    void RemoveChild(SGNode *t) {
        if (first == t->index) {
            first = t->next;
        }
        else {
            // find prev
            SGNode *prev = NULL;
            SGNode *c = First();

            while (c) {
                if (c->index == t->index && prev) {
                    assert(c == t);

                    prev->next = t->next;
                }
                else {
                    prev = c;
                    c = c->Next();
                }
            }
        }
    }
};


static SGNode g_sg_root;


void SceneGraphInit(s32 cap = 256) {
    assert(g_p_sgnodes.mem == NULL);

    g_p_sgnodes = PoolCreate(sizeof(SGNode), cap + 1);
    // lock index-0:
    PoolAlloc(&g_p_sgnodes);

    //  The root node is both the zero-stub, the tree root node, and the object of index 0.
    //  Thus every zero-initialized node is already has root as its parent

    g_sg_root.t_loc = Matrix4f_Identity();
    g_sg_root.t_world = Matrix4f_Identity();

    // the address of the root node (necessary due to laguage quirks)
    g_sg_root_addr = &g_sg_root;
}


SGNode *SceneGraphAlloc(SGNode *parent = NULL) {
    SGNode *t = (SGNode*) PoolAlloc(&g_p_sgnodes);
    t->index = (u16) PoolPtr2Idx(&g_p_sgnodes, t);
    t->t_loc = Matrix4f_Identity();
    t->t_world = Matrix4f_Identity();

    assert(t->index != 0);

    if (!parent) {
        parent = &g_sg_root;
    }
    parent->AppendChild(t);

    return t;
}


void SceneGraphFree(SGNode * t) {
    t->Parent()->RemoveChild(t);

    // relinquish child branches -> to root
    SGNode *c = t->First();
    SGNode *nxt = c;
    while (nxt) {
        c = nxt;
        nxt = c->Next();

        c->next = 0;
        g_sg_root.AppendChild(c);
    }

    PoolFree(&g_p_sgnodes, t);
}


#endif


#ifndef __COLOR_H__
#define __COLOR_H__


#define COLOR_RED (( Color { 255, 0, 0, 255 } ))
#define COLOR_GREEN (( Color { 0, 255, 0, 255 } ))
#define COLOR_YELLOW (( Color { 255, 255, 0, 255 } ))
#define COLOR_YELLOW2 (( Color { 240, 240, 50, 255 } ))
#define COLOR_BLUE (( Color {  0, 0, 255, 255 } ))
#define COLOR_BLACK (( Color { 0, 0, 0, 255 } ))
#define COLOR_WHITE (( Color { 255, 255, 255, 255 } ))
#define COLOR_GRAY (( Color { 128, 128, 128, 255 } ))
#define COLOR_GRAY_50 (( Color { 128, 128, 128, 255 } ))
#define COLOR_GRAY_60 (( Color { 150, 150, 150, 255 } ))
#define COLOR_GRAY_75 (( Color { 192, 192, 192, 255 } ))
#define COLOR_GRAY_80 (( Color { 217, 217, 217, 255 } ))


#endif


#ifndef __SPRITE_H__
#define __SPRITE_H__


struct Sprite {
    s32 w;
    s32 h;
    f32 u0;
    f32 u1;
    f32 v0;
    f32 v1;
};

inline
void PrintSprite(Sprite s) {
    printf("sprite w: %d, h: %d u0: %.4f, u1: %.4f, v0: %.4f, v1: %.4f\n", s.w, s.h, s.u0, s.u1, s.v0, s.v1);
}


//
//  Quads


struct QuadVertex {  // vertex layout
    Vector2f pos;
    Vector2f tex;

    // TODO: we can pack this by using just some flags or bytes that refer 
    //      to (pre-packated / matching / uniform) values
    //      In which case we should rename these to 
    //          u_texture_idx
    //          u_color_idx
    //      or some such.

    u64 tex_id;
    Color col;
};

inline
QuadVertex InitQuadVertex(Vector2f pos, Vector2f tex, Color color, u64 texture_id) {
    QuadVertex v = {};
    v.pos = pos;
    v.tex = tex;
    v.col = color;
    v.tex_id = texture_id;
    return v;
}

struct QuadHexaVertex { // renderable six-vertex quad
    QuadVertex verts[6];

    inline
    u64 GetTextureId() {
        return verts[0].tex_id;
    }
    inline
    Color GetColor() {
        return verts[0].col;
    }
    inline
    void SetColor(Color color) {
        for (s32 i = 0; i < 6; ++i) {
            verts[i].col = color;
        }
    }
    inline
    f32 GetWidth() {
        f32 x0 = verts[2].pos.x;
        f32 x1 = verts[0].pos.x;
        f32 width = x1 - x0;
        return width;
    }
    inline
    f32 GetHeight() {
        f32 y0 = verts[0].pos.y;
        s32 y1 = verts[2].pos.y;
        s32 width = y1 - y0;
        return width;
    }
    inline
    f32 GetX0() {
        f32 x0 = verts[2].pos.x;
        return x0;
    }
    inline
    f32 GetY0() {
        f32 y0 = verts[0].pos.y;
        return y0;
    }
    inline
    f32 GetX1() {
        f32 x1 = verts[0].pos.x;
        return x1;
    }
    inline
    f32 GetY1() {
        f32 y1 = verts[1].pos.y;
        return y1;
    }
    inline
    f32 GetTextureScaleX(s32 dest_width) {
        f32 u0 = verts[2].tex.x;
        f32 u1 = verts[0].tex.x;
        f32 scale_x = (u1 - u0) / dest_width;
        return scale_x;
    }
    inline
    f32 GetTextureScaleY(s32 dest_height) {
        f32 v0 = verts[0].tex.y;
        f32 v1 = verts[2].tex.y;
        f32 scale_y = (v1 - v0) / dest_height;
        return scale_y;
    }
    inline
    f32 GetTextureU0() {
        f32 u0 = verts[2].tex.x;
        return u0;
    }
    inline
    f32 GetTextureU1() {
        f32 u1 = verts[0].tex.x;
        return u1;
    }
    inline
    f32 GetTextureV0() {
        f32 v0 = verts[0].tex.y;
        return v0;
    }
    inline
    f32 GetTextureV1() {
        f32 v1 = verts[2].tex.y;
        return v1;
    }
    f32 GetTextureWidth() {
        f32 u0 = verts[2].tex.x;
        f32 u1 = verts[0].tex.x;
        return u1 - u0;
    }
    inline
    f32 GetTextureHeight() {
        f32 v0 = verts[0].tex.y;
        f32 v1 = verts[2].tex.y;
        return v1 - v0;
    }
};

QuadHexaVertex QuadCookSolid(f32 w, f32 h, f32 x0, f32 y0, Color c) {
    // lays down two three-vertex triangles: T1 = [ urc->lrc->llc ] and T2 = [ llc->ulc->urc ]
    // ulc: upper-left corner (etc.)
    QuadHexaVertex qh = {};
    f32 x1 = x0 + w;
    f32 y1 = y0 + h;

    Vector2f ulc_pos { x0, y0 };
    Vector2f urc_pos { x1, y0 };
    Vector2f lrc_pos { x1, y1 };
    Vector2f llc_pos { x0, y1 };

    qh.verts[0] = InitQuadVertex( urc_pos, { 0, 0 }, c, 0 );
    qh.verts[1] = InitQuadVertex( lrc_pos, { 0, 0 }, c, 0 );
    qh.verts[2] = InitQuadVertex( llc_pos, { 0, 0 }, c, 0 );
    qh.verts[3] = InitQuadVertex( llc_pos, { 0, 0 }, c, 0 );
    qh.verts[4] = InitQuadVertex( ulc_pos, { 0, 0 }, c, 0 );
    qh.verts[5] = InitQuadVertex( urc_pos, { 0, 0 }, c, 0 );

    return qh;
}

QuadHexaVertex QuadCookTextured(Sprite s, s32 x0, s32 y0, u64 texture_id) {
    // lays down two three-vertex triangles: T1 = [ urc->lrc->llc ] and T2 = [ llc->ulc->urc ]
    // ulc: upper-left corner (etc.)

    QuadHexaVertex qh = {};
    s32 x1 = x0 + s.w;
    s32 y1 = y0 + s.h;

    Vector2f ulc_pos { (f32) x0, (f32) y0 };
    Vector2f urc_pos { (f32) x1, (f32) y0 };
    Vector2f lrc_pos { (f32) x1, (f32) y1 };
    Vector2f llc_pos { (f32) x0, (f32) y1 };

    Vector2f ulc_tex { (f32) s.u0, (f32) s.v0 };
    Vector2f urc_tex { (f32) s.u1, (f32) s.v0 };
    Vector2f lrc_tex { (f32) s.u1, (f32) s.v1 };
    Vector2f llc_tex { (f32) s.u0, (f32) s.v1 };

    Color no_color = { 0, 0, 0, 255 };

    qh.verts[0] = InitQuadVertex( urc_pos, urc_tex, no_color, texture_id );
    qh.verts[1] = InitQuadVertex( lrc_pos, lrc_tex, no_color, texture_id );
    qh.verts[2] = InitQuadVertex( llc_pos, llc_tex, no_color, texture_id );
    qh.verts[3] = InitQuadVertex( llc_pos, llc_tex, no_color, texture_id );
    qh.verts[4] = InitQuadVertex( ulc_pos, ulc_tex, no_color, texture_id );
    qh.verts[5] = InitQuadVertex( urc_pos, urc_tex, no_color, texture_id );

    return qh;
}

inline
QuadHexaVertex QuadOffset(QuadHexaVertex *q, s16 x, s16 y, Color color, u64 texture_id) {
    QuadHexaVertex out = {};
    for (u32 i = 0; i < 6; ++i) {
        QuadVertex v = *(q->verts + i);
        v.pos.x += x;
        v.pos.y += y;
        v.col = color;
        v.tex_id = texture_id;
        out.verts[i] = v;
    }
    return out;
}


//
//  SpriteMap


inline
Color SampleTextureRGBA(ImageRGBA *tex, f32 x, f32 y) {
    s32 i = (s32) round(tex->width * x);
    s32 j = (s32) round(tex->height * y);
    u32 idx = tex->width * j + i;
    Color b = tex->img[idx];
    return b;
}

inline
Color SampleTextureRGBASafe(ImageRGBA *tex, f32 x, f32 y, Color col_default) {
    s32 i = (s32) round(tex->width * x);
    s32 j = (s32) round(tex->height * y);
    if (i < 0 || i >= tex->width || j < 0 || j >= tex->height) {
        return col_default;
    }
    u32 idx = tex->width * j + i;
    Color b = tex->img[idx];
    return b;
}

void BlitSprite(Sprite s, s32 x0, s32 y0, ImageRGBA *img_dest, ImageRGBA *img_src) {
    s32 q_w = s.w;
    s32 q_h = s.h;
    s32 q_x0 = x0;
    s32 q_y0 = y0;

    assert(img_dest->height >= q_w);
    assert(img_dest->width >= q_h);

    u32 stride_img = img_dest->width;

    f32 q_scale_x = (s.u1 - s.u0) / q_w;
    f32 q_scale_y = (s.v1 - s.v0) / q_h;
    f32 q_u0 = s.u0;
    f32 q_v0 = s.v0;

    // i,j          : target coords
    // i_img, j_img : img coords

    for (s32 j = 0; j < q_h; ++j) {
        s32 j_img = j + q_y0;
        if (j_img < 0 || j_img > img_dest->height) {
            continue;
        }

        for (s32 i = 0; i < q_w; ++i) {
            s32 i_img = q_x0 + i;
            if (i_img < 0 || i_img > img_dest->width) {
                continue;
            }
            f32 x = q_u0 + i * q_scale_x;
            f32 y = q_v0 + j * q_scale_y;

            // TODO: how do we regularize this code?
            Color color_src = SampleTextureRGBASafe(img_src, x, y, Color { 0, 0, 0, 255 });

            if (color_src.a != 0) {
                // rudimentary alpha-blending
                s32 idx = j_img * stride_img + i_img;
                Color color_background = img_dest->img[idx];

                f32 alpha = (1.0f * color_src.a) / 255;
                Color color_blended;
                color_blended.r = (u8) (floor( alpha*color_src.r ) + floor( (1-alpha)*color_background.r ));
                color_blended.g = (u8) (floor( alpha*color_src.g ) + floor( (1-alpha)*color_background.g ));
                color_blended.b = (u8) (floor( alpha*color_src.b ) + floor( (1-alpha)*color_background.b ));
                color_blended.a = 255;

                img_dest->img[idx] = color_blended;
            }
        }
    }
}


struct SpriteMap {
    u32 size_tot;
    char map_name[32];
    char key_name[32];
    List<Sprite> sprites;
    ImageRGBA texture;

    u64 GetKey() {
        return HashStringValue(key_name);
    }
};


SpriteMap *SpriteMapLoadStream(u8 *base_ptr, u32 data_sz) {
    SpriteMap *smap = (SpriteMap*) base_ptr;
    smap->sprites.lst = (Sprite*) (base_ptr + sizeof(SpriteMap));
    smap->texture.img = (Color*) (base_ptr + sizeof(SpriteMap) + smap->sprites.len * sizeof(Sprite));

    assert(data_sz = sizeof(SpriteMap) + smap->sprites.len * sizeof(Sprite) + sizeof(Color) * smap->texture.width * smap->texture.height && "sanity check sprite map data size");
    return smap;
}

SpriteMap *CompileSpriteMapInline(MArena *a_dest, const char *name, const char *key_name, List<Sprite> sprites, List<u32> tex_keys, HashMap *texture_map) {
    s16 nx = (s16) floor( (f32) sqrt(sprites.len) );
    s32 ny = sprites.len / nx + 1;
    assert(nx >= 0 && sprites.len <= (u32) (nx * ny));

    printf("compiling sprite map;\n    name: %s\n    key_name: %s\n", name, key_name);
    printf("sprites count x: %u, y: %u \n", nx, ny);

    // calc bitmap size
    s32 bm_w = 0;
    s32 bm_h = 0;
    for (s32 j = 0; j < ny; ++j) {
        s32 row_w = 0;
        s32 row_max_h = 0;
        for (s32 i = 0; i < nx; ++i) {
            u32 idx = (u32) (i + j*nx);
            if (idx < sprites.len) {
                Sprite s = sprites.lst[idx];
                row_w += s.w;
                row_max_h = MaxS32(row_max_h, s.h);
            }
        }

        bm_w = MaxS32(bm_w, row_w);
        bm_h += row_max_h;
    }
    printf("comp. bitmap size: %d %d\n", bm_w, bm_h);

    // alloc sprite map memory
    SpriteMap *smap = (SpriteMap*) ArenaAlloc(a_dest, sizeof(SpriteMap));
    sprintf(smap->map_name, "%s", name);
    sprintf(smap->key_name, "%s", key_name);
    smap->sprites.len = sprites.len;
    smap->sprites.lst = (Sprite*) ArenaAlloc(a_dest, sizeof(Sprite) * sprites.len);
    smap->texture.width = bm_w;
    smap->texture.height = bm_h;
    smap->texture.img = (Color*) ArenaAlloc(a_dest, sizeof(Color) * bm_w * bm_h);
    smap->size_tot = sizeof(SpriteMap) + sizeof(Sprite) * sprites.len + sizeof(Color) * bm_w * bm_h;

    // copy data to sprites & texture
    s32 x = 0;
    s32 y = 0;
    for (s32 j = 0; j < ny; ++j) {
        s32 row_max_h = 0;
        for (s32 i = 0; i < nx; ++i) {
            u32 idx = (u32) (i + j*nx);
            if (idx < sprites.len) {
                Sprite s = sprites.lst[idx];
                ImageRGBA *texture = (ImageRGBA*) MapGet(texture_map, tex_keys.lst[idx]);
                BlitSprite(s, x, y, &smap->texture, texture);

                Sprite t = s;
                t.u0 = 1.0f * x / bm_w;
                t.u1 = (1.0f*x + t.w) / bm_w;
                t.v0 = 1.0f*y / bm_h;
                t.v1 = (1.0f*y + t.h) / bm_h;
                smap->sprites.lst[idx] = t;

                x += t.w;
                row_max_h = MaxS32(row_max_h, t.h);
            }
        }
        x = 0;
        y += row_max_h;

        printf("row_h: %d\n", row_max_h);
    }

    return smap;
}


//
//  DrawCall
//  Blit


enum DrawCallType {
    DCT_UNDEF,
    DCT_SOLID,
    DCT_TEXTURE_BYTE,
    DCT_TEXTURE_RGBA,
    DCT_CNT
};


struct DrawCall {
    DrawCallType tpe;
    u64 texture_key;
    List<QuadHexaVertex> quads;
};


static HashMap g_texture_map;
void *GetTexture(u64 key) {
    void *result = (void*) MapGet(&g_texture_map, key);
    return result;
}

inline
u8 SampleTexture(ImageB *tex, f32 x, f32 y) {
    u32 i = (s32) round(tex->width * x);
    u32 j = (s32) round(tex->height * y);
    u32 idx = tex->width * j + i;
    u8 b = tex->img[idx];
    return b;
}

void BlitQuads(Array<QuadHexaVertex> quads, ImageRGBA *img) {


    // TODO: Don't blit a drawcall, just blit the frekkin quads directly:
    //      We can do the type switch below from raw quad data (as OGL would).


    // get the texture


    for (u32 i = 0; i < quads.len; ++i) {
        QuadHexaVertex *q = quads.arr + i;

        s32 q_w = round( q->GetWidth() );
        s32 q_h = round( q->GetHeight() );
        s32 q_x0 = round( q->GetX0() );
        s32 q_y0 = round( q->GetY0() );
        u64 q_texture = q->GetTextureId();
        Color q_color = q->GetColor();

        assert(img->height >= q_h);
        assert(img->width >= q_w);

        u32 stride_img = img->width;

        void *texture = GetTexture(q_texture);
        ImageB *texture_b = (ImageB*) texture;
        ImageRGBA *texture_rgba = (ImageRGBA*) texture;


        //
        // byte-texture / glyphs
        //
        if (q_texture != 0 && q_color.IsNonZero()) {
            assert(texture_b != NULL);

            f32 q_scale_x = q->GetTextureScaleX(q_w);
            f32 q_scale_y = q->GetTextureScaleY(q_h);
            f32 q_u0 = q->GetTextureU0();
            f32 q_v0 = q->GetTextureV0();

            // i,j          : target coords
            // i_img, j_img : img coords

            for (s32 j = 0; j < q_h; ++j) {
                s32 j_img = j + q_y0;
                if (j_img < 0 || j_img > img->height) {
                    continue;
                }

                for (s32 i = 0; i < q_w; ++i) {
                    s32 i_img = q_x0 + i;
                    if (i_img < 0 || i_img > img->width) {
                        continue;
                    }
                    f32 x = q_u0 + i * q_scale_x;
                    f32 y = q_v0 + j * q_scale_y;
                    if (u8 alpha_byte = SampleTexture(texture_b, x, y)) {
                        // rudimentary alpha-blending
                        u32 idx = (u32) (j_img * stride_img + i_img);
                        Color color_background = img->img[idx];

                        f32 alpha = (1.0f * alpha_byte) / 255;
                        Color color_blended;
                        color_blended.r = (u8) (floor( alpha*q_color.r ) + floor( (1-alpha)*color_background.r ));
                        color_blended.g = (u8) (floor( alpha*q_color.g ) + floor( (1-alpha)*color_background.g ));
                        color_blended.b = (u8) (floor( alpha*q_color.b ) + floor( (1-alpha)*color_background.b ));
                        color_blended.a = 255;

                        img->img[idx] = color_blended;
                    }
                }
            }
        }

        //
        // mono-color quads
        //
        else if (q_texture == 0 && q_color.IsNonZero()) {
            s32 j_img;
            s32 i_img;
            u32 idx;
            for (s32 j = 0; j < q_h; ++j) {
                j_img = j + q_y0;
                if (j_img < 0 || j_img > img->height) {
                    continue;
                }

                for (s32 i = 0; i < q_w; ++i) {
                    i_img = q_x0 + i;
                    if (i_img < 0 || i_img > img->width) {
                        continue;
                    }

                    idx = j_img * stride_img + i_img;
                    img->img[idx] = q_color;
                }
            }
        }

        //
        // blit 32bit texture
        //
        else if (q_texture != 0 && q_color.IsZero()) {
            assert(texture_rgba != NULL);

            // TODO: integrate
            Sprite s = {};
            s.w = q_w;
            s.h = q_h;
            s.u0 = q->GetTextureU0();
            s.u1 = q->GetTextureU1();
            s.v0 = q->GetTextureV0();
            s.v1 = q->GetTextureV1();
            BlitSprite(s, q_x0, q_y0, img, texture_rgba);
        }
    }
}


//
// sprite render API (hides the drawcall buffer)


static Array<QuadHexaVertex> g_quad_buffer;
void QuadBufferInit(MArena *a_life, u32 max_quads = 2048) {
    g_quad_buffer = InitArray<QuadHexaVertex>(a_life, max_quads);
}

void SpriteRender_PushDrawCall(DrawCall dc) {
    // TODO: do something with this for OGL
}

void QuadBufferPush(QuadHexaVertex quad) {
    g_quad_buffer.Add(quad);
}

void QuadBufferBlitAndClear(ImageRGBA render_target) {
    BlitQuads(g_quad_buffer, &render_target);
    g_quad_buffer.len = 0;
}


#endif


#ifndef __RESOURCE_H__
#define __RESOURCE_H__


enum ResourceType {
    RST_FONT,
    RST_SPRITE,

    RST_CNT
};
void PrintResourceType(ResourceType tpe) {
    if (tpe == RST_FONT) {
        printf("font\n");
    }
    else if (tpe == RST_SPRITE) {
        printf("sprite map\n");
    }
    else {
        printf("_unknown_\n");
    }
}


struct ResourceHdr {
    ResourceType tpe;
    u32 data_sz;
    u32 next;
    char name[64];
    char key_name[64];

    ResourceHdr *GetInlinedNext() {
        if (next == 0) {
            return NULL;
        }
        ResourceHdr *nxt =  (ResourceHdr*) ((u8*) this + next);
        return nxt;
    }
    u8 *GetInlinedData() {
        u8 *dta =  (u8*) this + sizeof(ResourceHdr);
        assert( (dta + data_sz) == (u8*) GetInlinedNext() || GetInlinedNext() == NULL );

        return dta;
    }
};


struct ResourceStreamHandle {
    u32 cnt;
    u32 cnt_tpe[RST_CNT];
    StrLst *names[RST_CNT];
    StrLst *key_names[RST_CNT];

    ResourceHdr *first;
    ResourceHdr *prev;
    ResourceHdr *current;
};


// TODO: how do we put a bound on the number of resources in a file?
#define MAX_RESOURCE_CNT 128


ResourceStreamHandle ResourceStreamLoadAndOpen(MArena *a_tmp, MArena *a_dest, const char *filename, bool put_strs_inline = true) {

    ResourceStreamHandle hdl = {};
    hdl.first = (ResourceHdr *) LoadFileFSeek(a_dest, (char*) filename);
    if (hdl.first == NULL) {
        printf("Could not load file: '%s', exiting ...\n", filename);
        exit(0);
        return hdl;
    }
    HashMap map_names = InitMap(a_tmp, MAX_RESOURCE_CNT);
    HashMap map_keynames = InitMap(a_tmp, MAX_RESOURCE_CNT);

    ResourceHdr *res = hdl.first;

    StrSetArenas(a_dest, NULL);
    while (res) {
        assert(hdl.cnt < MAX_RESOURCE_CNT && "artificial resources load count limit reached");

        hdl.prev = res;
        hdl.cnt++;
        hdl.cnt_tpe[res->tpe]++;

        // check keyname uniqueness & record unique names and keynames
        u64 key = HashStringValue(res->key_name);
        if (MapGet(&map_keynames, key)) {
            assert( MapGet(&map_keynames, key) == 0 && "resource key duplicate");
        }
        if (put_strs_inline) {
            hdl.key_names[res->tpe] = StrLstPush(res->key_name, hdl.key_names[res->tpe]);
        }
        MapPut(&map_names, key, res);
        key = HashStringValue(res->name);
        if (MapGet(&map_names, key) == 0) {
            MapPut(&map_names, key, res);
            if (put_strs_inline) {
                hdl.names[res->tpe] = StrLstPush(res->name, hdl.names[res->tpe]);
            }
        }

        // check
        res->GetInlinedData();

        // iter
        res = res->GetInlinedNext();
    }
    StrPopArenas();

    printf("opened resource file '%s': %u entries (", filename, hdl.cnt);
    for (u32 i = 0; i < RST_CNT; ++i) {
        printf("%u", hdl.cnt_tpe[i]);
        if (i + 1 < RST_CNT) {
            printf(", ");
        }
        if (hdl.key_names[i]) {
            hdl.key_names[i] = hdl.key_names[i]->first;
            hdl.names[i] = hdl.names[i]->first;
        }
    }
    printf(")\n");

    return hdl;
}

void ResourceStreamPushData(MArena *a_dest, ResourceStreamHandle *stream, ResourceType tpe, char *name, char *key_name, void *data, u32 data_sz) {
    assert(stream != NULL);

    stream->current = (ResourceHdr*) ArenaAlloc(a_dest, sizeof(ResourceHdr));
    stream->current->tpe = tpe;
    stream->current->data_sz = data_sz;
    memcpy(stream->current->name, key_name, strlen(name));
    memcpy(stream->current->key_name, key_name, strlen(key_name));
    if (stream->prev) {
        stream->prev->next = (u32) ((u8*) stream->current - (u8*) stream->prev);
    }
    stream->prev = stream->current;
    if (stream->first == NULL) {
        stream->first = stream->current;
    }
    ArenaPush(a_dest, data, data_sz);
}

void ResourceStreamPushData(MArena *a_dest, ResourceStreamHandle *stream, ResourceType tpe, char *name, const char *key_name, void *data, u32 data_sz) {
    return ResourceStreamPushData(a_dest, stream, tpe, (char*) name, (char*) key_name, data, data_sz);
}

void ResourceStreamPushDataExtra(MArena *a_dest, ResourceStreamHandle *stream, void *data, u32 data_sz) {
    ArenaPush(a_dest, data, data_sz);
    stream->current->data_sz += data_sz;
}

void ResourceStreamSave(ResourceStreamHandle *stream, const char *filename = "all.res") {
    assert(stream->first != NULL);
    assert(stream->prev != NULL);

    void *data = stream->first;
    u32 last_sz = stream->prev->data_sz + sizeof(ResourceHdr);
    u32 data_sz = (u32) ((u8*) stream->prev - (u8*) stream->first + last_sz);

    SaveFile(filename, data, data_sz);
}


#endif


#ifndef __FONT_H__
#define __FONT_H__


//
//  FontAtlas


struct FontAtlas {
    ImageB texture;
    u32 sz_px;
    u32 cell_width;
    List<Sprite> glyphs;
    char font_name[32];
    char key_name[32];

    // previously known as da GLYPH PLOTTA !
    s32 ln_height;
    s32 ln_measured;
    s32 ln_ascend;
    s32 ln_descend;
    List<u8> advance_x;
    List<u8> x_lsb;
    List<s8> y_ascend;
    List<QuadHexaVertex> cooked;

    Sprite glyphs_mem[128];
    u8 advance_x_mem[128];
    u8 x_lsb_mem[128];
    s8 y_ascend_mem[128];
    QuadHexaVertex cooked_mem[128];

    s32 GetLineBaseOffset() {
        return ln_measured - ln_descend;
    }
    u64 GetKey() {
        return HashStringValue(key_name);
    }
    Str GetFontName() {
        return Str { this->font_name, (u32) strlen(this->font_name) };
    }
    void Print() {
        printf("font_sz %u, bitmap_sz %u %u, cell_w %u, ln_height %u, ln_ascend %u, glyphs %u, data ptrs %p %p\n", sz_px, texture.width, texture.height, cell_width, ln_height, ln_ascend, glyphs.len, glyphs.lst, texture.img);
    }
};

FontAtlas *FontAtlasLoadBinaryStream(u8 *base_ptr, u32 sz_data) {
    FontAtlas *atlas = (FontAtlas*) base_ptr;
    u32 sz_base = sizeof(FontAtlas);
    u32 sz_bitmap = atlas->texture.width * atlas->texture.height;

    assert(sz_data == sz_base + sz_bitmap && "sanity check data size");

    // set pointers
    atlas->glyphs = { atlas->glyphs_mem, 128 };
    atlas->texture.img = base_ptr + sz_base;
    atlas->advance_x = { &atlas->advance_x_mem[0], 128 };
    atlas->x_lsb = { &atlas->x_lsb_mem[0], 128 };
    atlas->y_ascend = { &atlas->y_ascend_mem[0], 128 };
    atlas->cooked = { &atlas->cooked_mem[0], 128 };

    return atlas;
};

FontAtlas *FontAtlasLoadBinary128(MArena *a_dest, char *filename, u32 *sz = NULL) {
    u64 sz_file;
    u8 *base_ptr = (u8*) LoadFileMMAP(filename, &sz_file);
    u32 sz_alloc = (u32) sz_file;
    base_ptr = (u8*) ArenaPush(a_dest, base_ptr, sz_alloc); // move to read-write memory location
    if (sz != NULL) {
        *sz = sz_alloc;
    }

    return FontAtlasLoadBinaryStream(base_ptr, sz_alloc);
};

void FontAtlasSaveBinary128(MArena *a_tmp, char *filename, FontAtlas atlas) {
    u32 sz_base = sizeof(FontAtlas);
    u32 sz_bitmap = atlas.texture.width * atlas.texture.height;

    FontAtlas *atlas_inlined = (FontAtlas*) ArenaPush(a_tmp, &atlas, sz_base);
    ArenaPush(a_tmp, atlas_inlined->texture.img, sz_bitmap);

    // invalidate pointers
    atlas_inlined->glyphs.lst = 0;
    atlas_inlined->advance_x.lst = 0;
    atlas_inlined->x_lsb.lst = 0;
    atlas_inlined->y_ascend.lst = 0;
    atlas_inlined->texture.img = 0;
    atlas_inlined->cooked.lst = 0;

    SaveFile(filename, (u8*) atlas_inlined, sz_base + sz_bitmap);
}

void GlyphPlotterPrint(FontAtlas *plt) {
    printf("ln_height: %d\n", plt->ln_height);
    printf("ln_ascend: %d\n", plt->ln_ascend);
    for (u32 i = 0; i - plt->advance_x.len; ++i) {
        u8 adv_x = plt->advance_x.lst[i];
        printf("%d ", adv_x);
    }
    printf("tex_w: %d\n", plt->texture.width);
    printf("tex_h: %d\n", plt->texture.height);
    printf("tex_ptr: %lu\n", (u64) plt->texture.img);
    printf("\n");
}


enum FontSize {
    FS_18,
    FS_24,
    FS_30,
    FS_36,
    FS_48,
    FS_60,
    FS_72,
    FS_84,

    FS_CNT,
};

u32 FontSizeToPx(FontSize font_size) {
    u32 sz_px = 0;
    switch (font_size) {
        case FS_18: sz_px = 18; break;
        case FS_24: sz_px = 24; break;
        case FS_30: sz_px = 30; break;
        case FS_36: sz_px = 36; break;
        case FS_48: sz_px = 48; break;
        case FS_60: sz_px = 60; break;
        case FS_72: sz_px = 72; break;
        case FS_84: sz_px = 84; break;
        default: break;
    }
    return sz_px;
}

FontSize FontSizeFromPx(u32 sz_px) {
    FontSize fs = FS_18;
    switch (sz_px) {
        case 18 : fs = FS_18; break;
        case 24 : fs = FS_24; break;
        case 30 : fs = FS_30; break;
        case 36 : fs = FS_36; break;
        case 48 : fs = FS_48; break;
        case 60 : fs = FS_60; break;
        case 72 : fs = FS_72; break;
        case 84 : fs = FS_84; break;
        default: break;
    }
    return fs;
}


//
//  Font related globals

static HashMap g_resource_map;
static StrLst *g_font_names;
static FontAtlas *g_text_plotter;


FontAtlas *SetFontAndSize(FontSize font_size, Str font_name) {
    // font name
    font_name = StrCat(font_name, "_");

    // size
    char buff[8];
    sprintf(buff, "%.2u", FontSizeToPx(font_size));
    Str key_name = StrCat(font_name, StrL(buff));

    // get by key
    u64 key = HashStringValue(StrZ(key_name));
    u64 val = MapGet(&g_resource_map, key);
    g_text_plotter = (FontAtlas*) val;
    return g_text_plotter;
}

FontAtlas *SetFont(Str font_name) {
    assert(g_text_plotter != NULL);

    return SetFontAndSize( FontSizeFromPx(g_text_plotter->sz_px), font_name);
}

FontAtlas *SetFontSize(FontSize font_size) {
    assert(g_text_plotter != NULL);

    return SetFontAndSize( font_size, g_text_plotter->GetFontName());
}

FontSize GetFontSize() {
    s32 sz_px = g_text_plotter->sz_px;
    switch (sz_px) {
        case 18: return FS_18; break;
        case 24: return FS_24; break;
        case 30: return FS_30; break;
        case 36: return FS_36; break;
        case 48: return FS_48; break;
        case 60: return FS_60; break;
        case 72: return FS_72; break;
        case 84: return FS_84; break;
        default: return FS_CNT;
    }
}

static FontSize g_font_sz_default;
FontSize GetDefaultFontSize() {
    return g_font_sz_default;
}
void SetDefaultFontSize(FontSize fs) {
    g_font_sz_default = fs;
}


//
//  Text layout
//


// TODO: export helper functions to string.h


inline
bool IsWhiteSpace(char c) {
    bool result = (c == ' ' || c == '\n' || c == '\t');
    return result;
}
inline
bool IsWhiteSpace(Str s) {
    assert(s.len > 0);
    return IsWhiteSpace(s.str[0]);
}
inline
bool IsNewLine(char c) {
    bool result = (c == '\n');
    return result;
}
inline
bool IsNewLine(Str s) {
    assert(s.len > 0);
    return IsNewLine(s.str[0]);
}
inline
bool IsAscii(char c) {
    bool result = c >= 0 && c < 128;
    return result;
}
inline
Str StrInc(Str s, u32 inc) {
    assert(inc <= s.len);
    s.str += inc;
    s.len -= inc;
    return s;
}


inline
void ScaleTextInline(List<QuadHexaVertex> text, f32 scale, s32 x0, s32 y0, s32 w, s32 h) {
    if (scale != 1.0f) {
        for (u32 i = 0; i < text.len; ++i) {
            QuadHexaVertex *q = text.lst + i;

            for (u32 j = 0; j < 6; ++j) {
                Vector2f *pos = &(q->verts + j)->pos;
                pos->x = x0 + (pos->x - x0) * scale;
                pos->y = y0 + (pos->y - y0) * scale;
            }
        }
    }
}
inline
u32 WorldLen(Str s, List<u8> advance_x, s32 *w_adv) {
    u32 i = 0;
    *w_adv = 0;
    while (i < s.len && IsWhiteSpace(s.str[i]) == false) {
        *w_adv += advance_x.lst[s.str[i]];
        ++i;
    }
    return i;
}
inline
bool CanDoNewline(s32 pt_y, s32 ln_height, s32 ln_ascend, s32 ln_limit_y) {
    bool result = false;
    if (pt_y + ln_ascend + 2 * ln_height < ln_limit_y) {
        result = true;
    }
    return result;
}
inline
bool CanDoWhiteSpace(s32 pt_x, s32 w_space, s32 ln_limit_x) {
    bool result = pt_x + w_space <= ln_limit_x;
    return result;
}
inline
void DoNewLine(s32 ln_height, s32 left, s32 *pt_x, s32 *pt_y) {
    *pt_y += ln_height;
    *pt_x = left;
}
inline
void DoWhiteSpace(s32 space_width, s32 *pt_x) {
    *pt_x += space_width;
}


enum TextAlign {
    TAL_LEFT,
    TAL_CENTER,
    TAL_RIGHT,

    TAL_CNT,
};
inline
void AlignQuadsH(List<QuadHexaVertex> line_quads, s32 cx, TextAlign ta) {
    // TODO: re-impl.
    /*
    if (ta != TAL_LEFT && line_quads.len > 0) {
        QuadHexaVertex *ql = line_quads.lst;
        QuadHexaVertex *qr = line_quads.LastPtr();
        s32 line_c = (qr->GetX1() + ql->GetX0()) / 2;

        s32 offset_x = cx - line_c;
        if (ta == TAL_RIGHT) {
            offset_x *= 2;
        }
        else {
            assert(ta == TAL_CENTER);
        }

        for (u32 i = 0; i < line_quads.len; ++i) {
            QuadOffset(line_quads.lst + i, (f32) offset_x, 0.0f);
        }
    }
    */
}


List<QuadHexaVertex> LayoutTextAutowrap(MArena *a_dest, FontAtlas *plt, Str txt, s32 x0, s32 y0, s32 w, s32 h, Color color, TextAlign ta) {
    assert(g_text_plotter != NULL && "init text plotters first");


    //
    // TODO: un-retire this autowrap function
    //


    s32 pt_x = x0;
    s32 pt_y = y0;
    s32 box_r = x0 + w;
    s32 box_b = y0 + h;
    s32 w_space = plt->advance_x.lst[' '];

    u32 i = 0;
    Str s = txt;

    List<QuadHexaVertex> quads = InitList<QuadHexaVertex>(a_dest, 0);
    u32 line_first_idx = 0;
    u32 line_len = 0;
    while (s.len > 0) {
        // while words

        if (IsNewLine(s)) {
            if (CanDoNewline(pt_y, plt->ln_height, plt->ln_ascend, box_b)) {
                DoNewLine(plt->ln_height, x0, &pt_x, &pt_y);

                AlignQuadsH(List<QuadHexaVertex> { quads.lst + line_first_idx, line_len }, x0 + w / 2, ta);
                line_len = 0;
                line_first_idx = quads.len;
            }
            s = StrInc(s, 1);
        }
        if (IsWhiteSpace(s)) {
            if (CanDoWhiteSpace(pt_x, w_space, box_r)) {
                DoWhiteSpace(w_space, &pt_x);
            }
            s = StrInc(s, 1);
        }

        // lookahead word len (include leading whitespace)
        s32 w_adv = 0;
        u32 w_len = WorldLen(s, plt->advance_x, &w_adv);

        // word wrap
        if (pt_x + w_adv > box_r) {
            if (CanDoNewline(pt_y, plt->ln_height, plt->ln_ascend, box_b)) {
                DoNewLine(plt->ln_height, x0, &pt_x, &pt_y);
            
                AlignQuadsH(List<QuadHexaVertex> { quads.lst + line_first_idx, line_len }, x0 + w / 2, ta);
                line_len = 0;
                line_first_idx = quads.len;
            }
            else {
                // ran out of space, exit
                break;
            }
        }

        // lay out word
        for (u32 j = 0; j < w_len; ++j) {
            char c = s.str[j];

            // TODO: re-impl.
            //QuadHexaVertex q = QuadOffset(plt->cooked.lst + c, pt_x, pt_y, color);
            QuadHexaVertex q = {};

            pt_x += plt->advance_x.lst[c];
            ArenaAlloc(a_dest, sizeof(QuadHexaVertex));
            quads.Add(q);

            line_len++;
        }
        s = StrInc(s, w_len);

        // dbg count
        ++i;
    }
    assert(quads.len <= txt.len); // quad len equals char count minus whitespaces

    // align the last line of the batch
    AlignQuadsH(List<QuadHexaVertex> { quads.lst + line_first_idx, line_len }, x0 + w / 2, ta);

    // only scale if absolutely necessary
    f32 scale = 1.0f;
    if (scale != 1) {
        ScaleTextInline(quads, scale, x0, y0, w, h);
    }

    // TODO: update this hack to be more organized -> e.g. put assembling the drawcall outside of
    //      this function somehow, maybe in the UI_xxx calls.
    DrawCall dc = {};
    dc.tpe = DCT_TEXTURE_BYTE;
    dc.texture_key = plt->GetKey();
    dc.quads = quads;
    SpriteRender_PushDrawCall(dc);

    return quads;
}

s32 TextLineWidth(FontAtlas *plt, Str txt) {
    s32 pt_x = 0;
    s32 w_space = plt->advance_x.lst[' '];

    for (u32 i = 0; i < txt.len; ++i) {
        // while words
        char c = txt.str[i];

        if (c == ' ') {
            pt_x += w_space;
            continue;
        }
        if (IsAscii(c) == false) {
            continue;
        }

        pt_x += plt->advance_x.lst[c];
    }

    return pt_x;
}
s32 TextLineHeight(FontAtlas *plt) {
    return plt->ln_measured;
}

void TextPositionLine(Str txt, s32 box_l, s32 box_t, s32 box_w, s32 box_h, s32 align_horiz, s32 align_vert, s32 *txt_l, s32 *txt_t, s32 *txt_w, s32 *txt_h) {
    FontAtlas *plt = g_text_plotter;

    *txt_w = 0;
    *txt_h = plt->ln_measured; // single-line height

    s32 w_space = plt->advance_x.lst[' '];
    for (u32 i = 0; i < txt.len; ++i) {

        char c = txt.str[i];
        if (c == ' ') {
            *txt_w += w_space;
            continue;
        }
        if (IsAscii(c) == false) {
            continue;
        }

        *txt_w += plt->advance_x.lst[c];
    }

    s32 box_x = box_l + box_w / 2;
    s32 box_y = box_t + box_h / 2;

    // text rect center
    s32 txt_x;
    s32 txt_y;

    if (align_horiz == 0) { // alight horizontal center
        txt_x = box_x;
    }
    else if (align_horiz > 0) { // alight horizontal left
        txt_x = box_l + *txt_w / 2;
    }
    else if (align_horiz < 0) { // alight horizontal right
        s32 box_r = box_l + box_w;
        txt_x = box_r - *txt_w / 2;
    }
    if (align_vert == 0) { // alight vertical center
        txt_y = box_y;
    }
    else if (align_vert > 0) { // alight vertical left
        txt_y = box_l + *txt_w / 2;
    }
    else if (align_vert < 0) { // alight vertical right
        s32 box_b = box_t + box_h;
        txt_y = box_b - *txt_h / 2;
    }

    *txt_l = txt_x - *txt_w / 2;
    *txt_t = txt_y - *txt_h / 2;
}


// TODO: use floats for character rendering

void TextPlot(Str txt, s32 box_l, s32 box_t, s32 box_w, s32 box_h, s32 *sz_x, s32 *sz_y, Color color) {
    assert(g_text_plotter != NULL && "init text plotters first");
    FontAtlas *plt = g_text_plotter;

    s32 txt_l;
    s32 txt_t;
    TextPositionLine(txt, box_l, box_t, box_w, box_h, 0, 0, &txt_l, &txt_t, sz_x, sz_y);

    // position the quads
    s32 pt_x = txt_l;
    s32 pt_y = txt_t + plt->GetLineBaseOffset();
    s32 w_space = plt->advance_x.lst[' '];
    u64 plt_key = plt->GetKey();

    for (u32 i = 0; i < txt.len; ++i) {
        char c = txt.str[i];

        if (c == ' ') {
            pt_x += w_space;
            continue;
        }
        if (IsAscii(c) == false) {
            continue;
        }

        QuadHexaVertex q = QuadOffset(plt->cooked.lst + c, pt_x, pt_y, color, plt_key);
        pt_x += plt->advance_x.lst[c];
        QuadBufferPush(q);
    }
}


#endif


#ifndef __IMUI_H__
#define __IMUI_H__


//
//  UI Panel quad layout
//


void PanelPlot( f32 l, f32 t, f32 w, f32 h, f32 thic_border, Color col_border = { RGBA_GRAY_75 }, Color col_pnl = { RGBA_WHITE } )
{
    if (thic_border >= w / 2 || thic_border >= w / 2) {
        return;
    }

    QuadBufferPush(QuadCookSolid(w, h, l, t, col_border));
    QuadBufferPush(QuadCookSolid(w - 2*thic_border, h - 2*thic_border, l + thic_border, t + thic_border, col_pnl));
}


//
//  IMUI system experiments
//


//
//  Tree structure is built every turn
//  
//  How the tree structure links:
//      - siblings are iterated by next
//      - sub-branches are created from a node using first
//      - all nodes (except root) have parent set
//


struct CollRect {
    s32 x0;
    s32 x1;
    s32 y0;
    s32 y1;

    inline
    bool DidCollide(s32 x, s32 y) {
        bool bx = (x >= x0 && x <= x1);
        bool by = (y >= y0 && y <= y1);
        return bx && by;
    }
};


enum WidgetAlignmentFLags {
    WA_PASSIVE = 0,

    WA_TOP_LEFT = 1 << 0,
    WA_TOP_RIGHT = 1 << 1,
    WA_BOTTOM_LEFT = 1 << 2,
    WA_BOTTOM_RIGHT = 1 << 3,

    WA_CENTV_LEFT = 1 << 4,
    WA_CENTV_RIGHT = 1 << 5,
    WA_TOP_CENTH = 1 << 6,
    WA_BOTTOM_CENTH = 1 << 7,

    WA_CENTER = 1 << 8
};


enum WidgetFlags {
    WF_PASSIVE = 0,

    WF_DRAW_BACKGROUND_AND_BORDER = 1 << 0,
    WF_DRAW_TEXT = 1 << 1,
    WF_CAN_COLLIDE = 1 << 2,

    WF_LAYOUT_CENTER = 1 << 10,
    WF_LAYOUT_HORIZONTAL = 1 << 11,
    WF_LAYOUT_VERTICAL = 1 << 12,
    WF_ALIGN_LEFT = 1 << 13,
    WF_ALIGN_RIGHT = 1 << 14,
    WF_ALIGN_CENTER = 1 << 15,

    WF_EXPAND_HORIZONTAL = 1 << 16,
    WF_EXPAND_VERTICAL = 1 << 17,

    WF_ABSREL_POSITION = 1 << 20
};
bool WidgetIsLayout(u32 features) {
    bool result =
        features & WF_LAYOUT_CENTER ||
        features & WF_LAYOUT_HORIZONTAL ||
        features & WF_LAYOUT_VERTICAL ||
    false;
    return result;
}


struct Widget {
    Widget *next;       // sibling in the branch
    Widget *first;      // child sub-branch first
    Widget *parent;     // parent of the branch

    u64 hash_key;       // hash for frame-boundary persistence
    u64 frame_touched;  // expiration date

    f32 x0;
    f32 y0;
    f32 w;
    f32 h;
    f32 w_max;
    f32 h_max;

    Str text;
    FontSize sz_font;
    s32 sz_border;
    Color col_bckgrnd;
    Color col_text;
    Color col_border;

    // DBG / experimental
    Color col_hot;
    Color col_active;
    bool hot;
    bool active;
    bool clicked;

    u32 features_flg;
    u32 alignment_flg;

    // everything below belongs in the layout algorithm
    CollRect rect;

    void CollRectClear() {
        rect = {};
    }
    void SetCollisionRectUsingX0Y0WH() {
        rect.x0 = x0;
        rect.x1 = x0 + w;
        rect.y0 = y0;
        rect.y1 = y0 + h;
    }
    void SetFeature(WidgetFlags f) {
        features_flg = features_flg |= f;
    }
};

void WidgetSetFlag(Widget *wgt, u32 flag) {
    wgt->features_flg |= flag;
}


//
//  Core


static MArena _g_a_imui;
static MArena *g_a_imui;
static MPoolT<Widget> _g_p_widgets;
static MPoolT<Widget> *g_p_widgets;
static Stack<Widget*> _g_s_widgets;
static Stack<Widget*> *g_s_widgets;

static HashMap _g_m_widgets;
static HashMap *g_m_widgets;

static Widget _g_w_root;
static Widget *g_w_layout;
static Widget *g_w_active;

static u64 *g_frameno_imui;


void TreeSibling(Widget *w) {
    if (g_w_layout->first != NULL) {
        Widget *sib = g_w_layout->first;
        while (sib->next != NULL) {
            sib = sib->next;
        }
        sib->next = w;
        w->parent = sib->parent;
    }
    else {
        g_w_layout->first = w;
        w->parent = g_w_layout;
    }
}

void TreeBranch(Widget *w) {
    if (g_w_layout->first != NULL) {
        Widget *sib = g_w_layout->first;
        while (sib->next != NULL) {
            sib = sib->next;
        }
        sib->next = w;
    }
    else {
        g_w_layout->first = w;
    }
    w->parent = g_w_layout;
    g_w_layout = w;
}

void TreePop() {
    Widget *parent = g_w_layout->parent;
    if (parent != NULL) {
        g_w_layout = parent;
    }
}


static f32 g_mouse_x;
static f32 g_mouse_y;
static bool g_mouse_down;
static bool g_mouse_pushed;


void InitImUi(u32 width, u32 height, u64 *frameno) {
    if (g_a_imui != NULL) {
        assert(1 == 0 && "don't re-initialize imui");
    }
    else {
        g_frameno_imui = frameno;

        MArena _g_a_imui = ArenaCreate();
        g_a_imui = &_g_a_imui;

        u32 max_widgets = 1024;
        _g_p_widgets = PoolCreate<Widget>(max_widgets);
        g_p_widgets = &_g_p_widgets;

        _g_s_widgets = InitStack<Widget*>(g_a_imui, max_widgets);
        g_s_widgets = &_g_s_widgets;

        _g_m_widgets = InitMap(g_a_imui, max_widgets);
        g_m_widgets = &_g_m_widgets;

        _g_w_root = {};
        _g_w_root.features_flg |= WF_LAYOUT_HORIZONTAL;
        _g_w_root.w_max = width;
        _g_w_root.h_max = height,
        _g_w_root.x0 = 0;
        _g_w_root.y0 = 0;

        g_w_layout = &_g_w_root;
    }

    SetDefaultFontSize(FS_24);
}


void WidgetTreeSizeWrap_Rec(Widget *w, s32 *w_sum, s32 *h_sum, s32 *w_max, s32 *h_max) {
    // Recursively determines widget sizes by wrapping in child widgets. 
    // Sizes will be the minimal, and expander sizes will be expanded elsewhere.

    // There is an accumulated child size and a max child size.
    // Depending on the layou of the current widget, its actual size
    // is set to either the maximum child widget.
    // Or, if a panel has the WF_LAYOUT_H or WR_LAYOUT_V features, the sum of
    // each child's actual size.
    //
    // max & sum sizes are determined on descent, actual sizes are set on ascent.


    //
    // Descent: determine child_max and child_sum sizes


    *w_sum = 0;
    *h_sum = 0;
    *w_max = 0;
    *h_max = 0;

    Widget *ch = w->first;
    while (ch != NULL) { // iterate child widgets
        s32 w_sum_ch;
        s32 h_sum_ch;
        s32 w_max_ch;
        s32 h_max_ch;

        WidgetTreeSizeWrap_Rec(ch, &w_sum_ch, &h_sum_ch, &w_max_ch, &h_max_ch);

        *w_sum += ch->w;
        *h_sum += ch->h;
        *w_max = MaxS32(*w_max, ch->w);
        *h_max = MaxS32(*h_max, ch->h);

        ch = ch->next;
    }


    //
    // Ascent: Assign actual size to current widget (where undefined)


    if (w->w == 0 && w->h == 0) {
        if (w->features_flg & WF_LAYOUT_CENTER) {
            w->w = *w_max;
            w->h = *h_max;
        }
        if (w->features_flg & WF_LAYOUT_HORIZONTAL) {
            w->w = *w_sum;
            w->h = *h_max;
        }
        else if (w->features_flg & WF_LAYOUT_VERTICAL) {
            w->w = *w_max;
            w->h = *h_sum;
        }
    }

    // or keep pre-sets
    else {
        *w_sum = w->w;
        *h_sum = w->h;
        *w_max = w->w;
        *h_max = w->h;
    }
}


void WidgetTreeExpand_Rec(Widget *w) {
    // expands one sub-widget using own dimensions

    Widget *ch = w->first;
    if (ch == NULL) {
        return;
    }

    // extract info
    bool expander_vert_found = false;
    bool expander_horiz_found = false;
    u32 width_other = 0;
    u32 height_other = 0;
    while (ch) {
        if ( ! (ch->features_flg & WF_EXPAND_VERTICAL)) {
            height_other += ch->h;
        }
        else {
            assert(w->features_flg & WF_LAYOUT_HORIZONTAL || expander_vert_found == false && "Expander not allowed (WF_EXPAND_VERTICAL)");
            expander_vert_found = true;
        }

        if ( ! (ch->features_flg & WF_EXPAND_HORIZONTAL)) {
            width_other += ch->w;
        }
        else {
            assert(w->features_flg & WF_LAYOUT_VERTICAL || expander_horiz_found == false && "Expander not allowed (WF_EXPAND_HORIZONTAL)");
            expander_horiz_found = true;
        }

        ch = ch->next;
    }

    // assign max possible size to the expander(s)
    ch = w->first;
    while (ch) {
        if (ch->features_flg & WF_EXPAND_VERTICAL) {
            ch->h = w->h - height_other;
        }
        if (ch->features_flg & WF_EXPAND_HORIZONTAL) {
            ch->w = w->w - width_other;
        }
        ch = ch->next;
    }

    // descend
    ch = w->first;
    while (ch) {
        WidgetTreeExpand_Rec(ch);

        ch = ch->next;
    }
}


List<Widget*> WidgetTreePositioning(MArena *a_tmp, Widget *w_root) {
    List<Widget*> all_widgets = InitList<Widget*>(a_tmp, 0);
    Widget *w = w_root;


    while (w != NULL) {
        ArenaAlloc(a_tmp, sizeof(Widget*));
        all_widgets.Add(w);

        s32 pt_x = 0;
        s32 pt_y = 0;

        // with all widget sizes known, widgets can position their children
        Widget *ch = w->first;
        while (ch != NULL) { // iterate child widgets

            // set child position - if not absolutely positioned
            if ((ch->features_flg & WF_ABSREL_POSITION) == false) {

                ch->x0 = w->x0;
                ch->y0 = w->y0;

                if (w->features_flg & WF_LAYOUT_CENTER) {
                    ch->y0 = w->y0 + (w->h - ch->h) / 2;
                    ch->x0 = w->x0 + (w->w - ch->w) / 2;
                }

                else if (w->features_flg & WF_LAYOUT_HORIZONTAL) {
                    ch->x0 = w->x0 + pt_x;
                    pt_x += ch->w;

                    if (w->features_flg & WF_ALIGN_CENTER) {
                        ch->y0 = w->y0 + (w->h - ch->h) / 2;
                    }
                    else if (w->features_flg & WF_ALIGN_RIGHT) {
                        ch->y0 = w->y0 + (w->h - ch->h);
                    }
                }

                else if (w->features_flg & WF_LAYOUT_VERTICAL) {
                    ch->y0 = w->y0 + pt_y;
                    pt_y += ch->h;

                    if (w->features_flg & WF_ALIGN_CENTER) {
                        ch->x0 = w->x0 + (w->w - ch->w) / 2;
                    }
                    else if (w->features_flg & WF_ALIGN_RIGHT) {
                        ch->x0 = w->x0 + (w->w - ch->w);
                    }
                }
            }

            if (ch->features_flg & WF_ABSREL_POSITION) {
                // basic offset wrt. parent
                if (ch->alignment_flg == 0) {
                    ch->x0 += w->x0;
                    ch->y0 += w->y0;
                }

                // alignment also specified
                else if (ch->alignment_flg & WA_TOP_LEFT) {
                    ch->x0 = w->x0 + ch->x0;
                    ch->y0 = w->y0 + ch->y0;
                }
                else if (ch->alignment_flg & WA_TOP_RIGHT) {
                    ch->x0 = w->x0 + ch->x0 + w->w - ch->w;
                    ch->y0 = w->y0 + ch->y0;
                }
                else if (ch->alignment_flg & WA_BOTTOM_LEFT) {
                    ch->x0 = w->x0 + ch->x0;
                    ch->y0 = w->y0 + ch->y0 + w->h - ch->h;
                }
                else if (ch->alignment_flg & WA_BOTTOM_RIGHT) {
                    ch->x0 = w->x0 + ch->x0 + w->w - ch->w;
                    ch->y0 = w->y0 + ch->y0 + w->h - ch->h;
                }
                else if (ch->alignment_flg & WA_CENTV_LEFT) {
                    ch->x0 = w->x0 + ch->x0;
                    ch->y0 = w->y0 + ch->y0 + (w->h - ch->h) / 2;
                }
                else if (ch->alignment_flg & WA_CENTV_RIGHT) {
                    ch->x0 = w->x0 + ch->x0 + w->w - ch->w;
                    ch->y0 = w->y0 + ch->y0 + (w->h - ch->h) / 2;
                }
                else if (ch->alignment_flg & WA_TOP_CENTH) {
                    ch->x0 = w->x0 + ch->x0 + (w->w - ch->w) / 2;
                    ch->y0 = w->y0 + ch->y0;
                }
                else if (ch->alignment_flg & WA_BOTTOM_CENTH) {
                    ch->x0 = w->x0 + ch->x0 + (w->w - ch->w) / 2;
                    ch->y0 = w->y0 + ch->y0 + w->h - ch->h;
                }
                else if (ch->alignment_flg & WA_CENTER) {
                    ch->x0 = w->x0 + ch->x0 + (w->w - ch->w) / 2;
                    ch->y0 = w->y0 + ch->y0 + (w->h - ch->h) / 2;
                }
            }


            // set the collision rect for next frame code-interleaved mouse collision
            ch->SetCollisionRectUsingX0Y0WH();

            ch->hot = false;
            ch->clicked = false;

            if (ch->features_flg & WF_CAN_COLLIDE) {
                if (ch->rect.DidCollide( (s32) g_mouse_x, (s32) g_mouse_y )) {
                    if (g_w_active == NULL) {
                        ch->hot = true;
                        if (g_mouse_down) {
                            // enable active mode
                            g_w_active = ch;
                            ch->active = true;
                        }
                    }
                    else {
                        // disable active mode
                        ch->active = false;
                    }
                }
                else {
                    ch->hot = false;
                }

                if (g_mouse_down == false) {
                    ch->active = false;
                    g_w_active = NULL;
                }

                if (g_mouse_pushed && ch->active) {
                    ch->clicked = true;
                }
            }

            // iter
            ch = ch->next;
        }

        // iter
        if (w->first != NULL) {
            if (w->next) {
                g_s_widgets->Push(w->next);
            }
            w = w->first;
        }
        else if (w->next) {
            w = w->next;
        }
        else {
            w = g_s_widgets->Pop();
        }
    }

    return all_widgets;
}


void WidgetTreeRenderToDrawcalls(List<Widget*> all_widgets) {


    // TODO: This should iterate the tree, and do something meaningful to figure out
    //      what should be rendered on top of what - proper interleaving of calls.


    for (u32 i = 0; i < all_widgets.len; ++i) {
        Widget *w = all_widgets.lst[i];

        if (w->features_flg & WF_DRAW_BACKGROUND_AND_BORDER) {
            PanelPlot(w->x0, w->y0, w->w, w->h, w->sz_border, w->col_border, w->col_bckgrnd);
        }

        if (w->features_flg & WF_DRAW_TEXT) {
            SetFontSize(w->sz_font);
            s32 w_out;
            s32 h_out;

            TextPlot(w->text, w->x0, w->y0, w->w, w->h, &w_out, &h_out, w->col_text);
        }
    }
}


void UI_FrameEnd(MArena *a_tmp, s32 width, s32 height) {
    if (g_mouse_down == false) {
        g_w_active = NULL;
    }

    Widget *w = &_g_w_root;
    w->w_max = width;
    w->h_max = height;
    w->w = w->w_max;
    w->h = w->h_max;

    // size widgets to wrap tightly
    s32 w_sum_ch;
    s32 h_sum_ch;
    s32 w_max_ch;
    s32 h_max_ch;
    WidgetTreeSizeWrap_Rec(w, &w_sum_ch, &h_sum_ch, &w_max_ch, &h_max_ch);

    // size pass
    WidgetTreeExpand_Rec(w);

    // position pass
    List<Widget*> all_widgets = WidgetTreePositioning(a_tmp, w);

    // render pass
    WidgetTreeRenderToDrawcalls(all_widgets);

    // clean up pass
    _g_w_root.frame_touched = *g_frameno_imui;
    g_w_layout = &_g_w_root;
    for (u32 i = 0; i < all_widgets.len; ++i) {
        Widget *w = all_widgets.lst[i];

        // prune
        if (w->frame_touched < *g_frameno_imui) {
            MapRemove(g_m_widgets, w->hash_key, w); 
            g_p_widgets->Free(w);
        }
        // clean
        else {
            if (w->hash_key != 0) {
                MapPut(g_m_widgets, w->hash_key, w);
            }
            w->parent = NULL;
            w->first = NULL;
            w->next = NULL;
        }
    }
}


//
//  Builder API


Widget *WidgetGetCached(const char *text, bool *was_new = NULL) {
    u64 key = HashStringValue(text);
    Widget *w = (Widget*) MapGet(g_m_widgets, key);

    if (w == NULL) {
        w = g_p_widgets->Alloc();
        MapPut(g_m_widgets, key, w);
        w->hash_key = key;

        w->text = Str { (char*) text, (u32) strlen( (char*) text) };
        if (was_new) *was_new = true;
    }
    else {
        assert(key == w->hash_key);
        assert(w->frame_touched != *g_frameno_imui);
        if (was_new) *was_new = false;
    }

    w->frame_touched = *g_frameno_imui;

    return w;
}

Widget *WidgetGetNew(const char *text = NULL) {
    Widget *w = g_p_widgets->Alloc();
    assert(w->frame_touched == 0);
    if (text) {
        w->text = Str { (char*) text, (u32) strlen( (char*) text) };
    }

    return w;
}


bool UI_Button(const char *text, Widget **w_out = NULL) {
    Widget *w  = WidgetGetCached(text);
    w->features_flg |= WF_DRAW_TEXT;
    w->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
    w->features_flg |= WF_CAN_COLLIDE;
    w->w = 120;
    w->h = 50;
    w->sz_font = FS_24;

    if (w->active) {
        w->sz_border = 3;
        w->col_bckgrnd = COLOR_GRAY_75;
        w->col_text = COLOR_BLACK;
        w->col_border = COLOR_BLACK;
    }
    else if (w->hot) {
        w->sz_border = 3;
        w->col_bckgrnd = COLOR_WHITE;
        w->col_text = COLOR_BLACK;
        w->col_border = COLOR_BLACK;
    }
    else {
        w->sz_border = 1;
        w->col_bckgrnd = COLOR_WHITE;
        w->col_text = COLOR_BLACK;
        w->col_border = COLOR_BLACK;
    }

    TreeSibling(w);

    if (w_out != NULL) {
        *w_out = w;
    }
    return w->clicked;
}


bool UI_ToggleButton(const char *text, bool *state, Widget **w_out = NULL) {
    Widget *w  = WidgetGetCached(text);
    w->features_flg |= WF_DRAW_TEXT;
    w->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
    w->features_flg |= WF_CAN_COLLIDE;
    w->w = 120;
    w->h = 50;
    w->sz_font = FS_24;

    if (w->clicked) {
        *state = !(*state);
    }

    if (*state == true) {
        w->sz_border = 3;
        w->col_bckgrnd = ColorGray(0.8f);
        w->col_text = ColorBlack();
        w->col_border = ColorBlack();
    }
    else if (w->hot) {
        w->sz_border = 3;
        w->col_bckgrnd = ColorWhite();
        w->col_text = ColorBlack();
        w->col_border = ColorBlack();
    }
    else {
        w->sz_border = 1;
        w->col_bckgrnd = COLOR_WHITE;
        w->col_text = ColorBlack();
        w->col_border = ColorBlack();
    }

    TreeSibling(w);

    if (w_out != NULL) {
        *w_out = w;
    }
    return w->clicked;
}


Widget *UI_CoolPanel(s32 width, s32 height, bool center_h = true) {
    Widget *w = WidgetGetNew();
    w->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
    w->features_flg |= WF_LAYOUT_VERTICAL;
    if (center_h) {
        w->features_flg |= WF_ALIGN_CENTER;
    }
    w->w = width;
    w->h = height;
    w->sz_border = 20;
    w->col_bckgrnd = ColorGray(0.9f);
    w->col_border = ColorGray(0.7f);

    TreeBranch(w);

    return w;
}


bool UI_CrossButton(const char *symbol, Widget **w_out = NULL) {
    Widget *x = WidgetGetCached(symbol);
    if (w_out) *w_out = x;

    x->features_flg |= WF_ABSREL_POSITION;
    x->features_flg |= WF_DRAW_TEXT;
    x->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
    x->features_flg |= WF_CAN_COLLIDE;
    x->alignment_flg |= WA_TOP_RIGHT;
    x->sz_border = 1;
    x->col_border = COLOR_BLACK;
    x->col_bckgrnd = COLOR_WHITE;
    if (x->hot) {
        x->col_bckgrnd = COLOR_GRAY_75;
    }
    x->col_text = COLOR_BLACK;
    x->text = Str { (char*) symbol, 1 };
    x->sz_font = FS_18;
    x->w = 25;
    x->h = 25;

    TreeSibling(x);

    return x->clicked;
}


Widget *UI_CoolPopUp(s32 width, s32 height, s32 padding = 20, bool *close = NULL) {
    Widget *w = WidgetGetNew();
    w->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
    w->features_flg |= WF_LAYOUT_CENTER;

    w->w = width;
    w->h = height;
    w->sz_border = 20;
    w->col_bckgrnd = ColorGray(0.9f);
    w->col_border = ColorGray(0.7f);

    TreeBranch(w);

    Widget *x;
    bool cross_clicked = UI_CrossButton("x", &x);
    if (close) *close = cross_clicked;
    x->w = 18;
    x->h = 18;
    x->x0 = -1;
    x->y0 = 1;
    x->sz_border = 0;
    if (x->hot) {
        x->col_bckgrnd = COLOR_GRAY_50;
    }
    else {
        x->col_bckgrnd = w->col_border;
    }

    Widget *i = WidgetGetNew();
    i->features_flg |= WF_LAYOUT_VERTICAL;
    i->w = width - padding * 2;
    i->h = height - padding * 2;

    TreeBranch(i);

    return i;
}

Widget *UI_Plain() {
    Widget *w = WidgetGetNew();

    TreeBranch(w);
    return w;
}

Widget *UI_LayoutExpandCenter() {
    Widget *w = WidgetGetNew();

    w->features_flg |= WF_EXPAND_VERTICAL;
    w->features_flg |= WF_EXPAND_HORIZONTAL;
    w->features_flg |= WF_LAYOUT_CENTER;

    TreeBranch(w);
    return w;
}

Widget *UI_LayoutHorizontal() {
    Widget *w = WidgetGetNew();
    w->features_flg |= WF_LAYOUT_HORIZONTAL;

    TreeBranch(w);
    return w;
}

Widget *UI_LayoutVertical(bool center_vertical = false) {
    Widget *w = WidgetGetNew();

    w->features_flg |= WF_LAYOUT_VERTICAL;
    if (center_vertical) {
        w->features_flg |= WF_ALIGN_CENTER;
    }

    TreeBranch(w);
    return w;
}

void UI_SpaceH(u32 width) {
    Widget *w = WidgetGetNew();
    w->w = width;

    TreeSibling(w);
}

void UI_SpaceV(u32 height) {
    Widget *w = WidgetGetNew();
    w->h = height;

    TreeSibling(w);
}

Widget *UI_Label(const char *text, Color color = Color { RGBA_BLACK }) {
    Widget *w = WidgetGetNew(text);
    w->features_flg |= WF_DRAW_TEXT;

    w->sz_font = GetFontSize();
    w->col_bckgrnd = ColorGray(0.9f);
    w->col_border = ColorBlack();
    w->col_text = color;

    w->w = TextLineWidth(g_text_plotter, w->text);
    w->h = g_text_plotter->ln_measured;

    TreeSibling(w);
    return w;
}

void UI_Pop() {
    TreePop();
}


#endif


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


#ifndef __PLATFORM_GLFW_H__
#define __PLATFORM_GLFW_H__


#ifdef ENABLE_GLFW


#include <GL/glew.h>
#include <GLFW/glfw3.h>


//
//  OpenGL


void CheckShaderCompilationErrors(GLuint shader, const char *header_info) {
    int success;
    char info_log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        printf("%s:%s\n", header_info, info_log);
    }
}

void CheckShaderLinkErrors(GLuint program, const char *header_info) {
    int success;
    char info_log[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, info_log);
        printf("%s:%s\n", header_info, info_log);
    }
}

void ShaderProgramLink(GLuint *program, const GLchar *vsh_src, const GLchar *fsh_src, const GLchar *frag_data_loc = "o_color") {
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsh_src, NULL);
    glCompileShader(vs);
    CheckShaderCompilationErrors(vs, "vertex shader compilation error");

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsh_src, NULL);
    glCompileShader(fs);
    CheckShaderCompilationErrors(fs, "fragment shader compilation error");

    *program = glCreateProgram();
    glAttachShader(*program, vs);
    glAttachShader(*program, fs);
    glBindFragDataLocation(*program, 0, frag_data_loc);
    glLinkProgram(*program);
    CheckShaderLinkErrors(*program, "shader program link error");

    glDeleteShader(vs);
    glDeleteShader(fs);
    glUseProgram(*program);
}

struct ScreenProgram {
    // draws a texture to the screen
    GLuint program;
    GLuint vao;
    GLuint vbo;
    GLuint texture_id;

    const GLchar* vert_src = R"glsl(
        #version 330 core

        in vec2 position;
        in vec2 tex_coord;
        out vec2 coord;

        void main()
        {
            gl_Position = vec4(position, 0.0, 1.0);
            coord = tex_coord;
        }
    )glsl";
    const GLchar* frag_src = R"glsl(
        #version 330 core

        in vec2 coord;
        out vec4 o_color;
        uniform sampler2D sampler;

        void main()
        {
            o_color = texture(sampler, coord);
        }
    )glsl";

    void SetSize(u8* imgbuffer, u32 width, u32 height) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgbuffer);
        glViewport(0, 0, width, height);
    }

    void Draw(u8* imgbuffer, u32 width, u32 height) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, imgbuffer);

        u32 nverts = 4;
        glDrawArrays(GL_TRIANGLE_STRIP, 0, nverts);
        glBindVertexArray(0);
    }
};

ScreenProgram ScreenProgramInit(u8* imgbuffer, u32 width, u32 height) {
    ScreenProgram prog = {};

    ShaderProgramLink(&prog.program, prog.vert_src, prog.frag_src);
    glGenVertexArrays(1, &prog.vao);
    glBindVertexArray(prog.vao);

    // texture
    glGenTextures(1, &prog.texture_id);
    glBindTexture(GL_TEXTURE_2D, prog.texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glUseProgram(prog.program);
    glBindVertexArray(prog.vao);
    glBindBuffer(GL_ARRAY_BUFFER, prog.vbo);
    prog.SetSize(imgbuffer, width, height);

    // quad
    float sqreen_quad_verts[] = {
        1.0f,  1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 1.0f
    };
    u32 stride = 4;
    u32 nverts = 4;
    glGenBuffers(1, &prog.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, prog.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * stride * nverts, &sqreen_quad_verts, GL_STATIC_DRAW);

    GLint pos_attr = glGetAttribLocation(prog.program, "position");
    glVertexAttribPointer(pos_attr, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), 0);
    glEnableVertexAttribArray(pos_attr);
    GLint tex_attr = glGetAttribLocation(prog.program, "tex_coord");
    glVertexAttribPointer(tex_attr, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*) (2 * sizeof(float)));
    glEnableVertexAttribArray(tex_attr);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return prog;
}


//
//  glfw


struct MousePosition {
    f32 x;
    f32 y;
    f32 dx;
    f32 dy;
    f32 x_frac; // [-1, 1]
    f32 y_frac; // [-1, 1]
    f32 dx_frac;
    f32 dy_frac;
};

struct Button {
    bool pushed;
    bool released;
    bool ended_down;
    u32 pushes;
};

struct Scroll {
    double yoffset_acc;
    u32 steps_down;
    u32 steps_up;
};

struct AsciiKeys {
    u8 keys_cnt;
    u8 keys_idx;
    char keys[32]; // max 32 keystrokes per frame ...

    void Put(char c) {
        if (keys_cnt < 16) {
            keys[keys_cnt++] = c;
        }
    }
    char Get(s32 *mods = NULL) {
        if (keys_cnt && (keys_idx < keys_cnt)) {
            char c = keys[keys_idx++];
            return c;
        }
        else {
            return 0;
        }
    }
};

struct ActionKeys {
    bool esc;
    bool enter;
    bool backspace;
    bool del;
    bool space;
    bool left;
    bool right;
    bool up;
    bool down;
    bool mod_ctrl;
    bool mod_shift;
    bool mod_alt;
    u8 fkey;

    void ResetButKeepMods() {
        bool _mod_ctrl = mod_ctrl;
        bool _mod_shift = mod_shift;
        bool _mod_alt = mod_alt;
        *this = {};
        this->mod_ctrl = _mod_ctrl;
        this->mod_shift = _mod_shift;
        this->mod_alt = _mod_alt;
    }
};


struct PlafGlfw {
    GLFWwindow* window;
    bool fullscreen;
    char *title;

    Button left;
    Button right;
    Scroll scroll;
    AsciiKeys keys;
    ActionKeys akeys;

    MousePosition cursorpos;

    ScreenProgram screen;
    u32 width;
    u32 height;
    u32 width_cache;
    u32 height_cache;
    s32 window_xpos;
    s32 window_ypos;
    u8 *image_buffer;
};


inline PlafGlfw *_GlfwWindowToUserPtr(GLFWwindow* window) {
    PlafGlfw *plaf = (PlafGlfw*) glfwGetWindowUserPointer(window);
    return plaf;
}

void MouseButtonCallBack(GLFWwindow* window, int button, int action, int mods) {
    PlafGlfw *plaf = _GlfwWindowToUserPtr(window);

    // get button
    Button *btn = NULL;
    if (button == GLFW_MOUSE_BUTTON_1) {
        btn = &plaf->left;
    }
    else if (button == GLFW_MOUSE_BUTTON_2) {
        btn = &plaf->right;
    }

    // set event
    if (action == GLFW_PRESS) {
        btn->pushed = true;
        btn->ended_down = true;
    }
    else if (action == GLFW_RELEASE) {
        btn->released = true;
        btn->pushes++;
    }
}

void MouseScrollCallBack(GLFWwindow* window, double xoffset, double yoffset) {
    PlafGlfw *plaf = _GlfwWindowToUserPtr(window);

    plaf->scroll.yoffset_acc += yoffset;
    if (yoffset > 0) {
        plaf->scroll.steps_up++;
    }
    else if (yoffset < 0) {
        plaf->scroll.steps_down++;
    }
}

void CharCallBack(GLFWwindow* window, u32 codepoint) {
    PlafGlfw *plf = _GlfwWindowToUserPtr(window);

    if (codepoint >= 0 && codepoint < 128) {
        char c = (u8) codepoint;
        plf->keys.Put(c);
    }
}

void KeyCallBack(GLFWwindow* window,  int key, int scancode, int action, int mods) {
    PlafGlfw *plf = _GlfwWindowToUserPtr(window);

    if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_LEFT_CONTROL) {
        plf->akeys.mod_ctrl = (action == GLFW_PRESS);
    }
    if (key == GLFW_KEY_LEFT_ALT || key == GLFW_KEY_RIGHT_ALT) {
        plf->akeys.mod_alt = (action == GLFW_PRESS);
    }
    if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
        plf->akeys.mod_shift = (action == GLFW_PRESS);
    }

    if (action == GLFW_PRESS) {
        if (key == 256) {
            plf->akeys.esc = true;
        }
        else if (key == 257) {
            plf->akeys.enter = true;
        }
        else if (key == 259) {
            plf->akeys.backspace = true;
        }
        else if (key == 261) {
            plf->akeys.del = true;
        }
        else if (key == ' ') {
            plf->akeys.space = true;
        }
        else if (key == GLFW_KEY_LEFT) {
            plf->akeys.left = true;
        }
        else if (key == GLFW_KEY_RIGHT) {
            plf->akeys.right = true;
        }
        else if (key == GLFW_KEY_UP) {
            plf->akeys.up = true;
        }
        else if (key == GLFW_KEY_DOWN) {
            plf->akeys.down = true;
        }
        else if (key >= 290 && key <= 301) {
            // 290-301: F1 through F12
            plf->akeys.fkey = key - 289;
        }

        else if (key == 'C' && mods == GLFW_MOD_CONTROL) {
            printf("ctr-C\n");
        }
        else if (key == 'X' && mods == GLFW_MOD_CONTROL) {
            printf("ctr-X\n");
        }
        else if (key == 'Z' && mods == GLFW_MOD_CONTROL) {
            printf("ctr-Z\n");
        }
    }
}

void WindowResizeCallBack(GLFWwindow* window, int width, int height) {
    PlafGlfw *plf = _GlfwWindowToUserPtr(window);

    plf->width = width;
    plf->height = height;
    plf->screen.SetSize(plf->image_buffer, width, height);
}


static u8 *g_image_buffer;
#define IMG_BUFF_CHANNELS 4
#define IMG_BUFF_MAX_WIDTH 3840
#define IMG_BUFF_MAX_HEIGHT 2160
u8 *ImageBufferGet() {
    return g_image_buffer;
}
u8 *ImageBufferInit(MArena *a_dest) {
    g_image_buffer = (u8*) ArenaAlloc(a_dest, IMG_BUFF_CHANNELS * IMG_BUFF_MAX_WIDTH * IMG_BUFF_MAX_HEIGHT);
    return g_image_buffer;
}
void ImageBufferClear(u32 width, u32 height) {
    if (g_image_buffer) {
        memset(g_image_buffer, 255, IMG_BUFF_CHANNELS * width * height);
    }
}


static PlafGlfw g_plaf_glfw;
PlafGlfw* PlafGlfwInit(const char *title, u32 window_width = 640, u32 window_height = 480) {
    g_plaf_glfw = {};
    PlafGlfw *plf = &g_plaf_glfw;
    plf->width = window_width;
    plf->height = window_height;
    plf->title = (char*) title;

    glfwInit();

    // opengl window & context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    plf->window = glfwCreateWindow(plf->width, plf->height, title, NULL, NULL);
    glfwMakeContextCurrent(plf->window);

    // glew
    glewExperimental = GL_TRUE;
    glewInit();

    // alpha blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    // input
    glfwSetCharCallback(plf->window, CharCallBack); // NOTE: potentially use glfwSetCharModsCallback to additionally get the mods
    glfwSetKeyCallback(plf->window, KeyCallBack);
    glfwSetMouseButtonCallback(plf->window, MouseButtonCallBack);
    glfwSetScrollCallback(plf->window, MouseScrollCallBack);
    glfwSetWindowUserPointer(plf->window, plf);

    // window resize
    glfwSetFramebufferSizeCallback(g_plaf_glfw.window, WindowResizeCallBack);

    // shader
    plf->image_buffer = ImageBufferGet();
    plf->screen = ScreenProgramInit(plf->image_buffer, plf->width, plf->height);

    // initialize mouse position values (dx and dy are initialized to zero)
    f64 mouse_x;
    f64 mouse_y;
    glfwGetCursorPos(plf->window, &mouse_x, &mouse_y);
    plf->cursorpos.x = (f32) mouse_x;
    plf->cursorpos.y = (f32) mouse_y;
    plf->cursorpos.x_frac = ((f32) mouse_x - (plf->width * 0.5f)) / plf->width;
    plf->cursorpos.y_frac = ((f32) mouse_y - (plf->height * 0.5f)) / plf->height;

    return plf;
}

void PlafGlfwTerminate(PlafGlfw* plf) {
    glfwDestroyWindow(plf->window);
    glfwTerminate();
}

void PlafGlfwToggleFullscreen(PlafGlfw* plf) {
    plf->fullscreen = !plf->fullscreen;
    if (plf->fullscreen) {
        assert(plf->width_cache == 0);
        assert(plf->height_cache == 0);

        plf->width_cache = plf->width;
        plf->height_cache = plf->height;
        glfwGetWindowPos(plf->window, &plf->window_xpos, &plf->window_ypos);

        GLFWmonitor *monitor = glfwGetWindowMonitor(plf->window);

        const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        plf->width = mode->width;
        plf->height = mode->height;

        glfwSetWindowMonitor(plf->window, monitor, 0, 0, plf->width, plf->height, GLFW_DONT_CARE);
    }
    else {
        plf->width = plf->width_cache;
        plf->height = plf->height_cache;

        plf->width_cache = 0;
        plf->height_cache = 0;

        // doesn't get us back into windowed
        //glfwSetWindowMonitor(plf->window, NULL, 0, 0, 0, 0, GLFW_DONT_CARE);
        // TODO: try creating a "windowed full screen" mode switch

        // destroy and re-create everything (!?!)
        glfwDestroyWindow(plf->window);
        glfwTerminate();
        plf = PlafGlfwInit(plf->title, plf->width, plf->height);
    }

    plf->screen.SetSize(plf->image_buffer, plf->width, plf->height);
}

void PlafGlfwUpdate(PlafGlfw* plf) {
    if (plf->akeys.fkey == 10) {
        // toggle fullscreen

        PlafGlfwToggleFullscreen(plf);
    }

    plf->screen.Draw(plf->image_buffer, plf->width, plf->height);
    glfwSwapBuffers(plf->window);

    plf->left = {};
    plf->right = {};
    plf->scroll = {};
    plf->keys = {};
    plf->akeys.ResetButKeepMods();

    plf->left.ended_down = (glfwGetMouseButton(plf->window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS);
    plf->right.ended_down = (glfwGetMouseButton(plf->window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS);

    f64 mouse_x;
    f64 mouse_y;
    glfwGetCursorPos(plf->window, &mouse_x, &mouse_y);
    
    plf->cursorpos.dx = (f32) mouse_x - plf->cursorpos.x;
    plf->cursorpos.dy = (f32) mouse_y - plf->cursorpos.y;
    plf->cursorpos.x = (f32) mouse_x;
    plf->cursorpos.y = (f32) mouse_y;

    f32 x_frac = ((f32) mouse_x - plf->width * 0.5f) / plf->width;
    f32 y_frac = ((f32) mouse_y - plf->height * 0.5f) / plf->height;
    plf->cursorpos.dx_frac = plf->cursorpos.x_frac - x_frac;
    plf->cursorpos.dy_frac = plf->cursorpos.y_frac - y_frac;
    plf->cursorpos.x_frac = x_frac;
    plf->cursorpos.y_frac = y_frac;

    glfwPollEvents();
}


inline Button MouseLeft() { return g_plaf_glfw.left; }
inline Button MouseRight() { return g_plaf_glfw.right; }
inline Scroll MouseScroll() { return g_plaf_glfw.scroll; }
inline Vector2f MouseFrac() { return { g_plaf_glfw.cursorpos.x_frac, g_plaf_glfw.cursorpos.y_frac }; }
inline Vector2f MouseFracDelta() { return { (f32) g_plaf_glfw.cursorpos.dx / g_plaf_glfw.width, (f32) g_plaf_glfw.cursorpos.dy / g_plaf_glfw.height }; }
inline char GetChar() { return g_plaf_glfw.keys.Get(); }
inline bool GetEscape() { return g_plaf_glfw.akeys.esc; }
inline bool GetEnter() { return g_plaf_glfw.akeys.enter; }
inline bool GetSpace() { return g_plaf_glfw.akeys.space; }
inline bool GetBackspace() { return g_plaf_glfw.akeys.backspace; }
inline bool GetDelete() { return g_plaf_glfw.akeys.del; }
inline bool GetLeft() { return g_plaf_glfw.akeys.left; }
inline bool GetRight() { return g_plaf_glfw.akeys.right; }
inline bool GetUp() { return g_plaf_glfw.akeys.up; }
inline bool GetDown() { return g_plaf_glfw.akeys.down; }

inline bool GetFKey(u32 *fval) {
    assert(fval != NULL);

    u8 fkey = g_plaf_glfw.akeys.fkey;
    if (fkey == 0) {
        return false;
    }
    else {
        *fval = fkey;
        return true;
    }
}

inline bool GetFKey(u32 fval) {
    if (g_plaf_glfw.akeys.fkey == fval) {
        return true;
    }
    else {
        return false;
    }
}

inline bool GetChar(char c) {
    for (s32 i = 0; i < g_plaf_glfw.keys.keys_cnt; ++i) {
        char key = g_plaf_glfw.keys.keys[i];
        if (key == c) {
            return true;
        }
    }
    return false;
}

inline bool ModCtrl() { return g_plaf_glfw.akeys.mod_ctrl; }
inline bool ModShift() { return g_plaf_glfw.akeys.mod_shift; }
inline bool ModAlt() { return g_plaf_glfw.akeys.mod_alt; }

bool GetWindowShouldClose(PlafGlfw *plf) { return glfwWindowShouldClose(plf->window); }


#endif


#endif


#ifndef __CBUI_INIT_H__
#define __CBUI_INIT_H__


struct CbuiState {
    MContext *ctx;
    PlafGlfw *plf;
    u64 frameno;
    u64 dts[8];
    u64 t_framestart;
    u64 t_framestart_prev;
    f32 dt;
    f32 fr;
    bool running;

    f32 TimeSince(f32 t) {
        return t_framestart - t; 
    }
};

static CbuiState _g_cbui_state;
static CbuiState *cbui;

CbuiState *CbuiInit(const char *title, bool start_in_fullscreen) {
    _g_cbui_state = {};
    cbui = &_g_cbui_state;
    cbui->running = true;
    cbui->ctx = InitBaselayer();
    cbui->plf = PlafGlfwInit(title);
    cbui->plf->image_buffer = ImageBufferInit(cbui->ctx->a_life);
    cbui->t_framestart = ReadSystemTimerMySec();
    cbui->t_framestart_prev = cbui->t_framestart;

    InitImUi(cbui->plf->width, cbui->plf->height, &cbui->frameno);

    ImageRGBA render_target = { (s32) cbui->plf->width, (s32) cbui->plf->height, (Color*) cbui->plf->image_buffer };
    QuadBufferInit(cbui->ctx->a_life);

    g_texture_map = InitMap(cbui->ctx->a_life, MAX_RESOURCE_CNT);
    g_resource_map = InitMap(cbui->ctx->a_life, MAX_RESOURCE_CNT);

    // load & check resource file
    ResourceStreamHandle hdl = ResourceStreamLoadAndOpen(cbui->ctx->a_tmp, cbui->ctx->a_life, "all.res");
    g_font_names = hdl.names[RST_FONT];
    bool log_verbose = false;

    // map out the resources
    ResourceHdr *res = hdl.first;
    while (res) {
        // fonts
        if (res->tpe == RST_FONT) {
            FontAtlas *font = FontAtlasLoadBinaryStream(res->GetInlinedData(), res->data_sz);
            if (log_verbose) { font->Print(); }

            MapPut(&g_resource_map, font->GetKey(), font);
            MapPut(&g_texture_map, font->GetKey(), &font->texture);
        }

        // sprite maps
        else if (res->tpe == RST_SPRITE) {
            SpriteMap *smap = SpriteMapLoadStream((u8*) res->GetInlinedData(), res->data_sz);
            if (log_verbose) {

                printf("sprite map: %s, %s, count: %u, atlas w: %u, atlas h: %u\n", smap->map_name, smap->key_name, smap->sprites.len, smap->texture.width, smap->texture.height);
            }

            MapPut(&g_resource_map, smap->GetKey(), smap);
            MapPut(&g_texture_map, smap->GetKey(), &smap->texture);
        }

        // other
        else {
            printf("WARN: unknown resource detected\n");
        }

        // iter
        res = res->GetInlinedNext();
    }
    SetFontAndSize(FS_48, g_font_names->GetStr());

    if (start_in_fullscreen) { PlafGlfwToggleFullscreen(cbui->plf); }

    return cbui;
}


#define FR_RUNNING_AVG_COUNT 4
void CbuiFrameStart() {
    ArenaClear(cbui->ctx->a_tmp);
    ImageBufferClear(cbui->plf->width, cbui->plf->height);

    cbui->t_framestart = ReadSystemTimerMySec();
    cbui->dt = (cbui->t_framestart - cbui->t_framestart_prev) / 1000;
    cbui->dts[cbui->frameno % FR_RUNNING_AVG_COUNT] = cbui->dt;

    f32 sum = 0;
    for (s32 i = 0; i < FR_RUNNING_AVG_COUNT; ++i) { sum += cbui->dts[i]; }
    f32 dt_avg = sum / FR_RUNNING_AVG_COUNT;
    cbui->fr = 1.0f / dt_avg * 1000;
    cbui->t_framestart_prev = cbui->t_framestart;

    cbui->frameno++;
}

void CbuiFrameEnd() {
    // TODO: get delta t and framerate under control
    XSleep(1);

    UI_FrameEnd(cbui->ctx->a_tmp, cbui->plf->width, cbui->plf->height);
    QuadBufferBlitAndClear(InitImageRGBA(cbui->plf->width, cbui->plf->height, g_image_buffer));

    PlafGlfwUpdate(cbui->plf);
    // TODO: clean up these globals
    g_mouse_x = cbui->plf->cursorpos.x;
    g_mouse_y = cbui->plf->cursorpos.y;
    g_mouse_down = MouseLeft().ended_down;
    g_mouse_pushed = MouseLeft().pushed;

    cbui->running = cbui->running && !GetEscape() && !GetWindowShouldClose(cbui->plf);
}

void CbuiExit() {
    PlafGlfwTerminate(cbui->plf);
}


#endif


#endif // __JG_CBUI_H__

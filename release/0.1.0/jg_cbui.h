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


// 200520:
//
//#include "gtypes.h"
//#include "geometry.h"
//#include "indices.h"
//#include "octree.h"


#define CBUI_VERSION_MAJOR 0
#define CBUI_VERSION_MINOR 1
#define CBUI_VERSION_PATCH 0

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


//
//  gtypes.h
//


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


//
//  geometry.h
//


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


//
//  indices.h
//


//
//  Preserves the order of indices.
//  Preserves the number of indices.
//  Index sets may have duplicates
//  Preserves the order of values.
//  Values will not have duplicates.
//


//
//  Append-indices may be negative, enabling stitching-like operations.
//


// TODO: move geometry.h using functions to user code


void IndicesAppend(
    MArena *a_dest,
    List<Vector3f> *values_dest,
    List<u32> *indices_dest,
    List<Vector3f> values,
    List<u32> indices,
    List<Vector3f> values_append,
    List<s32> indices_append)
{
    *values_dest = InitList<Vector3f>(a_dest, values.len + values_append.len);
    *indices_dest = InitList<u32>(a_dest, indices.len + indices_append.len);

    _memcpy(values_dest->lst, values.lst, values.len * sizeof(Vector3f));
    values_dest->len = values.len;
    _memcpy(values_dest->lst + values_dest->len, values_append.lst, values_append.len * sizeof(Vector3f));
    values_dest->len += values_append.len;
    _memcpy(indices_dest->lst, indices.lst, indices.len * sizeof(u32));
    indices_dest->len = indices.len;

    // shifted and copy append-indices
    u32 shift = values.len;
    for (u32 i = 0; i < indices_append.len; ++i) {
        u32 append = indices_append.lst[i];
        assert(append + shift >= 0 && "check producing actual index");
        u32 idx = append + shift;
        indices_dest->Add(idx);
    }
}


//
//  Indirection-set based functions / building blocks for extract/remove.
//  The indirection set is an index set, of the same length as the values list,
//  (or the largest index into the values array).
//
//  The indirection set is used to re-code index arrays. Indirection sets can mark and shift
//  indices into a new values array, a selection of the original.
//


List<u32> IndicesMarkPositive(MArena *a_dest, u32 vals_len, List<u32> idxs_pos) {
    List<u32> indirection = InitList<u32>(a_dest, vals_len);
    indirection.len = vals_len;
    for (u32 i = 0; i < indirection.len; ++i) {
        indirection.lst[i] = -1;
    }

    u32 idx;
    for (u32 i = 0; i < idxs_pos.len; ++i) {
        idx = idxs_pos.lst[i];
        indirection.lst[idx] = idx;
    }
    return indirection;
}


List<u32> IndicesMarkNegative(MArena *a_dest, u32 vals_len, List<u32> idxs_neg) {
    List<u32> indirection = InitList<u32>(a_dest, vals_len);
    indirection.len = vals_len;
    for (u32 i = 0; i < indirection.len; ++i) {
        indirection.lst[i] = i;
    }

    u32 idx;
    for (u32 i = 0; i < idxs_neg.len; ++i) {
        idx = idxs_neg.lst[i];
        indirection.lst[idx] = -1;
    }
    return indirection;
}


List<u32> IndicesMarkPositiveNegative(MArena *a_dest, u32 vals_len, List<u32> idxs_pos, List<u32> idxs_neg) {
    List<u32> indirection = InitList<u32>(a_dest, vals_len);
    indirection.len = vals_len;
    for (u32 i = 0; i < indirection.len; ++i) {
        indirection.lst[i] = -1;
    }

    u32 idx;
    for (u32 i = 0; i < idxs_pos.len; ++i) {
        idx = idxs_pos.lst[i];
        indirection.lst[idx] = idx;
    }

    for (u32 i = 0; i < idxs_neg.len; ++i) {
        idx = idxs_neg.lst[i];
        indirection.lst[idx] = -1;
    }
    return indirection;
}


void IndicesShiftDownIndirectionList(List<u32> indirection) {
    u32 idx;
    u32 acc = 0;
    for (u32 i = 0; i < indirection.len; ++i) {
        idx = indirection.lst[i];
        if (idx == -1) {
            ++acc;
        }
        else {
            assert(i == idx && "");
            indirection.lst[i] = i - acc;
        }
    }
}


void IndicesIndirectInline(List<u32> idxs, List<u32> indirection) {
    u32 idx;
    for (u32 i = 0; i < idxs.len; ++i) {
        idx = idxs.lst[i];
        assert(idx < indirection.len && "");

        idxs.lst[i] = indirection.lst[idx];
    }
}


List<u32> IndicesIndirect(MArena *a_dest, List<u32> idxs, List<u32> indirection) {
    List<u32> idxs_out = InitList<u32>(a_dest, 0);
    u32 idx;
    for (u32 i = 0; i < idxs.len; ++i) {
        idx = idxs.lst[i];

        assert(idx < indirection.len && "");
        ArenaAlloc(a_dest, sizeof(u32));
        idxs_out.Add(indirection.lst[idx]);
    }
    return idxs_out;
}


List<u32> IndicesIndirectOrRemove(MArena *a_dest, List<u32> idxs, List<u32> indirection) {
    List<u32> idxs_out = InitList<u32>(a_dest, 0);
    u32 idx;
    u32 indir;
    for (u32 i = 0; i < idxs.len; ++i) {
        idx = idxs.lst[i];
        assert(idx < indirection.len && "");

        indir = indirection.lst[idx];
        if (indir != -1) {
            ArenaAlloc(a_dest, sizeof(u32));
            idxs_out.Add(indir);
        }
    }
    return idxs_out;
}


template<class T>
List<T> IndicesSelectValues(MArena *a_dest, List<T> values, List<u32> indirection) {
    List<T> seln = InitList<T>(a_dest, 0);
    T val;
    u32 idx;
    for (u32 i = 0; i < indirection.len; ++i) {
        idx = indirection.lst[i];

        if (idx != -1) {
            val = values.lst[i];
            ArenaAlloc(a_dest, sizeof(T));
            seln.Add(val);
        }
    }
    return seln;
}


//
//  Wrappers
//


template<class T>
List<u32> IndicesExtract(MArena *a_dest, MArena *a_indir, List<T> values, List<u32> idxs, List<T> *values_out, List<u32> *idxs_out) {
    List<u32> indirection = IndicesMarkPositive(a_indir, values.len, idxs);
    IndicesShiftDownIndirectionList(indirection);

    *values_out = IndicesSelectValues<T>(a_dest, values, indirection);
    *idxs_out = IndicesIndirect(a_dest, idxs, indirection);

    return indirection;
}
template<class T>
List<u32> IndicesExtractVals(MArena *a_dest, MArena *a_indir, List<T> values, List<u32> idxs, List<T> *values_out) {
    List<u32> indirection = IndicesMarkPositive(a_indir, values.len, idxs);
    IndicesShiftDownIndirectionList(indirection);

    *values_out = IndicesSelectValues<T>(a_dest, values, indirection);

    return indirection;
}


template<class T>
List<u32> IndicesRemove(MArena *a_dest, MArena *a_indir, List<T> values, List<u32> idxs, List<u32> idxs_rm, List<T> *values_out, List<u32> *idxs_out) {
    List<u32> indirection = IndicesMarkPositiveNegative(a_indir, values.len, idxs, idxs_rm);
    IndicesShiftDownIndirectionList(indirection);

    *values_out = IndicesSelectValues<T>(a_dest, values, indirection);
    *idxs_out = IndicesIndirectOrRemove(a_dest, idxs, indirection);

    return indirection;
}

List<u32> IndicesRemove(MArena *a_dest, MArena *a_indir, u32 vals_len, List<u32> idxs, List<u32> idxs_rm, List<u32> *idxs_out) {
    List<u32> indirection = IndicesMarkPositiveNegative(a_indir, vals_len, idxs, idxs_rm);
    IndicesShiftDownIndirectionList(indirection);

    *idxs_out = IndicesIndirectOrRemove(a_dest, idxs, indirection);

    return indirection;
}


//
//  octree.h
//


//
// Octree Stats / runtime and debug info


struct OcTreeStats {
    u32 depth_max;

    u32 max_branches;
    u32 max_leaves;
    u32 actual_branches;
    u32 actual_leaves;

    float cullbox_radius;
    float leaf_size;
    float rootcube_radius;

    float avg_verts_pr_leaf;
    u32 nvertices_in;
    u32 nvertices_out;
    float PctReduced() {
        assert(nvertices_in != 0 && "ensure initialization");
        float pct_reduced = 100.0f * nvertices_out / nvertices_in;
        return pct_reduced;
    }

    void Print() {
        printf("Depth:                 %u\n", depth_max);
        printf("Cullbox radius:        %f\n", cullbox_radius);
        printf("Rootcube radius:       %f\n", rootcube_radius);
        printf("Leaf size:             %f\n", leaf_size);
        printf("Branch nodes:          %u\n", actual_branches);
        printf("Leaf nodes (octets):   %u\n", actual_leaves);
        printf("Vertices in:           %u\n", nvertices_in);
        printf("Vertices out:          %u\n", nvertices_out);
        printf("Av. vertices pr. cube: %f\n", avg_verts_pr_leaf);
        printf("Reduction pct.:        %.2f (%u to %u)\n", 100 - PctReduced(), nvertices_in, nvertices_out);
    }
};
u32 OcTreeStatsSubCubesTotal(u32 depth) {
    u32 ncubes = 0;
    u32 power_of_eight = 1;
    for (u32 i = 1; i <= depth; ++i) {
        power_of_eight *= 8;
        ncubes += power_of_eight;
    }
    return ncubes;
}
u32 OcTreeStatsLeafSize2Depth(float leaf_size, float box_diameter, float *leaf_size_out = NULL) {
    assert(leaf_size_out != NULL);
    if (leaf_size > box_diameter) {
        return 0;
    }

    u32 depth = 1;
    u32 power_of_two = 2;
    while (leaf_size < box_diameter / power_of_two) {
        power_of_two *= 2;
        ++depth;
    }

    *leaf_size_out = box_diameter / power_of_two;
    return depth;
}
void OcTreeCalculateCubeRadiusAndDepthFromLeafSize(float leaf_size, float cullbox_radius, float *out_rootcube_radius, u32 *out_depth) {
    assert(out_rootcube_radius != NULL);
    assert(out_depth != NULL);

    float cullbox_diameter = cullbox_radius * 2;

    // demand minimum depth 1 - 8 cubes
    assert(leaf_size < cullbox_diameter);

    u32 depth = 1;
    u32 two_to_depth = 2;
    float rootcube_diameter = leaf_size * two_to_depth;
    while (rootcube_diameter < cullbox_diameter) {
        ++depth;
        two_to_depth *= 2;
        rootcube_diameter = leaf_size * two_to_depth;
    }

    *out_depth = depth;
    *out_rootcube_radius = rootcube_diameter * 0.5f;
}
OcTreeStats OcTreeStatsInit(float cullbox_radius, float leaf_size) {
    OcTreeStats stats;

    //float actual_leaf_size;
    //u32 depth = OcTreeStatsLeafSize2Depth(leaf_size_max, 2 * cullbox_radius, &actual_leaf_size);

    float rootcube_radius;
    u32 depth;
    OcTreeCalculateCubeRadiusAndDepthFromLeafSize(leaf_size, cullbox_radius, &rootcube_radius, &depth);

    //assert(depth <= 9 && "recommended max depth is 9");
    // DEBUG: will we crash?
    assert(depth <= 10 && "recommended max depth is 10");
    assert(depth >= 1 && "min depth is 2");

    stats.depth_max = depth;
    u32 max_cubes = OcTreeStatsSubCubesTotal(depth);
    stats.max_branches = OcTreeStatsSubCubesTotal(depth - 1);
    stats.max_leaves = max_cubes - stats.max_branches;
    stats.cullbox_radius = cullbox_radius;
    stats.rootcube_radius = rootcube_radius;
    stats.leaf_size = leaf_size;
    stats.nvertices_in = 0;
    stats.nvertices_out = 0;

    assert(stats.rootcube_radius >= stats.cullbox_radius && "rootcube region greater than or equal to the minimal value; for targeting custom leaf sizes");

    return stats;
}


//
// Voxel Grid Reduce


inline
bool FitsWithinBoxRadius(Vector3f point, float radius) {
    bool result =
        (abs(point.x) <= radius) &&
        (abs( point.y) <= radius) && 
        (abs( point.z) <= radius);
    return result;
}
inline
u8 SubcubeDescend(Vector3f target, Vector3f *center, float *radius) {
    Vector3f relative = target - *center;
    bool neg_x = relative.x < 0;
    bool neg_y = relative.y < 0;
    bool neg_z = relative.z < 0;

    // subcube index 0-7 octets: ---, --+, -+-, -++, +--, +-+, ++-, +++
    u8 subcube_idx = 4*neg_x + 2*neg_y + neg_z;

    // subcube center:
    *radius = 0.5f * (*radius);
    float pm_x = (1 - neg_x * 2) * (*radius);
    float pm_y = (1 - neg_y * 2) * (*radius);
    float pm_z = (1 - neg_z * 2) * (*radius);
    *center = Vector3f {
        center->x + pm_x,
        center->y + pm_y,
        center->z + pm_z
    };

    return subcube_idx;
}
#define VGR_DEBUG 0
struct OcBranch {
    u16 indices[8];
};
struct OcLeaf {
    Vector3f points_sum[8];
    Vector3f normals_sum[8];
    u32 cnt[8];

    #if VGR_DEBUG
    Vector3f center[8];
    float radius[8];
    u8 cube_idx[8];
    #endif
};


static OcBranch _branch_zero;
static OcLeaf _leaf_zero;
struct VoxelGridReduce {
    MArena a;
    OcTreeStats stats;
    Matrix4f center_transform;

    List<OcLeaf> leaves;
    List<OcBranch> branches;

    void AddPoints(List<Vector3f> vertices, List<Vector3f> normals, Matrix4f src_transform) {
        OcLeaf *leaf;
        OcBranch *branch;

        // build the tree
        Vector3f p;
        Vector3f n;
        Vector3f c = Vector3f_Zero();
        float r;
        Matrix4f p2box = TransformGetInverse(center_transform) * src_transform;
        u8 sidx;
        u16 *bidx;
        u16 *lidx;
        for (u32 i = 0; i < vertices.len; ++i) {
            // filter
            p = vertices.lst[i];
            p =  TransformPoint(p2box, p);
            bool keep = FitsWithinBoxRadius(p, stats.cullbox_radius);
            if (keep == false) {
                continue;
            }
            n = normals.lst[i];
            n = TransformDirection(p2box, n);

            // init
            branch = branches.lst;
            r = stats.rootcube_radius;
            c = Vector3f_Zero();

            // d < d_max-1
            for (u32 d = 1; d < stats.depth_max - 1; ++d) {
                sidx = SubcubeDescend(p, &c, &r);
                bidx = branch->indices + sidx;

                if (*bidx == 0) {
                    *bidx = branches.len;
                    branches.Add(_branch_zero);
                }
                branch = branches.lst + *bidx;
            }

            // d == d_max-1
            sidx = SubcubeDescend(p, &c, &r);
            lidx = branch->indices + sidx;
            if (*lidx == 0) {
                assert((u8*) (leaves.lst + leaves.len) == a.mem + a.used && "check memory contiguity");

                *lidx = leaves.len;
                ArenaAlloc(&a, sizeof(OcLeaf));
                leaves.Add(_leaf_zero);
            }
            leaf = leaves.lst + *lidx;

            // d == d_max
            sidx = SubcubeDescend(p, &c, &r);
            #if VGR_DEBUG
            leaf->center[sidx] = c;
            leaf->radius[sidx] = r;
            #endif

            leaf->points_sum[sidx] = leaf->points_sum[sidx] + p;
            leaf->normals_sum[sidx] = leaf->normals_sum[sidx] + n;
            leaf->cnt[sidx] = leaf->cnt[sidx] + 1;
        }
        stats.actual_branches = branches.len;
        stats.actual_leaves = leaves.len;
        stats.nvertices_in += vertices.len;
    }
    void GetPoints(MArena *a_dest, List<Vector3f> *points_dest, List<Vector3f> *normals_dest) {
        assert(points_dest != NULL);
        assert(normals_dest != NULL);

        // points
        u32 npoints_max = leaves.len * 8;
        List<Vector3f> points = InitList<Vector3f>(a_dest, npoints_max);
        Vector3f avg;
        Vector3f sum;
        Vector3f p_world;
        Vector3f n_world;
        u32 cnt = 0;
        float cnt_inv;
        float cnt_sum = 0.0f;
        for (u32 i = 0; i < leaves.len; ++i) {
            OcLeaf leaf = leaves.lst[i];
            for (u32 j = 0; j < 8; ++j) {
                cnt = leaf.cnt[j];
                if (cnt > 0) {
                    cnt_inv = 1.0f / cnt;

                    sum = leaf.points_sum[j];
                    avg = cnt_inv * sum;
                    p_world = TransformPoint(center_transform, avg);
                    points.Add(&p_world);

                    cnt_sum += cnt;
                }
            }
        }
        ArenaRelease(a_dest, sizeof(Vector3f) * (npoints_max - points.len) );
        *points_dest = points;
        stats.nvertices_out = points.len;
        stats.avg_verts_pr_leaf = cnt_sum / stats.nvertices_out;

        // normals
        List<Vector3f> normals = InitList<Vector3f>(a_dest, points.len);
        for (u32 i = 0; i < leaves.len; ++i) {
            OcLeaf leaf = leaves.lst[i];
            for (u32 j = 0; j < 8; ++j) {
                cnt = leaf.cnt[j];
                if (cnt > 0) {
                    cnt_inv = 1.0f / cnt;

                    sum = leaf.normals_sum[j];
                    avg = cnt_inv * sum;
                    n_world = TransformDirection(center_transform, avg);
                    normals.Add(n_world);
                }
            }
        }
        *normals_dest = normals;
    }
};
VoxelGridReduce VoxelGridReduceInit(float leaf_size_max, float cullbox_radius, Matrix4f cullbox_transform) {
    VoxelGridReduce vgr;

    vgr.a = ArenaCreate();
    vgr.center_transform = cullbox_transform;
    vgr.stats = OcTreeStatsInit(cullbox_radius, leaf_size_max);
    vgr.branches = InitList<OcBranch>(&vgr.a, vgr.stats.max_branches / 8);
    vgr.branches.Add(_branch_zero);
    vgr.leaves = InitList<OcLeaf>(&vgr.a, 0);

    return vgr;
}


//
//  color.h
//




//
//  font.h
//




//
//  resource.h
//




//
//  sprite.h
//



#endif

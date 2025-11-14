#pragma once
#include <string>
#include "Vector3.h"
#include <cmath>

namespace MathClasses
{
    // Represents a 3x3 matrix for 2D/3D transformations.
    struct Matrix3
    {
    public:
        // Union for multiple access patterns: flat, 2D, axis vectors.
        union
        {
            struct
            {
                float m1, m2, m3, m4, m5, m6, m7, m8, m9;
            };
            float v[9];           // Flat array access
            float mm[3][3];       // 2D array access
            Vector3 axis[3];      // As 3 Vector3 axes
        };

        // Default constructor (zero matrix).
        Matrix3() :
            m1(0.0f), m2(0.0f), m3(0.0f),
            m4(0.0f), m5(0.0f), m6(0.0f),
            m7(0.0f), m8(0.0f), m9(0.0f)
        {
        }

        // Constructor with explicit values.
        Matrix3(float _m1, float _m2, float _m3,
            float _m4, float _m5, float _m6,
            float _m7, float _m8, float _m9)
            : m1(_m1), m2(_m2), m3(_m3),
            m4(_m4), m5(_m5), m6(_m6),
            m7(_m7), m8(_m8), m9(_m9)
        {
        }

        // Construct from array of 9 floats.
        Matrix3(const float arr[9])
        {
            for (int i = 0; i < 9; ++i)
                v[i] = arr[i];
        }

		// Returns the identity matrix in row major.
        static Matrix3 MakeIdentity()
        {
            Matrix3 identity;
            identity.m1 = 1;
            identity.m2 = 0;
            identity.m3 = 0;
            identity.m4 = 0;
            identity.m5 = 1;
            identity.m6 = 0;
            identity.m7 = 0;
            identity.m8 = 0;
            identity.m9 = 1;
            return identity;
        }

        // Access element by index (mutable).
        float& operator [](int dim)
        {
            return v[dim];
        }

        // Access element by index (read-only).
        const float& operator [](int dim) const
        {
            return v[dim];
        }

        // Matrix multiplication.
        Matrix3 operator *(Matrix3 rhs) const
        {
            return Matrix3(
                m1 * rhs.m1 + m4 * rhs.m2 + m7 * rhs.m3,
                m2 * rhs.m1 + m5 * rhs.m2 + m8 * rhs.m3,
                m3 * rhs.m1 + m6 * rhs.m2 + m9 * rhs.m3,
                m1 * rhs.m4 + m4 * rhs.m5 + m7 * rhs.m6,
                m2 * rhs.m4 + m5 * rhs.m5 + m8 * rhs.m6,
                m3 * rhs.m4 + m6 * rhs.m5 + m9 * rhs.m6,
                m1 * rhs.m7 + m4 * rhs.m8 + m7 * rhs.m9,
                m2 * rhs.m7 + m5 * rhs.m8 + m8 * rhs.m9,
                m3 * rhs.m7 + m6 * rhs.m8 + m9 * rhs.m9);
        }

        // Matrix addition.
        Matrix3 operator+(const Matrix3& rhs) const
        {
            return Matrix3(
                m1 + rhs.m1, m2 + rhs.m2, m3 + rhs.m3,
                m4 + rhs.m4, m5 + rhs.m5, m6 + rhs.m6,
                m7 + rhs.m7, m8 + rhs.m8, m9 + rhs.m9
            );
        }

        // Matrix subtraction.
        Matrix3 operator-(const Matrix3& rhs) const
        {
            return Matrix3(
                m1 - rhs.m1, m2 - rhs.m2, m3 - rhs.m3,
                m4 - rhs.m4, m5 - rhs.m5, m6 - rhs.m6,
                m7 - rhs.m7, m8 - rhs.m8, m9 - rhs.m9
            );
        }

        // Scalar multiplication.
        Matrix3 operator*(float scalar) const
        {
            return Matrix3(
                m1 * scalar, m2 * scalar, m3 * scalar,
                m4 * scalar, m5 * scalar, m6 * scalar,
                m7 * scalar, m8 * scalar, m9 * scalar
            );
        }

        // Scalar multiplication (friend, scalar first).
        friend Matrix3 operator*(float scalar, const Matrix3& matrix)
        {
            return matrix * scalar;
        }

        // Compound assignment for matrix multiplication.
        Matrix3& operator *=(const Matrix3& rhs)
        {
            *this = *this * rhs;
            return *this;
        }

        // Matrix-vector multiplication.
        Vector3 operator *(const Vector3& rhs) const
        {
            return Vector3(
                m1 * rhs.x + m4 * rhs.y + m7 * rhs.z,
                m2 * rhs.x + m5 * rhs.y + m8 * rhs.z,
                m3 * rhs.x + m6 * rhs.y + m9 * rhs.z
            );
        }

        // Equality comparison with threshold for floating point error.
        bool operator==(const Matrix3& rhs) const
        {
            const float THRESHOLD = 0.00001f;
            for (int i = 0; i < 9; ++i)
            {
                if (fabsf(v[i] - rhs.v[i]) > THRESHOLD)
                    return false;
            }
            return true;
        }

        // Inequality comparison.
        bool operator != (const Matrix3& rhs) const
        {
            return !(*this == rhs);
        }

        // Set as X-axis rotation matrix.
        void setRotateX(float a)
        {
            m1 = 1; m2 = 0; m3 = 0;
            m4 = 0; m5 = cosf(a); m6 = -sinf(a);
            m7 = 0; m8 = sinf(a); m9 = cosf(a);
        }

        // Set as Y-axis rotation matrix.
        void setRotateY(float a)
        {
            m1 = cosf(a); m2 = 0; m3 = -sinf(a);
            m4 = 0; m5 = 1; m6 = 0;
            m7 = sinf(a); m8 = 0; m9 = cosf(a);
        }

        // Set as Z-axis rotation matrix.
        void setRotateZ(float a)
        {
            m1 = cosf(a); m2 = sinf(a); m3 = 0;
            m4 = -sinf(a); m5 = cosf(a); m6 = 0;
            m7 = 0; m8 = 0; m9 = 1;
        }

        // Set as 2D translation matrix.
        void setTranslate(float x, float y)
        {
            m1 = 1.0f; m2 = 0.0f; m3 = 0.0f;
            m4 = 0.0f; m5 = 1.0f; m6 = 0.0f;
            m7 = x; m8 = y; m9 = 1.0f;
        }

        // Returns the transposed matrix.
        Matrix3 Transposed() const
        {
            return Matrix3(m1, m4, m7, m2, m5, m8, m3, m6, m9);
        }

        // Returns a string representation of the matrix.
        std::string ToString() const
        {
            std::string str = std::to_string(v[0]);
            for (size_t i = 1; i < 9; ++i)
            {
                str += ", " + std::to_string(v[i]);
            }
            return str;
        }

        // Static factory: 2D translation matrix.
        static Matrix3 MakeTranslation(float x, float y, float z)
        {
            return Matrix3(1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                x, y, 1.0f);
        }

        // Static factory: 2D translation matrix from vector.
        static Matrix3 MakeTranslation(Vector3 vec)
        {
            return Matrix3(1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                vec.x, vec.y, 1.0f);
        }

        // Static factory: X-axis rotation matrix.
        static Matrix3 MakeRotateX(float a)
        {
            return Matrix3(1, 0, 0,
                0, cosf(a), -sinf(a),
                0, sinf(a), cosf(a));
        }

        // Static factory: Y-axis rotation matrix.
        static Matrix3 MakeRotateY(float a)
        {
            return Matrix3(cosf(a), 0, sinf(a),
                0, 1, 0,
                -sinf(a), 0, cosf(a));
        }

        // Static factory: Z-axis rotation matrix.
        static Matrix3 MakeRotateZ(float a)
        {
            return Matrix3(cosf(a), sinf(a), 0,
                -sinf(a), cosf(a), 0,
                0, 0, 1);
        }

        // Static factory: Euler rotation matrix (pitch, yaw, roll).
        static Matrix3 MakeEuler(float pitch, float yaw, float roll)
        {
            Matrix3 x = MakeRotateX(pitch);
            Matrix3 y = MakeRotateY(yaw);
            Matrix3 z = MakeRotateZ(roll);
            return (z * y * x);
        }

        // Static factory: Euler rotation from vector.
        static Matrix3 MakeEuler(const Vector3& euler)
        {
            return MakeEuler(euler.x, euler.y, euler.z);
        }

        // Static factory: 2D scale matrix.
        static Matrix3 MakeScale(float xScale, float yScale)
        {
            return Matrix3(xScale, 0.0f, 0.0f,
                0.0f, yScale, 0.0f,
                0.0f, 0.0f, 1.0f);
        }

        // Static factory: 3D scale matrix.
        static Matrix3 MakeScale(float xScale, float yScale, float zScale)
        {
            return Matrix3(xScale, 0.0f, 0.0f,
                0.0f, yScale, 0.0f,
                0.0f, 0.0f, zScale);
        }

        // Static factory: scale matrix from vector.
        static Matrix3 MakeScale(Vector3 scale)
        {
            return Matrix3(scale.x, 0.0f, 0.0f,
                0.0f, scale.y, 0.0f,
                0.0f, 0.0f, scale.z);
        }
    };
}
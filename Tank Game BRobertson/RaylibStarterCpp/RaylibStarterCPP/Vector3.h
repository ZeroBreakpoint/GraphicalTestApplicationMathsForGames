#pragma once
#ifndef VECTOR3_H
#define VECTOR3_H
#include <string>
#include <cmath>

namespace MathClasses
{
    // Represents a 3D vector or RGB color.
    struct Vector3
    {
    public:
        // Default constructor initializes to zero.
        Vector3() : x{ 0 }, y{ 0 }, z{ 0 }
        {
        }

        // Constructor with explicit x, y, z values.
        Vector3(float _x, float _y, float _z)
        {
            x = _x;
            y = _y;
            z = _z;
        }

        // Union allows access as x/y/z, r/g/b, or array.
        union
        {
            struct { float x, y, z; };   // Cartesian coordinates
            struct { float r, g, b; };   // Color channels
            float data[3];               // Array access
        };

        // Access vector component by index (read-only).
        const float& operator [](int dim) const
        {
            return data[dim];
        }

        // Implicit conversion to const float pointer.
        operator const float* () const { return data; }

        // Vector addition.
        Vector3 operator +(const Vector3& rhs) const
        {
            Vector3 sum;
            sum.x = x + rhs.x;
            sum.y = y + rhs.y;
            sum.z = z + rhs.z;
            return sum;
        }

        // Vector subtraction.
        Vector3 operator -(const Vector3& rhs) const
        {
            Vector3 subtract;
            subtract.x = x - rhs.x;
            subtract.y = y - rhs.y;
            subtract.z = z - rhs.z;
            return subtract;
        }

        // Equality comparison with threshold for floating point error.
        bool operator == (const Vector3& rhs) const
        {
            float xDist = fabsf(x - rhs.x);
            float yDist = fabsf(y - rhs.y);
            float zDist = fabsf(z - rhs.z);
            const float THRESHOLD = 0.00001f;
            return xDist < THRESHOLD && yDist < THRESHOLD && zDist < THRESHOLD;
        }

        // Inequality comparison.
        bool operator != (const Vector3& rhs) const
        {
            return !(*this == rhs);
        }

        // Returns the vector's magnitude (length).
        float Magnitude() const
        {
            return sqrtf(x * x + y * y + z * z);
        }

        // Returns the squared magnitude (avoids sqrt).
        float MagnitudeSqr() const
        {
            return x * x + y * y + z * z;
        }

        // Normalizes the vector in place.
        void Normalise()
        {
            float m = Magnitude();
            if (m != 0.0f)
            {
                x /= m;
                y /= m;
                z /= m;
            }
        }

        // Returns a normalized copy of the vector.
        Vector3 Normalised() const
        {
            float m = Magnitude();
            if (m != 0.0f)
            {
                return Vector3(x / m, y / m, z / m);
            }
            else
            {
                return Vector3();
            }
        }

        // Scalar multiplication.
        Vector3 operator*(float scalar) const {
            return Vector3(x * scalar, y * scalar, z * scalar);
        }

        // Dot product with another vector.
        float Dot(const Vector3& rhs) const
        {
            return x * rhs.x + y * rhs.y + z * rhs.z;
        }

        // Cross product with another vector.
        Vector3 Cross(const Vector3& rhs) const {
            return Vector3(y * rhs.z - z * rhs.y,
                z * rhs.x - x * rhs.z,
                x * rhs.y - y * rhs.x);
        }

        // Returns a string representation of the vector.
        std::string ToString() const
        {
            return std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z);
        }
    };
};

#endif
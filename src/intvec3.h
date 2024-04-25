#ifndef INTVEC3D_H
#define INTVEC3D_H

#include <ostream>
#include <cmath>
/**
* Defines 3D vectors.
*/
class IntVec3D {
    public:
        // components
        int x, y, z;

        /**
            * Constructor.
            * Initializes tp vector (0,0,0).
            */
        IntVec3D() : x(0), y(0), z(0) { };

        /**
            * Constructor.
            * Initializes to vector (x,y,z).
            */
        IntVec3D(int x, int y, int z) : x(x), y(y), z(z) { }

        /**
            * Constructor.
            * Initializes to vector (c,c,c)
            */
        IntVec3D(int c) : x(c), y(c), z(c) { }

        /**
            * Constructor.
            * Initializes from existing vector
            */
        IntVec3D(const IntVec3D& v) : x(v.x), y(v.y), z(v.z) { }

        // returns reference to the specified component (0-based indexing: x, y, z)
        inline int& operator[] (const int& index) {
            return (&x)[index];
        }

        // returns const reference to the specified component (0-based indexing: x, y, z)
        inline const int& operator[] (const int& index) const {
            return (&x)[index];
        }

        inline bool operator==(const IntVec3D& v) const {
            return v.x == x && v.y == y && v.z == z;
        }

        // negation
        inline IntVec3D operator-(void) const {
            return IntVec3D(-x, -y, -z);
        }

        // addition
        inline IntVec3D operator+(const IntVec3D& v) const {
            return IntVec3D(x + v.x, y + v.y, z + v.z);
        }

        // subtraction
        inline IntVec3D operator-(const IntVec3D& v) const {
            return IntVec3D(x - v.x, y - v.y, z - v.z);
        }

        // right scalar multiplication
        inline IntVec3D operator*(const int& c) const {
            return IntVec3D(x * c, y * c, z * c);
        }

        // scalar division
        inline IntVec3D operator/(const int& c) const {
            const int rc = 1.0 / c;
            return IntVec3D(rc * x, rc * y, rc * z);
        }

        // addition / assignment
        inline void operator+=(const IntVec3D& v) {
            x += v.x; y += v.y; z += v.z;
        }

        // subtraction / assignment
        inline void operator-=(const IntVec3D& v) {
            x -= v.x; y -= v.y; z -= v.z;
        }

        // scalar multiplication / assignment
        inline void operator*=(const int& c) {
            x *= c; y *= c; z *= c;
        }

        // scalar division / assignment
        inline void operator/=(const int& c) {
            (*this) *= (1. / c);
        }

        /**
            * Returns Euclidean length.
            */
        inline int norm(void) const {
            return sqrt(x * x + y * y + z * z);
        }

        /**
            * Returns Euclidean length squared.
            */
        inline int norm2(void) const {
            return x * x + y * y + z * z;
        }

        /**
            * Returns unit vector.
            */
        inline IntVec3D unit(void) const {
            int rNorm = 1. / sqrt(x * x + y * y + z * z);
            return IntVec3D(rNorm * x, rNorm * y, rNorm * z);
        }

        /**
            * Divides by Euclidean length.
            */
        inline void normalize(void) {
            (*this) /= norm();
        }

}; // class IntVec3D
        
typedef IntVec3D vec3;

// left scalar multiplication
inline IntVec3D operator* (const int& c, const IntVec3D& v) {
    return IntVec3D(c * v.x, c * v.y, c * v.z);
}

// dot product (a.k.a. inner or scalar product)
inline int dot(const IntVec3D& u, const IntVec3D& v) {
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

// cross product
inline IntVec3D cross(const IntVec3D& u, const IntVec3D& v) {
    return IntVec3D(u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x);
}

// prints components
std::ostream& operator<<(std::ostream& os, const IntVec3D& v);

#endif // INTVEC3D_H

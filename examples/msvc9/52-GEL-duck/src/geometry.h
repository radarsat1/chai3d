#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "chai3d.h"

inline double squaredDistance(const cVector3d &Y, const cTriangle &T)
{
    cVector3d D0 = T.getVertex1()->getPos() - T.getVertex0()->getPos();
    cVector3d D1 = T.getVertex2()->getPos() - T.getVertex0()->getPos();
    cVector3d Delta = Y - T.getVertex0()->getPos();

    double a00 = D0.lengthsq(), a01 = cDot(D0, D1), a11 = D1.lengthsq();
    double b0 = cDot(D0, Delta), b1 = cDot(D1, Delta);

    double n0 = a11 * b0 - a01 * b1;
    double n1 = a00 * b1 - a01 * b0;
    double d = a00 * a11 - a01 * a01;   // theoretically positive
    double c = Delta.lengthsq();

    double eps = 1e-8;
    if (n0 + n1 <= d) {
        if (n0 >= eps) { // 0.0) {
            if (n1 >= eps) { // 0.0) {
                // region 0 - point is inside the triangle
                return 0.0;
            }
            else {
                // region 5
                if (b0 > eps) { // 0.0) {
                    if (b0 < a00)   return c - b0 * b0 / a00;
                    else            return a00 - 2.0 * b0 + c;
                }
                else                return c;
            }
        }
        else if (n1 >= eps) { // 0.0) {
            // region 3
            if (b1 > eps) { // 0.0) {
                if (b1 < a11)   return c - b1 * b1 / a11;
                else            return a11 - 2.0 * b0 + c;
            }
            else                return c;
        }
        else {
            // region 4
            if (b0 < a00) {
                if (b0 > eps)           return c - b0 * b0 / a00;
                else {
                    if (b1 < a11) {
                        if (b1 > eps)   return c - b1 * b1 / a11;
                        else            return c;
                    }
                    else                return a11 - 2.0 * b1 + c;
                }
            }
            else                        return a00 - 2.0 * b0 + c;
        }
    }
    else if (n0 < -eps) { // 0.0) {
        // region 2
        if (b1 > eps) { // 0.0) {
            if (b1 < a11)       return c - b1 * b1 / a11;
            else {
                double n = a11 - a01 + b0 - b1, d = a00 - 2.0 * a01 + a11;
                if (n > eps) { // 0.0)  {
                    if (n < d)  return (a11 - 2.0 * b1 + c) - n * n / d;
                    else        return a00 - 2.0 * b0 + c;
                }
                else            return a11 - 2.0 * b1 + c;
            }
        }
        else                    return c;
    }
    else if (n1 < -eps) { // 0.0) {
        // region 6
        if (b0 > eps) { // 0.0) {
            if (b0 < a00)       return c - b0 * b0 / a00;
            else {
                double n = a11 - a01 + b0 - b1, d = a00 - 2.0 * a01 + a11;
                if (n > eps) { // 0.0) {
                    if (n < d)  return (a11 - 2.0 * b1 + c) - n * n / d;
                    else        return a00 - 2.0 * b0 + c;
                }
                else            return a11 - 2.0 * b1 + c;
            }
        }
        else                    return c;
    }
    else {
        // region 1
        double n = a11 - a01 + b0 - b1, d = a00 - 2.0 * a01 + a11;
        if (n > eps) { // 0.0) {
            if (n < d)  return (a11 - 2.0 * b1 + c) - n * n / d;
            else        return a00 - 2.0 * b0 + c;
        }
        else            return a11 - 2.0 * b1 + c;
    }
}

double area(const cVector3d &v0, const cVector3d &v1, const cVector3d &v2)
{
    return 0.5 * cCross((v1 - v0), (v2 - v0)).length();
}

cVector3d barycentric(const cVector3d &p, const cTriangle &t)
{
    cVector3d p0 = t.getVertex0()->getPos();
    cVector3d p1 = t.getVertex1()->getPos();
    cVector3d p2 = t.getVertex2()->getPos();

    double a = area(p0, p1, p2);

    cVector3d b;
    if (a != 0.0) {
        b.x = area(p, p1, p2) / a;
        b.y = area(p0, p, p2) / a;
        b.z = area(p0, p1, p) / a;
    }
    return b;
}

#endif

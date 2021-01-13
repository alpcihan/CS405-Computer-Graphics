#ifndef RAY_H
#define RAY_H

#include "helper.h"

class ray {
    public:
        ray() {}
        ray(const glm::dvec3& origin, const glm::dvec3& direction)
            : orig(origin), dir(direction), tm(0)
        {}

        ray(const glm::dvec3& origin, const glm::dvec3& direction, double time)
            : orig(origin), dir(direction), tm(time)
        {}

        glm::dvec3 origin() const  { return orig; }
        glm::dvec3 direction() const { return dir; }
        double time() const    { return tm; }

        glm::dvec3 at(double t) const {
            return orig + t*dir;
        }

    public:
        glm::dvec3 orig;
        glm::dvec3 dir;
        double tm;
};

#endif

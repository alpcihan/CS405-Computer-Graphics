#ifndef CAMERA_H
#define CAMERA_H

#include "helper.h"

class camera {
    public:
        camera(
            glm::dvec3 lookfrom,
            glm::dvec3 lookat,
            glm::dvec3   vup,
            double vfov, // vertical field-of-view in degrees
            double aspect_ratio
        ) {
            auto theta = degrees_to_radians(vfov);
            auto h = tan(theta/2);
            auto viewport_height = 7.0 * h;
            auto viewport_width = aspect_ratio * viewport_height;

            auto w = glm::normalize(lookfrom - lookat);
            auto u = glm::normalize(glm::cross(vup, w));
            auto v = glm::cross(w, u);

            origin = lookfrom;
            horizontal = viewport_width * u;
            vertical = viewport_height * v;
            lower_left_corner = origin - horizontal/2.0 - vertical/2.0 - w;
        }

        ray get_ray(double s, double t) const {
            return ray(origin, lower_left_corner + s*horizontal + t*vertical - origin);
        }

    private:
        glm::dvec3 origin;
        glm::dvec3 lower_left_corner;
        glm::dvec3 horizontal;
        glm::dvec3 vertical;
};

#endif

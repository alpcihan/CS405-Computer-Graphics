#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "helper.h"


class sphere : public hittable {
    public:
        sphere() {}

        sphere(glm::dvec3 cen, double r, shared_ptr<material> m)
            : center(cen), radius(r), mat_ptr(m) {};

        virtual bool hit(
            const ray& r, double tmin, double tmax, hit_record& rec) const override;
    
        virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;
    public:
        glm::dvec3 center;
        double radius;
        shared_ptr<material> mat_ptr;
    
    private:
        static void get_sphere_uv(const glm::dvec3& p, double& u, double& v) {
 
            auto theta = acos(-p.y);
            auto phi = atan2(-p.z, p.x) + pi;

            u = phi / (2*pi);
            v = theta / pi;
        }
};


bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    
    glm::dvec3 oc = r.origin() - center;
    auto a = dvec_length2(r.direction());
    auto half_b = glm::dot(oc, r.direction());
    auto c = dvec_length2(oc) - radius*radius;
    auto discriminant = half_b*half_b - a*c;

    if (discriminant > 0) {
        auto root = sqrt(discriminant);

        auto temp = (-half_b - root) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.at(rec.t);
            glm::dvec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            get_sphere_uv(outward_normal, rec.u, rec.v);
            rec.mat_ptr = mat_ptr;
            return true;
        }

        temp = (-half_b + root) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.at(rec.t);
            glm::dvec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }

    return false;
}

bool sphere::bounding_box(double time0, double time1, aabb& output_box) const {
    output_box = aabb(
        center - glm::dvec3(radius, radius, radius),
        center + glm::dvec3(radius, radius, radius));
    return true;
}


#endif

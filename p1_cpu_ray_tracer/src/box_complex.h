//
//  box_complex.h
//  raytracer2
//
//  Created by Alp Cihan on 9.11.2020.
//  Copyright © 2020 Alp Cihan. All rights reserved.
//

#ifndef box_complex_h
#define box_complex_h

#include "helper.h"

#include "aarect.h"
#include "hittable_list.h"

class box_complex : public hittable  {
    public:
        box_complex() {}
        box_complex(const glm::dvec3& pos, const double a, const double h, shared_ptr<material> ptr);

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

        virtual bool bounding_box(double time0, double time1, aabb& output_box) const override {
            output_box = aabb(pos + glm::dvec3(0,0,-h-a), pos + glm::dvec3(h+a,h+a,0));
            return true;
        }

    public:
        glm::dvec3 pos;
        double a;
        double h;
        hittable_list sides;
};

box_complex::box_complex(const glm::dvec3& coor1, const double a, const double h, shared_ptr<material> ptr) {
        
    glm::dvec3 coor2(coor1.x + a, coor1.y + h, coor1.z + a);
    
    sides.add(make_shared<box>(glm::dvec3(coor1), glm::dvec3(coor2), ptr)); // f-left
    
    sides.add(make_shared<box>(glm::dvec3(coor1 + glm::dvec3(h, 0, 0)), glm::dvec3(coor2 + glm::dvec3(h, 0, 0) ), ptr)); // f-right
    sides.add(make_shared<box>(glm::dvec3(coor1 + glm::dvec3(0, 0, -h)), glm::dvec3(coor2 + glm::dvec3(0, 0, -h) ), ptr)); // b-left
    sides.add(make_shared<box>(glm::dvec3(coor1 + glm::dvec3(h, 0, -h)), glm::dvec3(coor2 + glm::dvec3(h, 0, -h) ), ptr)); // b-right
    
    sides.add(make_shared<box>(glm::dvec3(coor1 + glm::dvec3(0, h, -h)), glm::dvec3(coor2 + glm::dvec3(0, a, 0) ), ptr)); // up-left
    sides.add(make_shared<box>(glm::dvec3(coor1 + glm::dvec3(h, h, -h)), glm::dvec3(coor2 + glm::dvec3(h, a, 0) ), ptr)); // up-right
    
    sides.add(make_shared<box>(glm::dvec3(coor1 + glm::dvec3(0, 0, -h)), glm::dvec3(coor2 + glm::dvec3(0, a-h, 0) ), ptr)); // down-left
    sides.add(make_shared<box>(glm::dvec3(coor1 + glm::dvec3(h, 0, -h)), glm::dvec3(coor2 + glm::dvec3(h, a-h, 0) ), ptr)); // down-right
    
    sides.add(make_shared<box>(glm::dvec3(coor1 + glm::dvec3(0,0,0)), glm::dvec3(coor2 + glm::dvec3(h,a-h,0)), ptr)); // down-front
    sides.add(make_shared<box>(glm::dvec3(coor1 + glm::dvec3(0,0,-h)), glm::dvec3(coor2 + glm::dvec3(h,a-h,-h)), ptr)); // down-back
    
    sides.add(make_shared<box>(glm::dvec3(coor1 + glm::dvec3(0,h,0)), glm::dvec3(coor2 + glm::dvec3(h,a,0)), ptr)); // down-front
    sides.add(make_shared<box>(glm::dvec3(coor1 + glm::dvec3(0,h,-h)), glm::dvec3(coor2 + glm::dvec3(h,a,-h)), ptr)); // down-back
}

bool box_complex::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    return sides.hit(r, t_min, t_max, rec);
}


#endif /* box_complex_h */

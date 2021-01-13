//
//  main.cpp
//  raytracer2
//
//  Created by Alp Cihan on 8.11.2020.
//  Copyright © 2020 Alp Cihan. All rights reserved.
//

#include "glm/glm.hpp"
#include "IO.h"
#include "pixel_buffer.h"

#include "helper.h"

#include "camera.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "aarect.h"
#include "box.h"
#include "box_complex.h"

#include <iostream>


glm::dvec3 ray_color(const ray& r, const glm::dvec3& background, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return glm::dvec3(0,0,0);

    // If the ray hits nothing, return the background color.
    if (!world.hit(r, 0.001, infinity, rec))
        return background;

    ray scattered;
    glm::dvec3 attenuation;
    glm::dvec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;

    return emitted + attenuation * ray_color(scattered, background, world, depth-1);
}

// Take a pixel_buffer and save as .ppm file
void out_ppm(const PixelBuffer & pixel_buffer)
{
    std::ofstream output;
    output.open("./render.ppm", std::ios::out | std::ios::trunc);
    if (!output.is_open())
        return;
    
    std::cout << "Outputting...";
    IO::write_as_PPM(pixel_buffer, output);
    output.close();
    std::cout << "done!" << std::endl;
}


int main() {

    // Image

    const auto aspect_ratio = 640 / 480;
    const int image_width = 640;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 50;
    const int max_depth = 100;
    
    // Pixel Buffer
    
    PixelBuffer pixel_buffer(glm::ivec2(image_width, image_height));

    // World

    hittable_list world;
    
    
    auto mirror = make_shared<metal>(glm::dvec3(0.8, 0.8, 0.8),0.2);
    auto mat_m_high = make_shared<metal>(glm::dvec3(0.3, 0.3, 0.3),0.6);
    
    auto mat_2 = make_shared<metal>(glm::dvec3(0.3, 0.3, 0.3),0.7);
    
    auto mat_d_a = make_shared<dielectric>(1.5);
    
    auto diff_white = make_shared<diffuse_light>(glm::dvec3(1,1,1)); // red
    auto diff_pink = make_shared<diffuse_light>(glm::dvec3(0.7, 0.3, 0.3));

    

    double a = 0.3;
    double h = 5.0;
    
    
    double x;
    double y;
    double z;
    
    auto mat = make_shared<lambertian>(glm::dvec3(0.2, 0.2, 0.2));
    
    world.add(make_shared<xz_rect>(-10, 10, -10, 10, 7, diff_white));
    
    world.add(make_shared<xy_rect>(-10, 10, -10, 10, 10, diff_pink));
    
    //world.add(make_shared<xy_rect>(-100, 100, -20, 20, -10, mat_m_high));
    
    /*int c = 0;
    for(int i = 0; i <= 32; i += 8)
    {
        x = -(a+h)*0.5;
        y = -(a+h)*0.5;
        z = (a+h)*0.5;
        
        shared_ptr<hittable> box1;
        
        if(c== 0)
            box1 = make_shared<box_complex>(glm::dvec3(x,y,z), a, h, mat);
        else if(c == 1)
            box1 = make_shared<box_complex>(glm::dvec3(x,y,z), a, h, mat);
        else if(c == 2)
            box1 = make_shared<box_complex>(glm::dvec3(x,y,z), a, h, mat_2);
        else if(c == 3)
            box1 = make_shared<box_complex>(glm::dvec3(x,y,z), a, h, mat);
        else if(c == 4)
            box1 = make_shared<box_complex>(glm::dvec3(x,y,z), a, h, mirror);

 

        box1 = make_shared<rotate_y>(box1, i);
        world.add(box1);
        
        a -= 0.02;
        h -= 0.6;
        c++;
    }*/
    
    world.add(make_shared<sphere>(glm::dvec3(-1.6,0,0), 0.5, diff_pink));
    world.add(make_shared<sphere>(glm::dvec3(0,0.1,0), 0.65, mat_m_high));
    world.add(make_shared<sphere>(glm::dvec3(1.6,0,0), 0.5, diff_pink));
    world.add(make_shared<sphere>(glm::dvec3(0,-100.5,-1), 100, mat)); // ground
    
    // Camera

    glm::dvec3 lookfrom(0,0,3);
    glm::dvec3 lookat(0,0,-1);
    glm::dvec3 vup(0,1,0);
    glm::dvec3 background(0,0,0); // background

    camera cam(lookfrom, lookat, vup, 20, aspect_ratio);

    // Render

    for (int y = image_height-1; y >= 0; --y)
    {
        std::cerr << "Countdown: " << y << ' ' << std::endl;
        for (int x = 0; x < image_width; ++x)
        {
            glm::dvec3 pixel_color(0,0,0);
            for (int s = 0; s < samples_per_pixel; ++s)
            {
                auto u = (x + random_double()) / (image_width-1);
                auto v = (y + random_double()) / (image_height-1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, background, world, max_depth);
            }
            
            // Grid Super Sampling
            //for(int i = -sqrt(samples_per_pixel)/2; i < sqrt(samples_per_pixel)/2;i++)
            //{
            //    for(int j = -sqrt(samples_per_pixel)/2; j < sqrt(samples_per_pixel)/2; j++)
            //    {
            //        auto u = abs(x + i/sqrt(samples_per_pixel)) / (image_width-1);
            //        auto v = abs(y + j/sqrt(samples_per_pixel)) / (image_height-1);
            //        Ray r = camera.get_ray(u, v);
            //        pixel_color += ray_color(r, world);
            //    }
            //}
            
            
            
            
            return glm::dvec3(vec.x(),vec.y(),vec.z());
            
            glm::dvec3 pixel_c = glm::dvec3(pixel_color.x,pixel_color.y,pixel_color.z);
            
            
            pixel_buffer.set(x,pixel_buffer.dimensions.y - y - 1, sqrt(pixel_c / double(samples_per_pixel)));
        }
    }
    
    out_ppm(pixel_buffer);
    
    std::cerr << "\nDone.\n";
    
    return 0;
}



//
//  pixel_buffer.h
//  raytracer2
//
//  Created by Alp Cihan on 8.11.2020.
//  Copyright © 2020 Alp Cihan. All rights reserved.
//

#ifndef pixel_buffer_h
#define pixel_buffer_h

#pragma once

#include "glm/glm.hpp"

struct PixelBuffer
{
    const glm::ivec2 dimensions;
    const double ratio;
    glm::dvec3 * const pixels;

    PixelBuffer(const glm::ivec2& dimensions) :
        dimensions(dimensions),
        ratio(dimensions.y / dimensions.x),
        pixels(new glm::dvec3[dimensions.x * dimensions.y])
    {}

    ~PixelBuffer()
    {
        delete[] pixels;
    }

    const glm::dvec3& get(unsigned i) const
    {
        return pixels[i];
    }

    const glm::dvec3& get(unsigned x, unsigned y) const
    {
        return pixels[y * dimensions.x + x];
    }

    void set(unsigned x, unsigned y, const glm::dvec3& value) const
    {
        pixels[y * dimensions.x + x] = value;
    }
};



#endif /* pixel_buffer_h */

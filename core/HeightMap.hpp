//
// Created by f0xeri on 09.01.2023.
//

#ifndef VULKAN_TERRAIN_HEIGHTMAP_HPP
#define VULKAN_TERRAIN_HEIGHTMAP_HPP

#include "Texture.hpp"
#include "glm/glm.hpp"

class HeightMap
{
    Texture *texture;
    uint32_t scale;
    uint32_t dim;
public:
    HeightMap(Texture *texture, uint32_t patchsize) : texture(texture) {
        dim = texture->width;
        scale = dim / patchsize;
    }
    ~HeightMap() = default;
    float getHeight(uint32_t x, uint32_t y) {
        glm::ivec2 rpos = glm::ivec2(x, y) * glm::ivec2(scale);
        rpos.x = std::max(0, std::min(rpos.x, (int)dim - 1));
        rpos.y = std::max(0, std::min(rpos.y, (int)dim - 1));
        rpos /= glm::ivec2(scale);
        return *(texture->data + (rpos.x + rpos.y * dim) * scale) / 65535.0f;
    }
};

#endif //VULKAN_TERRAIN_HEIGHTMAP_HPP

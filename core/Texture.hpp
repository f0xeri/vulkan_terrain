//
// Created by f0xeri on 04.01.2023.
//

#ifndef VULKAN_EXPERIMENTS_TEXTURE_HPP
#define VULKAN_EXPERIMENTS_TEXTURE_HPP

#include <iostream>
#include "stb_image.h"

class Texture {

public:
    Texture() = default;
    explicit Texture(const char *name) : name(name) {};
    void loadTextureFromFile(const char *path) {
        stbi_set_flip_vertically_on_load(true);
        data = stbi_load(path, &width, &height, &nrChannels, STBI_rgb_alpha);
        if (data) {

        }
        else {
            std::cout << "Failed to load texture" << std::endl;
        }
    }
    ~Texture(){
        //delete data;
    }

    const char *name;
    int width{}, height{}, nrChannels{}, nrLayers{1};
    unsigned char *data = nullptr;

    // get pixel color
    glm::vec4 getPixelColor(int x, int y) const {
        int index = (y * width + x) * 4;
        return {data[index], data[index + 1], data[index + 2], data[index + 3]};
    }
};


class TextureArray : public Texture {
public:
    explicit TextureArray(const char *name) : Texture(name) {}
    void loadTexturesFromFiles(const std::vector<std::string> &paths) {
        std::vector<unsigned char*> vectoredData;
        for (const auto &path : paths) {
            unsigned char *temp = stbi_load(path.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
            vectoredData.push_back(temp);
        }
        data = new unsigned char[width * height * nrChannels * vectoredData.size()];
        nrLayers = vectoredData.size();
        for (int i = 0; i < vectoredData.size(); i++) {
            memcpy(data + i * width * height * nrChannels, vectoredData[i], width * height * nrChannels);
            delete[] vectoredData[i];
        }
        vectoredData.clear();
    }
};


#endif //VULKAN_EXPERIMENTS_TEXTURE_HPP

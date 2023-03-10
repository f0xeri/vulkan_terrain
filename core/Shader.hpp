//
// Created by f0xeri on 02.01.2023.
//

#ifndef VULKAN_EXPERIMENTS_SHADER_HPP
#define VULKAN_EXPERIMENTS_SHADER_HPP

#include <vector>
#include <string>
#include "DescriptorBinding.hpp"
#include "ShaderStage.hpp"

struct ShaderStageInfo
{
    ShaderStage stage = ShaderStage::VERTEX;
    std::vector<uint32_t>bytecode;
};

struct Constant {
    std::string name;
    uint32_t size;
    uint32_t offset;
    std::vector<ShaderStage> stages;
};

struct Shader {
    std::string name;
    std::vector<ShaderStageInfo> stagesInfo;
    DescriptorBinding descriptorBinding;
    std::vector<Constant> constants;
};

class ShaderLoader
{
public:
    virtual ShaderStageInfo loadFromBinaryFile(const std::string& filepath, ShaderStage stage) = 0;
    virtual ~ShaderLoader() = default;
};

#endif //VULKAN_EXPERIMENTS_SHADER_HPP

//
//  Shader.hpp
//  Chuang
//
//  Created by 孙川 on 2019/10/9.
//  Copyright © 2019 孙川. All rights reserved.
//
#pragma once

#include <string>
#include <unordered_map>

#include "glm/glm.hpp"

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

class Shader {
private:
    std::string m_FilePath;
    unsigned int m_RendererID;
    mutable std::unordered_map<std::string, int> m_UniformLocationCache;
public:
    Shader(const std::string& filename);
    ~Shader();
    
    void Bind() const;
    void Unbind() const;
    
    // Set uniforms
    void SetUniform1i(const std::string& name, int value);
    void SetUniform1f(const std::string& name, float value);
    void SetUniform2f(const std::string& name, const glm::vec2& value);
    void SetUniform3f(const std::string& name, const glm::vec3& value);
    void SetUniform4f(const std::string& name, const glm::vec4& value);
    
    void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
    
private:
    ShaderProgramSource ParseShader(const std::string& filepath);
    unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
    unsigned int CompileShader(unsigned int type, const std::string& source);
    int GetUniformLocation(const std::string& name) const;
};

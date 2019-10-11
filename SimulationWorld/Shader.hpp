//
//  Shader.hpp
//  Chuang
//
//  Created by 孙川 on 2019/10/9.
//  Copyright © 2019 孙川. All rights reserved.
//
#include <string>

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

class Shader {
private:
    std::string m_FilePath;
    unsigned int m_RendererID;
    
    // caching for uniforms
public:
    Shader(const std::string& filename);
    ~Shader();
    
    void Bind() const;
    void Unbind() const;
    
    // Set uniforms
    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    
private:
    ShaderProgramSource ParseShader(const std::string& filepath);
    unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
    unsigned int CompileShader(unsigned int type, const std::string& source);
    int GetUniformLocation(const std::string& name);
};

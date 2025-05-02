#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader {
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath);
    void use() const;

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    std::string loadFile(const char* path);
    void checkCompileErrors(unsigned int shader, const std::string& type);
};

#endif

# OpenGL 学习笔记（光照）

> 后面大量使用 Uniform，为了方便已在 `ShaderProgram` 类实现了 `setFloat` `setVec3` `setMat4` 三个方法用于设置 Uniform 变量

## 重写着色器

光照需要法线

VBO(`main.cpp`)

```
float vertices[] = {
    // pos              // normal           // color
    -0.5f,-0.5f,0.0f,   0,0,1,               1,0,0,
     0.5f,-0.5f,0.0f,   0,0,1,               0,1,0,
     0.5f, 0.5f,0.0f,   0,0,1,               0,0,1,
    -0.5f, 0.5f,0.0f,   0,0,1,               1,1,0
};
```

VAO(`Mesh.cpp`)

```
// position
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *) 0);
glEnableVertexAttribArray(0);

// normal
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *) (3 * sizeof(float)));
glEnableVertexAttribArray(1);

// color
glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *) (6 * sizeof(float)));
glEnableVertexAttribArray(2);
```

`basic.vert`

```
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;

out vec3 FragPos;
out vec3 Normal;
out vec3 Color;

uniform mat4 uMVP;
uniform mat4 uModel;

void main() {
    FragPos = vec3(uModel * vec4(aPos, 1.0));
    Normal  = mat3(transpose(inverse(uModel))) * aNormal;
    Color   = aColor;

    gl_Position = uMVP * vec4(aPos, 1.0);
}
```

`basic.frag`(Phong 着色法)

```
#version 460 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    // 环境光
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * Color;

    // 漫反射
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * Color;

    // 镜面高光
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(1.0);

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
```

需要设置的 Uniform

```
shader.setMat4("uMVP", mvp);
shader.setMat4("uModel", model);
shader.setVec3("lightPos", glm::vec3(2.0f, 2.0f, 2.0f)); // 光源位置
shader.setVec3("viewPos", camera.Position); // 摄像机位置
```
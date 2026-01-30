#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    // ===== 环境光 =====
    vec3 ambient = 0.1 * Color;

    // ===== 漫反射 =====
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * Color;

    // ===== Blinn-Phong 高光 =====
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float shininess = 64.0;// 比 Phong 要大
    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
    vec3 specular = 0.5 * spec * vec3(1.0);

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}

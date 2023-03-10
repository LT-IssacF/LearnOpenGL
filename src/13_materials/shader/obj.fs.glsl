#version 330 core
struct Material {
    vec3 ambient, diffuse, specular;
    float shininess;
}; struct Light {
    vec3 ambient, diffuse, specular, position;
}; in vec3 normal; // 法向量
in vec3 FragPos; // 片元位置
out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main() {
    // 环境光
    vec3 ambient = material.ambient * light.ambient;
    // 漫反射
    vec3 lightDir = normalize(light.position - FragPos); // 光的方向向量
    float diff = max(dot(normal, lightDir), 0.0f); // 计算光线与物体表面法向量的夹角；如果两向量之间的夹角大于90°，点乘的值为负，也对应于现实中无法产生漫反射，于是为0
    vec3 diffuse = diff * material.diffuse * light.diffuse;
    // 高光
    vec3 viewDir = normalize(viewPos - FragPos), reflectDir = reflect(-lightDir, normal); // 观察方向，反射方向（光向需要取反）
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    vec3 specular = spec * material.specular * light.specular;

    FragColor = vec4(ambient + diffuse + specular, 1.0f);
}
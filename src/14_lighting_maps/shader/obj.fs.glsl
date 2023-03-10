#version 330 core
struct Material {
    sampler2D diffuse; // 环境光颜色在几乎所有情况下都等于漫反射颜色
    sampler2D specular;
    // sampler2D matrix; // exercise: emission map
    float shininess;
}; struct Light {
    vec3 ambient, diffuse, specular, position;
}; in vec3 normal; // 法向量
in vec3 FragPos; // 片元位置
in vec2 texCoords;
out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;
// 应用光照贴图
void main() {
    // 环境光 = 材质的漫反射属性 * 光的环境属性
    vec3 ambient = texture(material.diffuse, texCoords).rgb * light.ambient;
    // 漫反射 = 材质的漫反射属性 * 光的漫反射属性 * 反射角
    vec3 lightDir = normalize(light.position - FragPos); // 光的方向向量
    float diff = max(dot(normal, lightDir), 0.0f); // 计算光线与物体表面法向量的夹角；如果两向量之间的夹角大于90°，点乘的值为负，也对应于现实中无法产生漫反射，于是为0
    vec3 diffuse = diff * texture(material.diffuse, texCoords).rgb * light.diffuse;
    // 高光 = 材质的高光属性 * 光的高光属性 * 观察方向与反射方向的夹角
    vec3 viewDir = normalize(viewPos - FragPos), reflectDir = reflect(-lightDir, normal); // 观察方向，反射方向（光向需要取反）
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    vec3 specular = spec * vec3(texture(material.specular, texCoords)) * light.specular;

    FragColor = vec4(ambient + diffuse + specular, 1.0f);
    // FragColor = (vec4(ambient + diffuse + specular, 1.0f)) + texture(material.matrix, texCoords); // exercise: emission map
}
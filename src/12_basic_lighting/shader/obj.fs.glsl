#version 330 core
in vec3 normal; // 法向量
in vec3 FragPos; // 片元的位置
out vec4 FragColor;

uniform vec3 objColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    // 环境光
    float ambientStrenth = 0.1f;
    vec3 ambient = lightColor * ambientStrenth;
    // 漫反射
    vec3 lightDir = normalize(lightPos - normalize(FragPos)); // 光的方向向量
    float diff = max(dot(normal, lightDir), 0.0f); // 计算光线与物体表面法向量的夹角；如果两向量之间的夹角大于90°，点乘的值为负，也对应于现实中无法产生漫反射，于是为0
    vec3 diffuse = lightColor * diff;
    // 高光
    float specularStrenth = 0.5f;
    vec3 viewDir = normalize(viewPos - FragPos), reflectDir = reflect(-lightDir, normal); // 观察方向，反射方向（光向需要取反）
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32); // 32为反光度
    vec3 specular = lightColor * specularStrenth * spec;

    FragColor = vec4(objColor * (ambient + diffuse + specular), 1.0f);
}
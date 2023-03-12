#version 330 core
#define _POINT_LIGHTS 4
// uniform sampler2D texture_diffuse1;
struct Material { // 由于不确定网格到底有几个纹理，所以就干脆各自只定义一个，此处变动，mesh.cpp下对应也需要改
    sampler2D texture_diffuse1;
    // sampler2D texture_diffuse2;
    // sampler2D texture_diffuse3;
    sampler2D texture_specular1;
    // sampler2D texture_specular2;
    // sampler2D texture_specular3;
    float shininess;
}; uniform Material material;
struct DirLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 direction;
}; uniform DirLight dirLight;
struct PoiLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
    float constant;
    float linear;
    float quadratic;
}; uniform PoiLight poiLights[_POINT_LIGHTS];
struct SpotLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
    vec3 direction;
    float cutOff;
    float outerCutOff;
}; uniform SpotLight spotLight;
in vec3 FragPos;
in vec3 normal;
in vec2 texCoords;
out vec4 FragColor;
uniform vec3 viewPos;

vec3 calculateDirLight(vec3 viewDir); // 定向光
vec3 calculatePoiLight(vec3 viewDir, PoiLight light); // 点光源，如果对数组进行操作，就得把单个对象作为参数
vec3 calculateSpotLight(vec3 viewDir); // 手电筒
float calculateAttenuation(float constant, float linear, float quadratic, vec3 position);

void main() {    
    vec3 result, viewDir = normalize(viewPos - FragPos);
    result = calculateDirLight(viewDir);
    for(int i = 0; i < _POINT_LIGHTS; i++) {
        result += calculatePoiLight(viewDir, poiLights[i]);
    } result += calculateSpotLight(viewDir);
    // FragColor = texture(texture_diffuse1, texCoords);
    FragColor = vec4(result, 1.0f);
} vec3 calculateDirLight(vec3 viewDir) {
    // 环境光 = 材质的漫反射属性 * 光的环境属性
    vec3 ambient = texture(material.texture_diffuse1, texCoords).rgb * dirLight.ambient;
    // 漫反射 = 材质的漫反射属性 * 光的漫反射属性 * 反射角
    vec3 lightDir = normalize(-dirLight.direction);
    float diff = max(dot(lightDir, normal), 0.0f);
    vec3 diffuse = texture(material.texture_diffuse1, texCoords).rgb * dirLight.diffuse * diff;
    // 高光 = 材质的高光属性 * 光的高光属性 * 观察方向与反射方向的夹角
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, normalize(reflectDir)), 0.0f), material.shininess);
    vec3 specular = texture(material.texture_specular1, texCoords).rgb * dirLight.specular * spec;
    return ambient + diffuse + specular;
} vec3 calculatePoiLight(vec3 viewDir, PoiLight light) {
    vec3 ambient = texture(material.texture_diffuse1, texCoords).rgb * light.ambient;
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = texture(material.texture_diffuse1, texCoords).rgb * light.diffuse * diff;
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    vec3 specular = texture(material.texture_specular1, texCoords).rgb * light.specular * spec;
    float attenuation = calculateAttenuation(light.constant, light.linear, light.quadratic, light.position);
    return (ambient + diffuse + specular) * attenuation;
} vec3 calculateSpotLight(vec3 viewDir) {
    // 散射光线: 手电筒光源到不同片元的光线
    vec3 scatterDir = viewDir;
    vec3 ambient = texture(material.texture_diffuse1, texCoords).rgb * spotLight.ambient;
    float diff = max(dot(scatterDir, normal), 0.0f);
    vec3 diffuse = texture(material.texture_diffuse1, texCoords).rgb * spotLight.diffuse * diff;
    vec3 reflectDir = reflect(-scatterDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    vec3 specular = texture(material.texture_specular1, texCoords).rgb * spotLight.specular * spec;
    // 衰减
    float attenuation = calculateAttenuation(spotLight.constant, spotLight.linear, spotLight.quadratic, viewPos);
    // 平滑处理
    float theta = dot(normalize(-spotLight.direction), scatterDir);
    float epsilon = spotLight.cutOff - spotLight.outerCutOff;
    float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0f, 1.0f);
    return (ambient + diffuse + specular) * attenuation * intensity;
} float calculateAttenuation(float constant, float linear, float quadratic, vec3 position) {
    float distance = length(position - FragPos);
    return 1.0 / (constant + linear * distance + quadratic * distance * distance);
}

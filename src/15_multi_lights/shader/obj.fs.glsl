#version 330 core
#define _POINT_LIGHTS 4
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; uniform Material material;
/*  公式：光照强度 = 1.0 / （constant + linear * distance + quadratic * distance ^ 2）

    距离 常数项	  一次项	  二次项
    7	 1.0	   0.7	       1.8
    13	 1.0	  0.35	      0.44
    20	 1.0	  0.22	      0.20
    32	 1.0	  0.14	      0.07
    50	 1.0	  0.09	     0.032
    65	 1.0	  0.07	     0.017
    100	 1.0	 0.045	    0.0075
    160	 1.0	 0.027	    0.0028
    200	 1.0	 0.022	    0.0019
    325	 1.0	 0.014	    0.0007
    600	 1.0	 0.007	    0.0002
    3250 1.0	0.0014	  0.000007

    常数项保证分母永远不会比1小，否则在某些距离上光线反而更强显然是不合适的
    一次线性项以线性减少光照强度
    二次项在距离近的时候衰减会不明显，但在远距离时会非常明显 */
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

in vec3 normal; // 法向量
in vec3 FragPos; // 片元位置
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
    FragColor = vec4(result, 1.0f);
} vec3 calculateDirLight(vec3 viewDir) {
    // 环境光 = 材质的漫反射属性 * 光的环境属性
    vec3 ambient = texture(material.diffuse, texCoords).rgb * dirLight.ambient;
    // 漫反射 = 材质的漫反射属性 * 光的漫反射属性 * 反射角
    vec3 lightDir = normalize(-dirLight.direction);
    float diff = max(dot(lightDir, normal), 0.0f);
    vec3 diffuse = texture(material.diffuse, texCoords).rgb * dirLight.diffuse * diff;
    // 高光 = 材质的高光属性 * 光的高光属性 * 观察方向与反射方向的夹角
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, normalize(reflectDir)), 0.0f), material.shininess);
    vec3 specular = texture(material.specular, texCoords).rgb * dirLight.specular * spec;
    return ambient + diffuse + specular;
} vec3 calculatePoiLight(vec3 viewDir, PoiLight light) {
    vec3 ambient = texture(material.diffuse, texCoords).rgb * light.ambient;
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = texture(material.diffuse, texCoords).rgb * light.diffuse * diff;
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    vec3 specular = texture(material.specular, texCoords).rgb * light.specular * spec;
    float attenuation = calculateAttenuation(light.constant, light.linear, light.quadratic, light.position);
    return (ambient + diffuse + specular) * attenuation;
}
vec3 calculateSpotLight(vec3 viewDir) {
    // 散射光线: 手电筒光源到不同片元的光线
    vec3 scatterDir = viewDir;
    vec3 ambient = texture(material.diffuse, texCoords).rgb * spotLight.ambient;
    float diff = max(dot(scatterDir, normal), 0.0f);
    vec3 diffuse = texture(material.diffuse, texCoords).rgb * spotLight.diffuse * diff;
    vec3 reflectDir = reflect(-scatterDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    vec3 specular = texture(material.specular, texCoords).rgb * spotLight.specular * spec;
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
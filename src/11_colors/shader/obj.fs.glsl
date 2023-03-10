#version 330 core
out vec4 FragColor;

uniform vec3 objColor; // 物体颜色
uniform vec3 lightColor; // 光源颜色

void main() {
    FragColor = vec4(objColor * lightColor, 1.0f);
}
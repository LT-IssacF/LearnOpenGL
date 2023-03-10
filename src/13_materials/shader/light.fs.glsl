#version 330 core
out vec4 FragColor;

uniform vec3 lightColor;
uniform float brightness;

void main() {
    FragColor = vec4(lightColor, brightness);
}
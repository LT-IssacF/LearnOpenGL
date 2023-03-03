#version 330 core
out vec4 FragColor;
in vec3 vColor;
uniform vec3 uColor;
void main() {
    FragColor = vec4(vColor + uColor, 0.5f);
}

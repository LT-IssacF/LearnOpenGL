#version 330 core
out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D texture1;

void main() {             
    vec4 texColor = texture(texture1, texCoords);
    if(texColor.a < 0.1)
        discard;
    FragColor = texColor;
}
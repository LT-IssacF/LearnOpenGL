#version 330 core
out vec4 FragColor;

in vec3 myColor;
in vec2 TexCoord;

uniform sampler2D myTexture;
uniform sampler2D myTexture2;
uniform float mixValue;
uniform bool discoStyle;

void main() {
    FragColor = mix(texture(myTexture, TexCoord), texture(myTexture2, TexCoord), mixValue);
    if(discoStyle) {
        FragColor = FragColor * vec4(myColor, 1.0f);
    }
}

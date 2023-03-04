#version 330 core
out vec4 FragColor;

in vec3 myColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;

uniform int index;

void main() {
	// case 1.----------------------
	// FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2f) * vec4(myColor, 1.0f);
	// case 2.----------------------
	if(index == 0) { // container
		FragColor = texture(texture1, TexCoord) * vec4(myColor, 1.0f);
	} else { // awesomeface
		FragColor = texture(texture2, TexCoord);
	}
}
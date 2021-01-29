#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// font bitmap
uniform sampler2D textureFont;

// character coordinates
uniform vec2 charCoords;

const float width = 1./8.;
const float height = 1./8.;

void main() {
    vec2 uv = TexCoord;
    uv = uv/8. + charCoords/8.;
    // use charCoords to get the right character
    FragColor = texture(textureFont, uv);
}
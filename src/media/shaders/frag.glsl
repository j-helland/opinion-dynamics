#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;

// node color
uniform vec3 nodeColor;

void main()
{
    // mess with the texture coords
    vec2 uv = TexCoord;
    vec4 TexColor = texture(texture1, uv);
    // use red of image to color
    if (TexColor.r > 0.0) {
        TexColor.rgb = nodeColor*TexColor.r;
    }
    FragColor = TexColor;
}
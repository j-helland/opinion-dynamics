#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;     // Node
uniform sampler2D texture2;     // Wire

// texture selection
uniform int selection;

// node color
uniform vec3 nodeColor;

void main()
{
    vec2 uv = TexCoord;
    vec4 TexColor = texture(texture1, uv);
    // Node
    if(selection == 1) {
        // use red of image to color
        if (TexColor.r > 0.0) {
            TexColor.rgb = nodeColor*TexColor.r;
        }
    }
    // Wire
    else {
        TexColor = texture(texture2, uv);
    }
    FragColor = TexColor;
}
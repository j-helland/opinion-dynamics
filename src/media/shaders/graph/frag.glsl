#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture sampler
uniform sampler2D textureNode;     // Node
uniform sampler2D textureEdge;     // Edge

// texture selection
uniform int texSelection;

// node selection
uniform int selected;
uniform vec3 highlight;

// node color
uniform vec3 nodeColor;

void main()
{
    vec2 uv = TexCoord;
    vec4 TexColor = texture(textureNode, uv);
    // Node
    if(texSelection == 1) {
        // use red of image to color
        if (TexColor.r > 0.0) {
            TexColor.rgb = nodeColor*TexColor.r;
        }
        // use highlight
        if (selected == 1) {
            TexColor.rgb = mix(nodeColor, highlight, vec3(0.5, 0.5, 0.5));
        }
    }
    // Wire
    else if (texSelection == 2) {
        TexColor = texture(textureEdge, uv);
    }
    FragColor = TexColor;
}
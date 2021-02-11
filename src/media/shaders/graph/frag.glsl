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

// wire pulse
uniform int pulsing; // 0 or 1
uniform float pulse; // in [0, 1]
uniform vec3 pulseColor;

// wire highlighting
uniform float fade;

// node color
uniform vec3 nodeColor;

const float pi = 4.*atan(1.);

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
        // make it fat, yo
        //uv.y = uv.y*.5+.25;
        TexColor = texture(textureEdge, uv);
        // Assimilation Pulse
        if(pulsing == 1) {
            float s = exp(-128.*pow(abs(uv.x-pulse),2.));
            // on-line
            if(TexColor.a > 0.) TexColor.rgba = s * vec4(pulseColor, 1.);
        }
        else {
            // on-line
            if(TexColor.a > 0.) TexColor.a = fade;
        }
    }
    FragColor = TexColor;
}
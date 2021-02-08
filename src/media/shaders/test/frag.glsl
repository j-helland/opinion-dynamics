#version 330 core
out vec4 FragColor;

in vec2 vPos;
in vec2 vOffset;

// Screen (account for resolution)
uniform vec2 resolution;
// Camera
uniform vec3 cameraPos;
// Circle
// uniform vec2 circlePos;
const float circleRadius = 0.25;
const vec3 circleColor = vec3(0.25, 0., 0.);
const float borderWidth = 0.05;

void main()
{
    // Get coordinates, account for aspect
    vec2 uv = cameraPos.z*vec2((vPos.x + cameraPos.x)*resolution.x/resolution.y, vPos.y + cameraPos.y);
    // Get signed distance to circle
    float d = length(uv - vOffset) - circleRadius;
    // Canvas Color (transparent)
    vec4 back = vec4(0.2, 0.3, 0.3, 0.);
    // Node Color
    vec4 front = vec4(circleColor, 1.);
    vec4 color = vec4(0.);
    float eps = 0.01;   // make this a function of true pixel width
    if(d < 0) {
        // Inside Circle
        float s = smoothstep(-borderWidth/2.-eps,-borderWidth/2.+eps,d);
        color = mix(front,vec4(0.,0.,0.,1.),s);
    }
    else {
        // Outside Circle
        float s = smoothstep(borderWidth/2.+eps,borderWidth/2.-eps,d);
        color = mix(back,vec4(0.,0.,0.,1.),s);
    }
    FragColor = color;
}
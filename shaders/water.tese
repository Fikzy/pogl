#version 420 core

layout(triangles, equal_spacing, ccw) in;

out vec4 clipSpace;

uniform float time;

float randHeight(vec2 co)
{
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    // barycentric coordinates
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    float w = gl_TessCoord.z;

    vec4 pos0 = gl_in[0].gl_Position;
    vec4 pos1 = gl_in[1].gl_Position;
    vec4 pos2 = gl_in[2].gl_Position;

    // barycentric interpolation
    clipSpace = u * pos0 + v * pos1 + w * pos2;

    gl_Position = clipSpace;

    // wave effect
    float offset =
        sin(gl_Position.x + time) * 0.1 + sin(gl_Position.z + time) * 0.1;
    // random "bumps" for lowpoly effect
    offset += randHeight(vec2(u)) * 0.1;

    gl_Position.y += offset;
}

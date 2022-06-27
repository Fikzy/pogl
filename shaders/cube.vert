#version 420 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 fragmentColor;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(position, 1);
    fragmentColor = normal * vec3(0.0, 0.0, 1.0);
}

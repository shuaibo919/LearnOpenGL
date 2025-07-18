#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = model * view * projection *  vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
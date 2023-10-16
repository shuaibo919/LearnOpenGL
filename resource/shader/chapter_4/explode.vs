#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec3 Position;

out VS_OUT {
    vec2 texCoords;
}vs_out;
uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;


void main()
{
    vs_out.texCoords = aTexCoords;
    Normal = mat3(transpose(inverse(model))) * aNormal;
    Position = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
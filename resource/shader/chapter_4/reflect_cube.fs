#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform sampler2D texture1;
uniform samplerCube skybox;
uniform float factor;

void main()
{    
    vec4 texColor = texture(texture1,TexCoords);
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    FragColor = vec4(texture(skybox, R).rgb, 1.0) * factor + (1.0 - factor) * texColor;
    
}
#version 330 core
out vec4 FragColor;
uniform vec4 mFragmentColor;
void main()
{
    FragColor = mFragmentColor;
}
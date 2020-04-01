#version 330 core
out vec4 FragColor;

in vec3 ourColor;

uniform vec3 colorOver;

void main()
{
    FragColor = vec4(colorOver, 1.0f);
}
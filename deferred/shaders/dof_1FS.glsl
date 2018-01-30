#version 450 core
out vec4 FragColor;

in float z;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform float focalDistance, focalRange;

void main()
{
    float blur = clamp(abs(focalDistance + z) / focalRange, 0.0, 1.0);
    FragColor = vec4(texture(texture_diffuse1, TexCoords).rgb, blur);
}
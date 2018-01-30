#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out float z;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    vec4 pos = view * model * vec4(aPos, 1.0);
    TexCoords = aTexCoords;
    gl_Position = proj * view * model * vec4(aPos, 1.0);
    z = pos.z;
}

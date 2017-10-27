#version 450

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 vertexUv;
uniform mat4 mvpMatrix;
out vec2 pos;

void main(void)
{
    gl_Position = mvpMatrix * vec4(vertex, 1.0f);
    pos = vertexUv;
}

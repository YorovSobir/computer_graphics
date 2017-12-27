#include "main.h"

const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    1.0f, -1.0f, 0.0f, 1.0, 0.0f,
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
    1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
};

quad1_t::quad1_t(GLuint shader) {
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    GLuint in_pos = 0;
    glEnableVertexAttribArray(in_pos);
    glVertexAttribPointer(in_pos, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);

    GLuint in_uv = 1;
    glEnableVertexAttribArray(in_uv);
    glVertexAttribPointer(in_uv, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

    glBindVertexArray(0);
}

quad1_t::~quad1_t() {
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
}

void quad1_t::draw() {
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

#ifndef UTIL_H
#define UTIL_H

#include <GL/glew.h>

GLuint loadShaders(const char* vertexFilePath, const char* fragmentFilePath);
void loadTexture(const char * fname);

#endif // UTIL_H

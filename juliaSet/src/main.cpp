#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include <cmath>
#include "event.h"
#include "util.h"

static const int width = 640;
static const int height = 480;

static int maxIter = 60;
static float radius = 4.0f;
static float cReal = -0.5f;
static float cImag = 0.3f;

TwBar* bar;
GLuint matrixId;

static GLuint programmId;
static GLuint vertexbuffer;
static GLuint uvbuffer;

void display() {

    GLint loc = glGetUniformLocation(programmId, "c");
    if (loc != -1) {
       glUniform2f(loc, cReal, cImag);
    }

    loc = -1;
    loc = glGetUniformLocation(programmId, "maxIter");
    if (loc != -1) {
        glUniform1i(loc, maxIter);
    }

    loc = -1;
    loc = glGetUniformLocation(programmId, "radius");
    if (loc != -1) {
        glUniform1f(loc, radius);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(programmId);

    glUniformMatrix4fv(matrixId, 1, GL_FALSE, &mvp[0][0]);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        0,
        (void*)0
    );

    changeMatrix();
    glDrawArrays(GL_QUADS, 0, 4);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    TwDraw();

    glutSwapBuffers();
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);

    int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
    int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
    glutInitWindowPosition((screenWidth - width) / 2, (screenHeight - height) / 2);
    glutInitWindowSize(width, height);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutCreateWindow("Julia Set");
    glutDisplayFunc(display);
    glutIdleFunc(display);

    glutMotionFunc(mouseMove);
    glutMouseFunc(mouseButton);
    glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
    glutReshapeFunc(reshape);
    TwGLUTModifiersFunc(glutGetModifiers);

    glewInit();

    // initialize AntTweakBar
    TwWindowSize(width, height);
    TwInit(TW_OPENGL, NULL);

    glBindTexture(GL_TEXTURE_1D, 1);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    loadTexture("./resources/texture");
    glEnable(GL_TEXTURE_1D);

    programmId = loadShaders("./src/vertex.vsh", "src/fragment.fsh");

    static GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f, 1.0f, 0.0f,
         -1.0f, 1.0f, 0.0f
    };

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    static GLfloat uv[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uv), uv, GL_STATIC_DRAW);

    matrixId = glGetUniformLocation(programmId, "mvpMatrix");

    bar = TwNewBar("Julia set parameters");
    TwAddVarRW(bar, "c.real", TW_TYPE_FLOAT, &cReal, "label='c.real' group='point C' step=0.1");
    TwAddVarRW(bar, "c.imag", TW_TYPE_FLOAT, &cImag, "label='c.imag' group='point C' step=0.1");
    TwAddVarRW(bar, "maxIter", TW_TYPE_INT32, &maxIter, "label='max iteration'");
    TwAddVarRW(bar, "radius", TW_TYPE_FLOAT, &radius, "label='radius'");

    glutMainLoop();
    TwTerminate();
    return 0;
}

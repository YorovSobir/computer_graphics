#include "event.h"
#include <GL/glut.h>
#include <glm/gtc/matrix_transform.hpp>

static const float SCALE_FACTOR = 0.90f;
static glm::fvec2 centerStart;
static glm::fvec2 center(0.0f);
static float scale = 1.0f;
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 mvp = glm::mat4(1.0f);

namespace details {
    bool inBarMenu(int x, int y) {
        int barSize[2];
        TwGetParam(bar, NULL, "size", TW_PARAM_INT32, 2, barSize);
        return x < barSize[0] && y < barSize[1];
    }

    glm::fvec2 unproject(const glm::fvec2& win) {
        glm::ivec4 viewPort;
        glGetIntegerv(GL_VIEWPORT, &viewPort[0]);
        return glm::fvec2(glm::unProject(glm::fvec3(win, 0.0f), model, glm::mat4(1.0f), viewPort));
    }
}

void changeMatrix() {
    model = glm::scale(glm::mat4(1.0f), glm::fvec3(scale, scale, 1.0f))
            * glm::translate(glm::mat4(1.0f), glm::fvec3(-center.x, -center.y, 0.0f));
    mvp = model;
}

void mouseWheel(int direction, int x, int y) {
    changeMatrix();
    glm::fvec2 pos = glm::fvec2(x, y);
    glm::fvec2 beforeZoom = details::unproject(pos);
    if (direction == -1) {
        scale *= SCALE_FACTOR;
    } else {
        scale /= SCALE_FACTOR;
    }

    changeMatrix();
    glm::fvec2 afterZoom = details::unproject(pos);
    center += beforeZoom - afterZoom;
    glutPostRedisplay();
}

void mouseMove(int x, int y) {
    if (!details::inBarMenu(x, y)) {
        y = glutGet(GLUT_WINDOW_HEIGHT) - y;
        changeMatrix();
        glm::fvec2 cur = details::unproject(glm::fvec2(x, y));
        center += (centerStart - cur);
        glutPostRedisplay();
    } else {
        TwEventMouseMotionGLUT(x, y);
    }
}

void mouseButton(int button, int state, int x, int y) {
    if (!details::inBarMenu(x, y)) {
        y = glutGet(GLUT_WINDOW_HEIGHT) - y;
        switch (button) {
            case 0: {
                centerStart = details::unproject(glm::fvec2(x, y));
                break;
            }
            case 3: {
                mouseWheel(-1, x, y);
                break;
            }
            case 4: {
                mouseWheel(1, x, y);
                break;
            }
            default:
                break;
        }
        glutPostRedisplay();
    } else {
        TwEventMouseButtonGLUT(button, state, x, y);
    }
}

void reshape(int width, int height) {
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    int position[] = {0, 0};
    TwSetParam(bar, NULL, "position", TW_PARAM_INT32, 2, position);
    int size[] = {200, 120};
    TwSetParam(bar, NULL, "size", TW_PARAM_INT32, 2, size);
    TwWindowSize(width, height);
}

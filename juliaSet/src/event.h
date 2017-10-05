#ifndef EVENT_H
#define EVENT_H
#include <glm/glm.hpp>
#include <AntTweakBar.h>

extern glm::mat4 mvp;
extern TwBar* bar;

void mouseButton(int button, int state, int x, int y);
void mouseMove(int x, int y);
void reshape(int width, int height);
void changeMatrix();

#endif // EVENT_H

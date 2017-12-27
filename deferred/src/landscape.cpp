#include "main.h"

landscape_t::landscape_t(const char* path, GLuint shader)
    : model(utils::Model(path))
    , shader(shader)
{

}

void landscape_t::draw() {
    model.draw(shader);
}

landscape_t::~landscape_t() {

}

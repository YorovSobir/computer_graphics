#pragma once

#include "util.h"
#include "camera.h"
#include <memory>
#include <vector>
#include <AntTweakBar.h>
#include "frame_buffer.h"

using std::vector;
using std::unique_ptr;


struct gbuffer_t;
struct lbuffer_t;
struct obj_t;
struct light_t;

class sample_t
{
public:
    sample_t(Camera& camera);
    ~sample_t();

    void draw_frame();
    void change_mode();

    inline void set_width(int width) {
        this->window_width = width;
    }

    inline void set_height(int height) {
        this->window_height = height;
    }

    Camera camera;
private:

    int mode = 0;

    int window_width, window_height;

    GLuint program1;
    GLuint program2;
    GLuint program3;
    GLuint gauss1;
    GLuint gauss2;

    unique_ptr<obj_t> landscape_;
    unique_ptr<obj_t> quad_;
    float focalDistance;
    float focalRange;
    float radiusScale;
    FrameBuffer	buffer;
    FrameBuffer	buffer2;
    FrameBuffer gauss_buffer1;
    FrameBuffer gauss_buffer2;
    TwBar *bar_;
};

struct obj_t {
    obj_t(){}
    virtual ~obj_t(){}
    virtual void draw(){}
};

struct quad1_t : obj_t {
    quad1_t();
    ~quad1_t();
    void draw();

private:
    GLuint vao_, vbo_;
};

struct landscape_t : obj_t {
    landscape_t(const char* path, GLuint shader);
    ~landscape_t();
    void draw();
private:
    GLuint shader;
    utils::Model model;
};

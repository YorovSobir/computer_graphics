#pragma once

#include "util.h"
#include "camera.h"
#include <memory>
#include <vector>
#include <AntTweakBar.h>

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

    size_t get_additional_lights();
    void set_additional_lights(size_t count);

    inline float get_gamma() {
        return gamma;
    }

    inline void set_gamma(float gamma) {
        this->gamma = gamma;
    }

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
    float gamma = 2.2f;
    float lights_height = -0.15f;
    float lights_speed = 0.9f;

    std::vector<light_t> lights;
    void init_lights();
    void update_lights();
    static void TW_CALL get_lights_callback(void *value, void *clientData);
    static void TW_CALL set_lights_callback(const void *value, void *clientData);

    GLuint gbuffer_shader_;
    GLuint light_shader_;
    GLuint combine_shader_;
    GLuint sphere_shader_;

    unique_ptr<gbuffer_t> gbuffer_;
    unique_ptr<lbuffer_t> lbuffer_;
    unique_ptr<obj_t> quad_;
    unique_ptr<obj_t> sphere_;
    unique_ptr<obj_t> landscape_;

    void draw_gbuffer();
    void draw_lbuffer();
    void draw_combined();
    void draw_sphere_centers();

    TwBar *bar_;
};

GLuint gen_texture(int width, int height, GLint internalFormat, GLenum format, GLenum type);

struct gbuffer_t {
    gbuffer_t(int width, int height);
    ~gbuffer_t();

    GLuint fb_;
    GLuint pos_tex_;
    GLuint norm_tex_;
    GLuint diffuse_tex_;
    GLuint specular_tex_;
private:
    GLuint depth_buf_;
};

struct lbuffer_t {
    lbuffer_t(int width, int height);
    ~lbuffer_t();

    GLuint fb_;
    GLuint light_tex_;
private:
    GLuint depth_buf_;
};


struct obj_t {
    obj_t(){}
    virtual ~obj_t(){}
    virtual void draw(){}
};


struct quad1_t : obj_t {
    quad1_t(GLuint shader);
    ~quad1_t();
    void draw();
private:
    GLuint vao_, vbo_;
};

struct sphere_t : obj_t {
    sphere_t(GLuint shader);
    ~sphere_t();
    void draw();
private:
    GLuint vao_, vbo_, ebo_;
    unsigned faces_;
};


struct landscape_t : obj_t {
    landscape_t(const char* path, GLuint shader);
    ~landscape_t();
    void draw();
private:
    GLuint shader;
    utils::Model model;
};

struct light_t {
    vec3 position;
    GLfloat size;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    bool enabled;

    struct position_params_t {
        float A, B, a, b, d;
        vec3 zero, dx, dy;
    } position_params;
    void update_position(float t);
};

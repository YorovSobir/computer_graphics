#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/vec3.hpp>
#include "camera.h"
#include "main.h"
#include <AntTweakBar.h>

GLFWwindow* window;

sample_t::sample_t(Camera& camera)
    : camera(camera)
{
//    TwInit(TW_OPENGL, nullptr);
//    bar_ = TwNewBar("Parameters");
//    TwDefine("Parameters size='300 160' color='70 100 120' iconpos=topleft valueswidth=70 iconified=true");

    gbuffer_shader_ = utils::loadShaders("../shaders/gBufferVS.glsl", "../shaders/gBufferFS.glsl");
    light_shader_ = utils::loadShaders("../shaders/lightVS.glsl", "../shaders/lightFS.glsl");
    combine_shader_ = utils::loadShaders("../shaders/combineVS.glsl", "../shaders/combineFS.glsl");
    sphere_shader_ = utils::loadShaders("../shaders/sphereVS.glsl", "../shaders/sphereFS.glsl");

    glfwGetWindowSize(window, &window_width, &window_height);

    gbuffer_.reset(new gbuffer_t(window_width, window_height));
    lbuffer_.reset(new lbuffer_t(window_width, window_height));

    quad_.reset(new quad1_t(0));
    sphere_.reset(new sphere_t(0));
    landscape_.reset(new landscape_t("../resources/hill/hills1.obj", gbuffer_shader_));

    init_lights();


//    TwAddVarCB(bar, "additional lights", TW_TYPE_UINT32, set_lights_callback, get_lights_callback, this, "");
//    TwAddVarRW(bar_, "gamma", TW_TYPE_FLOAT, &gamma, "");
//    TwAddVarRW(bar_, "lights height", TW_TYPE_FLOAT, &lights_height, "step=0.05");
//    TwAddVarRW(bar_, "lights speed", TW_TYPE_FLOAT, &lights_speed, "step=0.1");
}

sample_t::~sample_t() {
//    TwDeleteAllBars();
//    TwTerminate();
}

void sample_t::change_mode() {
    ++mode;
    mode %= 3;
}

void sample_t::draw_combined() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glUseProgram(combine_shader_);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gbuffer_->pos_tex_);
    glUniform1i(glGetUniformLocation(combine_shader_, "gbuffer_pos"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gbuffer_->norm_tex_);
    glUniform1i(glGetUniformLocation(combine_shader_, "gbuffer_norm"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gbuffer_->diffuse_tex_);
    glUniform1i(glGetUniformLocation(combine_shader_, "gbuffer_color"), 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, lbuffer_->light_tex_);
    glUniform1i(glGetUniformLocation(combine_shader_, "lbuffer"), 3);

    glUniform1i(glGetUniformLocation(combine_shader_, "mode"), mode);
    glUniform1f(glGetUniformLocation(combine_shader_, "gamma"), gamma);

    quad_->draw();
}

void sample_t::draw_frame() {
    update_lights();

    draw_gbuffer();
    draw_lbuffer();
    draw_combined();
    draw_sphere_centers();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

unique_ptr<sample_t> g_sample;

size_t const default_width = 800;
size_t const default_height = 600;

float lastX = default_width / 2.0f;
float lastY = default_height / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (TwEventMouseMotionGLUT(static_cast<int>(xpos), static_cast<int>(ypos))) {
        return;
    }

    if (firstMouse) {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos - lastX);
    float yoffset = static_cast<float>(lastY - ypos);
    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);
    g_sample->camera.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    g_sample->camera.processMouseScroll(static_cast<float>(yoffset));
}

void window_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (TwEventKeyGLFW(key, action)) {
        return;
    }

    switch (key) {

        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
            break;
        case GLFW_KEY_W:
            g_sample->camera.processKeyboard(FORWARD, deltaTime);
            break;
        case GLFW_KEY_S:
            g_sample->camera.processKeyboard(BACKWARD, deltaTime);
            break;
        case GLFW_KEY_A:
            g_sample->camera.processKeyboard(LEFT, deltaTime);
            break;
        case GLFW_KEY_D:
            g_sample->camera.processKeyboard(RIGHT, deltaTime);
            break;
        case GLFW_KEY_M:
            g_sample->change_mode();
            break;
        case GLFW_KEY_KP_ADD:
        case GLFW_KEY_EQUAL:
            g_sample->set_additional_lights(g_sample->get_additional_lights() + 1);
            break;
        case GLFW_KEY_KP_SUBTRACT:
        case GLFW_KEY_MINUS:
            g_sample->set_additional_lights(g_sample->get_additional_lights() == 0 ? 0 :
                                            g_sample->get_additional_lights() - 1);
            break;
        case GLFW_KEY_G:
            g_sample->set_gamma(g_sample->get_gamma() + 0.2f);
            break;
        case GLFW_KEY_H:
            g_sample->set_gamma(g_sample->get_gamma() - 0.2f);
            break;
    }
}

GLFWwindow* initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(default_width, default_height, "Deferred light",
                                          nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
    glEnable(GL_DEPTH_TEST);

    return window;
}

int main(int argc, char ** argv) {

    window = initWindow();

    Camera camera(glm::vec3(0.0f, 1.0f, 5.0f));
    g_sample.reset(new sample_t(camera));

    while (!glfwWindowShouldClose(window)) {
        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        g_sample->draw_frame();
//        TwDraw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    g_sample.reset(nullptr);

    return 0;
}

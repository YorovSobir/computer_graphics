#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include "main.h"
#include <AntTweakBar.h>
#include "frame_buffer.h"

GLFWwindow* window;

sample_t::sample_t(Camera& camera)
    : camera(camera)
    , focalDistance(4.5)
    , focalRange(20)
    , radiusScale(3.0f / 512)
    , buffer(512, 512, FrameBuffer::depth32)
    , buffer2(512 / 4, 512 / 4)
    , gauss_buffer1(512 / 4, 512 / 4)
    , gauss_buffer2(512 / 4, 512 / 4)
{
    glfwGetWindowSize(window, &window_width, &window_height);
    TwInit(TW_OPENGL_CORE, nullptr);
    TwWindowSize(window_width, window_height);
    bar_ = TwNewBar("Parameters");
    TwDefine("Parameters size='300 160' color='70 100 120' iconpos=topleft valueswidth=70 iconified=true");


    buffer.create();
    buffer.bind();

    if (!buffer.attachColorTexture(GL_TEXTURE_2D, buffer.createColorTexture ( GL_RGBA, GL_RGBA8 ), 0) )
        printf ( "buffer error with color attachment\n");

    if ( !buffer.isOk () )
        printf ( "Error with framebuffer\n" );

    buffer.unbind ();

    buffer2.create ();
    buffer2.bind   ();

    if ( !buffer2.attachColorTexture ( GL_TEXTURE_2D, buffer2.createColorTexture ( GL_RGBA, GL_RGBA8 ), 0 ) )
        printf ( "buffer2 error with color attachment\n");

    if ( !buffer2.isOk () )
        printf ( "Error with framebuffer2\n" );

    buffer2.unbind ();

    gauss_buffer1.create ();
    gauss_buffer1.bind   ();

    if ( !gauss_buffer1.attachColorTexture ( GL_TEXTURE_2D, gauss_buffer1.createColorTexture ( GL_RGBA, GL_RGBA8 ), 0 ) )
        printf ( "gauss buffer 1 error with color attachment\n");

    if ( !gauss_buffer1.isOk () )
        printf ( "Error with gauss buffer 1\n" );

    gauss_buffer1.unbind ();

    gauss_buffer2.create ();
    gauss_buffer2.bind   ();

    if ( !gauss_buffer2.attachColorTexture ( GL_TEXTURE_2D, gauss_buffer2.createColorTexture ( GL_RGBA, GL_RGBA8 ), 0 ) )
        printf ( "gauss buffer 2 error with color attachment\n");

    if ( !gauss_buffer2.isOk () )
        printf ( "Error with gauss buffer 2\n" );

    gauss_buffer2.unbind ();

    program1 = utils::loadShaders("../shaders/dof_1VS.glsl", "../shaders/dof_1FS.glsl");
    program2 = utils::loadShaders("../shaders/dof_2VS.glsl", "../shaders/dof_2FS.glsl");
    program3 = utils::loadShaders("../shaders/dof_2VS.glsl", "../shaders/dof_3FS.glsl");
    gauss1 = utils::loadShaders("../shaders/gauss_blur1VS.glsl", "../shaders/gauss_blur1FS.glsl");
    gauss2 = utils::loadShaders("../shaders/gauss_blur2VS.glsl", "../shaders/gauss_blur2FS.glsl");
    landscape_.reset(new landscape_t("../resources/Small Tropical Island/Small Tropical Island.obj", program1));
    quad_.reset(new quad1_t());

    TwAddVarRW(bar_, "focal distance", TW_TYPE_FLOAT, &focalDistance, "");
    TwAddVarRW(bar_, "focal range", TW_TYPE_FLOAT, &focalRange, "step=0.5");
    TwAddVarRW(bar_, "radius scale", TW_TYPE_FLOAT, &radiusScale, "step=0.5");

}

sample_t::~sample_t() {
    TwDeleteAllBars();
    TwTerminate();
}

void sample_t::change_mode() {
    ++mode;
    mode %= 3;
}

void sample_t::draw_frame() {

    buffer.bind   ();
    glUseProgram(program1);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUseProgram(program1);
    glUniform1f(glGetUniformLocation(program1, "focalDistance"), focalDistance);
    glUniform1f(glGetUniformLocation(program1, "focalRange"), focalRange);
    mat4 model;
    model = translate(model, vec3(0.0f, -5.0f, -20.0f));
    model = scale(model, vec3(0.07));

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)512 / 512,
                                            0.1f, 100.0f);

    glUniformMatrix4fv(glGetUniformLocation(program1, "model"), 1, GL_FALSE, value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(program1, "view"), 1, GL_FALSE, value_ptr(camera.getViewMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(program1, "proj"), 1, GL_FALSE, value_ptr(projection));
    landscape_->draw();
    glUseProgram(0);
    buffer.unbind( true );

    glUseProgram(program2);
    buffer2.bind    ();
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(program2, "tex"), 0);
    glBindTexture(GL_TEXTURE_2D, buffer.getColorBuffer());

    glDisable   (GL_DEPTH_TEST);
    glDepthMask (GL_FALSE);
    quad_->draw();

    // restore matrix
    glEnable    ( GL_DEPTH_TEST );
    glDepthMask ( GL_TRUE );

    buffer2.unbind(true);
    glUseProgram(0);


    glUseProgram(gauss1);
    gauss_buffer1.bind();

    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(gauss1, "tex"), 0);
    glBindTexture(GL_TEXTURE_2D, buffer2.getColorBuffer());

    glUniform1i(glGetUniformLocation(gauss1, "width"), 512);
    quad_->draw();

    gauss_buffer1.unbind(true);
    glUseProgram(0);

    glUseProgram(gauss2);
    gauss_buffer2.bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(gauss2, "tex"), 0);
    glBindTexture(GL_TEXTURE_2D, gauss_buffer1.getColorBuffer());

    glUniform1i(glGetUniformLocation(gauss2, "height"), 512);

    quad_->draw();

    gauss_buffer2.unbind(true);
    glUseProgram(0);

    glUseProgram(program3);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(program3, "texLow"), 1);
    glBindTexture      ( GL_TEXTURE_2D, gauss_buffer2.getColorBuffer () );

    glActiveTexture( GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(program3, "tex"), 0);
    glBindTexture      ( GL_TEXTURE_2D, buffer.getColorBuffer () );

    glUniform1f(glGetUniformLocation(program3, "radiusScale"), radiusScale);

    glDisable   ( GL_DEPTH_TEST );
    glDepthMask ( GL_FALSE );
    quad_->draw();
    // restore matrix
    glEnable    ( GL_DEPTH_TEST );
    glDepthMask ( GL_TRUE );

    glUseProgram(0);

}

unique_ptr<sample_t> g_sample;
bool view_mode = true;

void toggle_mode() {
    view_mode = !view_mode;
    if (view_mode) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        TwDefine("Parameters iconified=true");
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        TwDefine("Parameters iconified=false");
    }
}


size_t const default_width = 512;
size_t const default_height = 512;

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = default_width / 2.0f;
float lastY = default_height / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (!view_mode) {
        if (TwEventMouseMotionGLUT(static_cast<int>(xpos), static_cast<int>(ypos))) {
            return;
        }
    } else {
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

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    g_sample->camera.processMouseScroll(static_cast<float>(yoffset));
}

void window_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    TwWindowSize(width, height);
    g_sample->set_height(height);
    g_sample->set_width(width);
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
        case GLFW_KEY_M: {
            if (action == GLFW_PRESS) {
                g_sample->change_mode();
            }
            break;

        }
        case GLFW_KEY_Q: {
            if (action == GLFW_PRESS) {
                toggle_mode();
            }
            break;
        }
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
    glfwSetMouseButtonCallback(window, reinterpret_cast<GLFWmousebuttonfun>(TwEventMouseButtonGLFW));
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
    freopen("/dev/null", "w", stderr);
    window = initWindow();

    g_sample.reset(new sample_t(camera));

    while (!glfwWindowShouldClose(window)) {
        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        g_sample->draw_frame();
        TwDraw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    g_sample.reset(nullptr);

    return 0;
}

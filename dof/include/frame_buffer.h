#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class FrameBuffer {
    int		flags;
    int		width;
    int		height;
    GLuint	frameBuffer;					// id of framebuffer object
    GLuint	colorBuffer [8];				// texture id or buffer id
    GLuint	depthBuffer;
    GLuint	stencilBuffer;					//
    GLint	saveViewport [4];				// saved viewport setting during bind

public:
    FrameBuffer  ( int theWidth, int theHeight, int theFlags = 0 );
    ~FrameBuffer ();

    inline int getWidth() const {
        return width;
    }

    inline int getHeight() const {
        return height;
    }

    inline unsigned getColorBuffer(int no = 0) const {
        return colorBuffer [no];
    }

    bool isOk() const;
    bool create();
    bool bind();
    bool unbind(bool genMipmaps = false,  GLenum target = GL_TEXTURE_2D );

    bool	attachColorTexture   ( GLenum target, unsigned texId, int no = 0 );

                                                                            // create texture for attaching
    unsigned	createColorTexture     ( GLenum format = GL_RGBA, GLenum internalFormat = GL_RGBA8,
                                         GLenum clamp = GL_REPEAT, int filter = filterLinear );

   enum								// flags for depth and stencil buffers
    {
        depth16 = 1,					// 16-bit depth buffer
        depth24 = 2,					// 24-bit depth buffer
        depth32 = 4,					// 32-bit depth buffer

        stencil1  = 16,					// 1-bit stencil buffer
        stencil4  = 32,					// 4-bit stencil buffer
        stencil8  = 64,					// 8-bit stencil buffer
        stencil16 = 128					// 16-bit stencil buffer
    };

    enum								// filter modes
    {
        filterNearest = 0,
        filterLinear  = 1,
        filterMipmap  = 2
    };
};
#endif // FRAME_BUFFER_H

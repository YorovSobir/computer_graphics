#include "frame_buffer.h"

FrameBuffer::FrameBuffer(int theWidth, int theHeight, int theFlags)
    : width(theWidth)
    , height(theHeight)
    , flags(theFlags)
    , frameBuffer(0)
    , depthBuffer(0)
    , stencilBuffer(0)
{
    for (int i = 0; i < 8; i++) {
        colorBuffer [i] = 0;
    }
}

bool FrameBuffer::create() {
    if (width <= 0 || height <= 0) {
        return false;
    }

    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    int	depthFormat   = 0;
    int stencilFormat = 0;

    if (flags & depth16)
        depthFormat = GL_DEPTH_COMPONENT16;
    else
    if (flags & depth24)
        depthFormat = GL_DEPTH_COMPONENT24;
    else
    if (flags & depth32)
        depthFormat = GL_DEPTH_COMPONENT32;

    if (flags & stencil1)
        stencilFormat = GL_STENCIL_INDEX1;
    else
    if (flags & stencil4)
        stencilFormat = GL_STENCIL_INDEX4;
    else
    if (flags & stencil8)
        stencilFormat = GL_STENCIL_INDEX8;
    else
    if (flags & stencil16)
        stencilFormat = GL_STENCIL_INDEX16;


    if (depthFormat != 0) {
        glGenRenderbuffers(1, &depthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, depthFormat, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,  GL_DEPTH_ATTACHMENT,
                                  GL_RENDERBUFFER, depthBuffer);
    }

    if (stencilFormat != 0) {
        glGenRenderbuffers(1, &stencilBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, stencilBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, stencilFormat, width, height);
        glFramebufferRenderbuffer (GL_FRAMEBUFFER,  GL_STENCIL_ATTACHMENT,
                                       GL_RENDERBUFFER, stencilBuffer);
    }

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return status == GL_FRAMEBUFFER_COMPLETE;
}

FrameBuffer :: ~FrameBuffer ()
{
    if ( depthBuffer != 0 )
        glDeleteRenderbuffers( 1, &depthBuffer );

    if ( stencilBuffer != 0 )
        glDeleteRenderbuffers( 1, &stencilBuffer );

    if ( frameBuffer != 0 )
        glDeleteFramebuffers( 1, &frameBuffer );
}

bool FrameBuffer::isOk() const {
    GLint currentFb;

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFb);

    if (currentFb != frameBuffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
    }

    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (currentFb != frameBuffer)
            glBindFramebuffer(GL_FRAMEBUFFER, currentFb);

    return status == GL_FRAMEBUFFER_COMPLETE;
}

bool FrameBuffer::bind () {
    if (frameBuffer == 0)
        return false;

    glFlush ();
                                                            // save current viewport
    glGetIntegerv(GL_VIEWPORT, saveViewport);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glViewport(0, 0, getWidth (), getHeight ());

    return true;
}

bool FrameBuffer::unbind(bool genMipmaps, GLenum target) {
    if (frameBuffer == 0)
        return false;

    glFlush();

    if (genMipmaps)
        glGenerateMipmap ( target );

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(saveViewport[0], saveViewport[1], saveViewport[2], saveViewport[3]);

    return true;
}

bool FrameBuffer::attachColorTexture(GLenum target, unsigned texId, int no) {
    if (frameBuffer == 0)
        return false;

    if (target != GL_TEXTURE_2D && target != GL_TEXTURE_RECTANGLE &&
            (target < GL_TEXTURE_CUBE_MAP_POSITIVE_X || target > GL_TEXTURE_CUBE_MAP_NEGATIVE_Z))
        return false;

    glBindTexture(target, colorBuffer[no] = texId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + no, target, texId, 0 );

    return true;
}

unsigned FrameBuffer::createColorTexture(GLenum format, GLenum internalFormat,
                                         GLenum clamp, int filter)
{
        GLuint	tex;

        glGenTextures   ( 1, &tex );
        glBindTexture   ( GL_TEXTURE_2D, tex );
        glPixelStorei   ( GL_UNPACK_ALIGNMENT, 1 );                         // set 1-byte alignment
        glTexImage2D    ( GL_TEXTURE_2D, 0, internalFormat, getWidth (), getHeight (), 0,
                      format, GL_UNSIGNED_BYTE, NULL );

        if ( filter == filterNearest )
        {
                glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
                glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        }
        else
        if ( filter == filterLinear )
        {
                glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
                glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        }
        else
        if ( filter == filterMipmap )
        {
                glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
                glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        }

    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp );

    return tex;
}

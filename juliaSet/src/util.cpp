#include <vector>
#include <stdlib.h>
#include <cstdio>

#include <GL/glew.h>
#include "util.h"

namespace details {
    char* readShader(const char* path) {
        FILE *fp;
        unsigned int  len;
        char *src_buf;

        if(!(fp = fopen(path, "r"))) {
            fprintf(stderr, "failed to open shader: %s\n", path);
            return 0;
        }
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        src_buf = (char*)malloc(len + 1);

        fread(src_buf, 1, len, fp);
        src_buf[len] = 0;
        return src_buf;
    }
}

GLuint loadShaders(const char* vertex, const char* fragment) {
    unsigned int prog, vsdr, fsdr;
    int success, linked;
    char* src_buf;

    src_buf = details::readShader(vertex);
    vsdr = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    glShaderSourceARB(vsdr, 1, (const char**)&src_buf, 0);
    free(src_buf);

    src_buf = details::readShader(fragment);
    fsdr = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
    glShaderSourceARB(fsdr, 1, (const char**)&src_buf, 0);
    free(src_buf);

    glCompileShaderARB(vsdr);
    glGetObjectParameterivARB(vsdr, GL_OBJECT_COMPILE_STATUS_ARB, &success);
    if(!success) {
        int info_len;
        char *info_log;

        glGetObjectParameterivARB(vsdr, GL_OBJECT_INFO_LOG_LENGTH_ARB, &info_len);
        if(info_len > 0) {
            if(!(info_log = (char*)malloc(info_len + 1))) {
                perror("malloc failed");
                return 0;
            }
            glGetInfoLogARB(vsdr, info_len, 0, info_log);
            fprintf(stderr, "shader compilation failed: %s\n", info_log);
            free(info_log);
        } else {
            fprintf(stderr, "shader compilation failed\n");
        }
        return 0;
    }

    glCompileShaderARB(fsdr);
    glGetObjectParameterivARB(fsdr, GL_OBJECT_COMPILE_STATUS_ARB, &success);
    if(!success) {
        int info_len;
        char *info_log;

        glGetObjectParameterivARB(fsdr, GL_OBJECT_INFO_LOG_LENGTH_ARB, &info_len);
        if(info_len > 0) {
            if(!(info_log = (char*)malloc(info_len + 1))) {
                perror("malloc failed");
                return 0;
            }
            glGetInfoLogARB(fsdr, info_len, 0, info_log);
            fprintf(stderr, "shader compilation failed: %s\n", info_log);
            free(info_log);
        } else {
            fprintf(stderr, "shader compilation failed\n");
        }
        return 0;
    }

    prog = glCreateProgramObjectARB();
    glAttachObjectARB(prog, vsdr);
    glAttachObjectARB(prog, fsdr);
    glLinkProgramARB(prog);
    glGetObjectParameterivARB(prog, GL_OBJECT_LINK_STATUS_ARB, &linked);
    if(!linked) {
        fprintf(stderr, "shader linking failed\n");
        return 0;
    }

    glUseProgramObjectARB(prog);
    return prog;
}

#define PACK_COLOR24(r, g, b) (((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff))

void loadTexture(const char * fname) {
    FILE* fp = fopen(fname, "r");
    if (!fp) {
        printf("file %s not found\n", fname);
        exit(-1);
    }
    std::vector<uint32_t> pixels;
    for (int i = 0; i < 256; ++i) {
        int r, g, b;
        fscanf(fp, "%d %d %d\n", &r, &g, &b);
        pixels.push_back(PACK_COLOR24(r, g, b));
    }
    fclose(fp);

    glBindTexture(GL_TEXTURE_1D, 1);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, pixels.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glEnable(GL_TEXTURE_1D);
}

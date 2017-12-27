#include <vector>
#include <stdlib.h>
#include <cstdio>

#include <glad/glad.h>
#include <iostream>
#include "../include/util.h"

namespace utils {
    namespace details {
        char* readShader(const char* path) {
            FILE *fp;
            long len;
            char *src_buf;

            if(!(fp = fopen(path, "r"))) {
                fprintf(stderr, "failed to open shader: %s\n", path);
                return 0;
            }
            fseek(fp, 0, SEEK_END);
            len = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            src_buf = (char*)malloc(static_cast<size_t>(len + 1));

            fread(src_buf, 1, static_cast<size_t>(len), fp);
            src_buf[len] = 0;
            return src_buf;
        }
    }

    GLuint loadShaders(const char* vertex, const char* fragment) {
        unsigned int prog, vsdr, fsdr;
        int success, linked;
        char* src_buf;

        src_buf = details::readShader(vertex);
        vsdr = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vsdr, 1, (const char**)&src_buf, nullptr);
        free(src_buf);

        src_buf = details::readShader(fragment);
        fsdr = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fsdr, 1, (const char**)&src_buf, 0);
        free(src_buf);

        glCompileShader(vsdr);
        glGetShaderiv(vsdr, GL_COMPILE_STATUS, &success);
        if(!success) {
            int info_len;
            char info_log[512];

            glGetShaderInfoLog(vsdr, info_len, nullptr, info_log);
            fprintf(stderr, "shader compilation failed: %s\n", info_log);
            return 0;
        }

        glCompileShader(fsdr);
        glGetShaderiv(fsdr, GL_COMPILE_STATUS, &success);
        if(!success) {
            int info_len = 512;
            char info_log[info_len];

            glGetShaderInfoLog(fsdr, info_len, nullptr, info_log);
            fprintf(stderr, "shader compilation failed: %s\n", info_log);
            return 0;
        }

        prog = glCreateProgram();
        glAttachShader(prog, vsdr);
        glAttachShader(prog, fsdr);
        glLinkProgram(prog);
        glGetProgramiv(prog, GL_LINK_STATUS, &linked);
        if(!linked) {
            fprintf(stderr, "shader linking failed\n");
            return 0;
        }

        glDeleteShader(vsdr);
        glDeleteShader(fsdr);
        return prog;
    }

    Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
            : vertices(vertices)
            , indices(indices)
    {
        setupMesh();
    }

    void Mesh::setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                     &indices[0], GL_STATIC_DRAW);

        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        glBindVertexArray(0);
    }

    void Mesh::draw() {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }


    Model::Model(const char *path) {
        loadModel(path);
    }

    void Model::draw() {
        for(size_t i = 0; i < meshes.size(); i++) {
            meshes[i].draw();
        }
    }

    void Model::loadModel(const std::string &path) {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate |
                                                       aiProcess_FlipUVs | aiProcess_GenNormals);
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << importer.GetErrorString() << std::endl;
            return;
        }
        processNode(scene->mRootNode, scene);
    }

    void Model::processNode(aiNode *node, const aiScene *scene) {
        for(size_t i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh));
        }
        for(size_t i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh Model::processMesh(aiMesh *mesh) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex{};
            glm::vec3 vector;
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
            vertices.push_back(vertex);
        }

        for(size_t i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for(size_t j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
        return Mesh(vertices, indices);
    }
}

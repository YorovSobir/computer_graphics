#include <vector>
#include <stdlib.h>
#include <cstdio>

#include <iostream>
#include "util.h"
#include "stb_image.h"

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

    Mesh::Mesh(const std::vector<Vertex>& vertices,
               const std::vector<unsigned int>& indices,
               const std::vector<Texture>& textures)
            : _vertices(vertices)
            , _indices(indices)
            , _textures(textures)
    {
        setupMesh();
    }

    void Mesh::setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), _vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int),
                     &_indices[0], GL_STATIC_DRAW);

        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        glBindVertexArray(0);
    }

    void Mesh::draw(unsigned int shaderId) {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLE_STRIP, static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }


    Model::Model(const char *path) {
        loadModel(path);
    }

    void Model::draw(unsigned int shaderId) {
        for(size_t i = 0; i < _meshes.size(); i++) {
            _meshes[i].draw(shaderId);
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
        _directory = path.substr(0, path.find_last_of('/'));
        processNode(scene->mRootNode, scene);
    }

    void Model::processNode(aiNode *node, const aiScene *scene) {
        for(size_t i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            _meshes.push_back(processMesh(mesh, scene));
        }
        for(size_t i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh Model::processMesh(aiMesh *mesh, const aiScene* scene) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
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
            if (mesh->mTextureCoords[0]) {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            } else {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }
            vertices.push_back(vertex);
        }

        for(size_t i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for(size_t j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        // 1. diffuse maps
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        return Mesh(vertices, indices, textures);
    }

    std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                         std::string typeName) {
        std::vector<Texture> textures;
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            if (_loaded_textures.find(str.C_Str()) != _loaded_textures.end()) {

                    textures.push_back(_loaded_textures[str.C_Str()]);
            } else {
                Texture texture{};
                texture.id = TextureFromFile(str.C_Str(), _directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                _loaded_textures.insert(std::make_pair(texture.path, texture));
            }
        }
        return textures;
    }

    unsigned int TextureFromFile(const char *path, const std::string &directory)
    {
        std::string filename = std::string(path);
        filename = directory + '/' + filename;

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data) {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }
}

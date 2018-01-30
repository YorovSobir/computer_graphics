#ifndef UTIL_H
#define UTIL_H
#include <vector>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glad/glad.h>

using namespace glm;
namespace utils {
    GLuint loadShaders(const char* vertexFilePath, const char* fragmentFilePath);
    unsigned int TextureFromFile(const char *path, const std::string &directory);

    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    struct Texture {
        unsigned int id;
        std::string type;
        std::string path;
    };

    class Mesh {
    public:
        std::vector<Vertex> _vertices;
        std::vector<unsigned int> _indices;
        std::vector<Texture> _textures;
        Mesh(const std::vector<Vertex>& vertices,
             const std::vector<unsigned int>& indices,
             const std::vector<Texture>& textures);
        void draw(unsigned int shaderId);
    private:
        unsigned int VAO, VBO, EBO;
        void setupMesh();
    };

    class Model {
    public:
        Model() {}
        explicit Model(const char *path);
        void draw(unsigned int shaderId);
    private:
        std::unordered_map<std::string, Texture> _loaded_textures;
        std::vector<Mesh> _meshes;
        std::string _directory;
        void loadModel(const std::string& path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene* scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial *mat,
                                             aiTextureType type, std::string typeName);
    };

    struct light_t {
        vec3 position;
        GLfloat size;
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        bool enabled;

        struct position_params_t {
            double A, B, a, b, d;
            vec3 zero, dx, dy;
        } position_params;
        void update_position(double t);
    };
}

#endif // UTIL_H

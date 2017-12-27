#version 450 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform sampler2D shadowMap;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

float calcShadow() {

    // perform perspective divide
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;

    // Because the depth from the depth map is in the range [0,1] we
    // and we also want to use projCoords to sample from the depth map
    // so we transform the NDC coordinates to the range [0,1]
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    // for shadow acne add bias
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = 0.0;

    // percentage-closer filtering to get soft looking shadow
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 0.7 : 0.0;
        }
    }
    shadow /= 9.0;
    if(projCoords.z > 1.0) {
        shadow = 0.0;
    }
    return shadow;
}

vec3 calcPhong() {
    // calc ambient using light.ambient and material.ambient
    vec3 ambient = light.ambient * material.ambient;
    vec3 norm = normalize(Normal);

    // calc diffuse
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // calc specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    return (ambient + diffuse + specular);
}

void main() {
    vec3 color = vec3(0.7, 0.7, 0.7);
    vec3 normal = normalize(Normal);
    vec3 lightColor = vec3(0.3);
    vec3 ambient = 0.15 * color;
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;
    float shadow = calcShadow();
    vec3 ph = calcPhong();
    vec3 lighting = ((ambient + (1.0 - shadow) * (diffuse + specular)) * ph) + ph;
    FragColor = vec4(lighting, 1.0);
}
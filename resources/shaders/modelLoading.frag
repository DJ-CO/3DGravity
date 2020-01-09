#version 330 core
#define MAX_NUMBER_OF_LIGHTS 20
#define POINT 0
#define DIRECTIONAL 1
#define SPOT 2

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 colour;

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};


struct DirLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};


struct Light
{
    vec3 position;// Location
    vec3 diffuse;// RGB of diffuse
    vec3 ambient;// RGB of ambient
    vec3 specular;// RGE of specular
    vec3 direction;// Components of direction
    float constant;// Amount of constant light
    float linear;// Amount of linear falloff
    float quadratic;// Amount of quadratic falloff
    float cutOff;
    float outerCutOff;
    int type;// The type of light: 0 for point light, 1 for directional light, 2 for spot light
};

uniform vec3 viewPos;
uniform sampler2D texture_diffuse;
uniform Material material;

uniform DirLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;
uniform Light light[MAX_NUMBER_OF_LIGHTS];
uniform int NUMBER_OF_LIGHTS;

// Function prototypes
vec3 CalcDirLight (Light light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight (Light light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight (Light light, vec3 normal, vec3 fragPos, vec3 viewDir);


void main ( )
{
    vec3 norm = normalize ( Normal );
    vec3 viewDir = normalize( viewPos - FragPos );

    vec3 result = vec3 (0.0f, 0.0f, 0.0f);

    for (int i = 0; ((i < MAX_NUMBER_OF_LIGHTS)&&(i<NUMBER_OF_LIGHTS)); i++)
    {
        if (light[i].type == POINT) result+= CalcPointLight (light[i], norm, FragPos, viewDir);
        if (light[i].type == DIRECTIONAL) result+= CalcPointLight (light[i], norm, FragPos, viewDir);
        if (light[i].type == SPOT) result+= CalcPointLight (light[i], norm, FragPos, viewDir);
    }

    colour = vec4 (result, 1.0);
}


vec3 CalcDirLight (Light light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir),0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir,reflectDir),0.0), material.shininess);
    vec3 ambient = light.ambient* vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular,TexCoords));

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight (Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir),0.0);

    vec3 reflectDir = reflect(-lightDir, normal);

    float spec = pow(max(dot(viewDir,reflectDir),0.0), material.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance*distance));


    vec3 ambient = light.ambient* vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular,TexCoords));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight (Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir),0.0);

    vec3 reflectDir = reflect(-lightDir, normal);

    float spec = pow(max(dot(viewDir,reflectDir),0.0), material.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance*distance));

    float theta = dot (lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta-light.outerCutOff)/ epsilon, 0.0, 1.0);


    vec3 ambient = light.ambient* vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular,TexCoords));

    ambient *= attenuation *intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);

}




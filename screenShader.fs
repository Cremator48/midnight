#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColorSpec;
uniform sampler2D ssao;

struct Light {
    vec3 Position;
    vec3 Color;

    float Linear;
    float Quadratic;
};

uniform Light lights;

void main()
{   
    // Получаем данные из G-буфера
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Color = texture(gColorSpec, TexCoords).rgb;
    float AmbientOcclusion = texture(ssao, TexCoords).r;
    
    vec3 ambient = vec3(0.1 * Color * AmbientOcclusion);
    vec3 lighting = ambient;
   
    vec3 lightDir = normalize(lights.Position - FragPos);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Color * lights.Color;

    float distance = length(lights.Position - FragPos);
    float attenuation = 1.0 / (1.0 + lights.Linear * distance + lights.Quadratic * distance * distance);
    diffuse *= attenuation;


    lighting += diffuse;

    FragColor = vec4(lighting, 1.0);
//      FragColor = vec4(FragPos, 1.0);
}
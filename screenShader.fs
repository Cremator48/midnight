#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColorSpec;

struct Light {
    vec3 Position;
    vec3 Color;
};

const int NR_LIGHTS = 3;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

void main()
{   
    // Получаем данные из G-буфера
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Color = texture(gColorSpec, TexCoords).rgb;
    float Specular = texture(gColorSpec, TexCoords).a;

    
    vec3 lighting = Color * 0.1; 
    vec3 viewDir = normalize(viewPos - FragPos);
    for(int i = 0; i < NR_LIGHTS; ++i)
    {
        
        vec3 lightDir = normalize(lights[i].Position - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Color * lights[i].Color;
        lighting += diffuse;
    }
    FragColor = vec4(lighting, 1.0);

    // FragColor = texture(screenTexture, TexCoords);
}
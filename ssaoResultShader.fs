#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColorSpec;
uniform sampler2D ssao;
uniform sampler2D gWorldPosition;
uniform samplerCube depthMap;


uniform float far_plane;




struct Light {
    vec3 Position;
    vec3 Color;

    float Linear;
    float Quadratic;
};

uniform Light lights;

float ShadowCalculation(vec3 fragPos)
{
    vec3 fragToLight = fragPos - lights.Position; 
    float closestDepth = texture(depthMap, fragToLight).r;

    closestDepth *= far_plane;

    float currentDepth = length(fragToLight);

    float bias = 0.05; 
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

//    FragColor = vec4(vec3(closestDepth / far_plane), 1.0);

    return shadow;
    
}

void main()
{   
    // Получаем данные из G-буфера
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 WorldPos = texture(gWorldPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Color = texture(gColorSpec, TexCoords).rgb;
    float ssaoTexture = texture(ssao, TexCoords).r;
    
    vec3 ambient = vec3(0.1 * Color * ssaoTexture);
  
    vec3 lightDir = normalize(lights.Position - FragPos);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Color * lights.Color;

    float distance = length(lights.Position - FragPos);
    float attenuation = 1.0 / (1.0 + lights.Linear * distance + lights.Quadratic * distance * distance);
    diffuse *= attenuation;

    float shadow = ShadowCalculation(WorldPos);                      
    vec3 lighting = (ambient + (1.0 - shadow) * diffuse);

    FragColor = vec4(lighting, 1.0);
//    FragColor = vec4((ambient + diffuse), 1.0);

//      FragColor = vec4(WorldPos, 1.0);
    
}
#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
} fs_in;

struct PointLight {    
    vec3 position;
 
    vec3 color;
};  

uniform sampler2D texture_diffuse1;
uniform vec3 viewPos;

#define NR_POINT_LIGHTS 4  
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{           
    vec3 norm = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
 
    vec3 result = vec3(0.0, 0.0, 0.0);

    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, fs_in.FragPos, viewDir); 
    
    FragColor = vec4(result, 1.0);

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

}
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
 
    
    float diff = max(dot(normal, lightDir), 0.0);
  
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (distance * distance);    
     
    vec3 ambient = vec3(texture(texture_diffuse1, fs_in.TexCoords));
    vec3 diffuse = diff * vec3(texture(texture_diffuse1, fs_in.TexCoords));
 

    ambient *= attenuation;
    diffuse *= attenuation;

    return ((ambient  * light.color + diffuse  * light.color));
} 


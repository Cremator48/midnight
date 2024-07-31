#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
} fs_in;

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  
 
    vec3 color;
    float volume;
};  

uniform sampler2D texture_diffuse1;
uniform vec3 viewPos;

#define NR_POINT_LIGHTS 3  
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

}
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
 
    
    float diff = max(dot(normal, lightDir), 0.0);
 
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
 
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
     
    vec3 ambient = vec3(texture(texture_diffuse1, fs_in.TexCoords));
    vec3 diffuse = diff * vec3(texture(texture_diffuse1, fs_in.TexCoords));
    vec3 specular = spec * vec3(0.1, 0.1, 0.1);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 colorOfLight = light.color * light.volume;

    return ((ambient + diffuse + specular) * colorOfLight);
} 


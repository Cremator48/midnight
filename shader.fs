#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
}; 

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  


struct SpotLight {
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

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  
 
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

in vec3 Normal;  
in vec3 FragPos;
in vec2 TexCoords;
uniform int isFlashlightEnable;
uniform int isDirLightEnable;
uniform int isPointLightEnable;

uniform vec3 viewPos;

uniform SpotLight spotLight;
uniform PointLight pointLight;
uniform DirLight dirLight;

uniform Material material;


vec3 calcDirLight(DirLight dirLight, vec3 normal, vec3 viewDir, Material material);
vec3 calcSpotLight(SpotLight spotLight, vec3 norm, vec3 viewDir, Material material);
vec3 calcPointLight(PointLight pontLight, vec3 normal, vec3 fragPos, vec3 viewDir, Material material);

void main()
{    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result;

    result = vec3(0.0f, 0.0, 0.0);

    if(isDirLightEnable == 1)
    {
        result += calcDirLight(dirLight, norm, viewDir, material);
    }
    
    if(isFlashlightEnable == 1)
    {
        result += calcSpotLight(spotLight, norm, viewDir, material);
    }

    if(isPointLightEnable == 1)
    {
        result += calcPointLight(pointLight, norm, FragPos, viewDir, material);
    }
    
    FragColor = vec4(result, 1.0);
} 

vec3 calcSpotLight(SpotLight spotLight, vec3 norm, vec3 viewDir, Material material)
{

    vec3 ambient = spotLight.ambient * texture(material.texture_diffuse1, TexCoords).rgb;
    

    
    vec3 lightDir = normalize(spotLight.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = spotLight.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;  

    
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spotLight.specular * spec * texture(material.texture_specular1, TexCoords).rrr;  
    

    float theta = dot(lightDir, normalize(-spotLight.direction)); 
    float epsilon = (spotLight.cutOff - spotLight.outerCutOff);
    float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;
    

    float distance = length(spotLight.position - FragPos);
    float attenuation = 1.0 / (spotLight.constant + spotLight.linear * distance + spotLight.quadratic * (distance * distance));    
    ambient *= attenuation; 
    diffuse *= attenuation;
    specular *= attenuation;   
        
    return (ambient + diffuse + specular);
}


vec3 calcPointLight(PointLight pontLight, vec3 normal, vec3 fragPos, vec3 viewDir, Material material)
{
    vec3 lightDir = normalize(pontLight.position - fragPos);
 
    // Рассеянное затенение
    float diff = max(dot(normal, lightDir), 0.0);
 
    // Отраженное затенение
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
 
    // Затухание
    float distance = length(pontLight.position - fragPos);
    float attenuation = 1.0 / (pontLight.constant + pontLight.linear * distance + pontLight.quadratic * (distance * distance));    
 
    // Комбинируем результаты
    vec3 ambient = pontLight.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse = pontLight.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = pontLight.specular * spec * vec3(texture(material.texture_specular1, TexCoords).rrr);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 calcDirLight(DirLight dirLight, vec3 normal, vec3 viewDir, Material material)
{
    vec3 lightDir = normalize(-dirLight.direction);
 
    // Рассеянное затенение
    float diff = max(dot(normal, lightDir), 0.0);
 
    // Отраженное затенение
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
 
    // Комбинируем результаты
    vec3 ambient = dirLight.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse = dirLight.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = dirLight.specular * spec * vec3(texture(material.texture_specular1, TexCoords).rrr);
    return (ambient + diffuse + specular);
}
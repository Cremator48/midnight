#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
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

uniform vec3 viewPos;

uniform SpotLight spotLight;
uniform PointLight pointLight;

uniform Material material;

vec3 calcSpotLight(SpotLight spotLight, vec3 norm, vec3 viewDir);
vec3 calcPointLight(PointLight pontLight, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result;

    result = vec3(0.0f, 0.0, 0.0);

    
    if(isFlashlightEnable == 1.0f)
    {
        result += calcSpotLight(spotLight, norm, viewDir);
    }

    result += calcPointLight(pointLight, norm, FragPos, viewDir);
    
    FragColor = vec4(result, 1.0);
} 

vec3 calcSpotLight(SpotLight spotLight, vec3 norm, vec3 viewDir)
{

    vec3 ambient = spotLight.ambient * texture(material.diffuse, TexCoords).rgb;
    

    
    vec3 lightDir = normalize(spotLight.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = spotLight.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  

    
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spotLight.specular * spec * texture(material.specular, TexCoords).rgb;  
    

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


vec3 calcPointLight(PointLight pontLight, vec3 normal, vec3 fragPos, vec3 viewDir)
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
    vec3 ambient = pontLight.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = pontLight.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = pontLight.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

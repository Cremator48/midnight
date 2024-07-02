#version 330 core
out vec4 FragColor;

uniform samplerCube depthMap;
uniform sampler2D texture_diffuse1;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float far_plane;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;





float ShadowCalculation(vec3 fragPos)
{
    vec3 fragToLight = fragPos - lightPos; 

    float closestDepth = texture(depthMap, fragToLight).r;

    closestDepth *= far_plane;

    float currentDepth = length(fragToLight);

   float bias = 0.05; 

   float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0; 
   //FragColor = vec4(vec3(closestDepth / far_plane), 1.0);
   
    return shadow;


}

void main()
{    

    vec3 color = texture(texture_diffuse1, TexCoords).rgb;
	vec3 normal = normalize(Normal);
    vec3 lightColor = vec3(1.0);
    
    vec3 ambient = 0.3 * color;
	
    
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(viewPos - FragPos);
    float spec = 0.0;   
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;
    
    float shadow = ShadowCalculation(FragPos);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    FragColor = vec4(lighting, 1.0);
} 


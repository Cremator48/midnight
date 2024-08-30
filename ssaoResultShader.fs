#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColorSpec;
uniform sampler2D ssao;
uniform sampler2D gWorldPosition;
uniform sampler2D gWorldNormal;
uniform samplerCube depthMap;



uniform float far_plane;
// uniform float exposure;
uniform vec3 viewPos;
uniform bool shadows;
uniform int samples;




vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);  

struct Light {
    vec3 Position;
    vec3 PositionView;
    vec3 Color;

    float Linear;
    float Quadratic;
};

uniform Light lights;

float ShadowCalculation(vec3 fragPos, vec3 normal, vec3 lightDir)
{
    vec3 fragToLight = fragPos - lights.Position; 
    //  float closestDepth = texture(depthMap, fragToLight).r;

    //  closestDepth *= far_plane;

    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.15;
    

    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= far_plane;
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }

    //  FragColor = vec4(vec3(closestDepth / far_plane), 1.0); 
    shadow /= float(samples);  
    return shadow;
}

void main()
{   
    const float gamma = 2.2;

    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 WorldPos = texture(gWorldPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 WorldNormal = texture(gWorldNormal, TexCoords).rgb;

    vec3 Color = texture(gColorSpec, TexCoords).rgb;

    float ssaoTexture = texture(ssao, TexCoords).r;

//    Color =* exposure;

//    vec3 ldrColor = vec3(1.0) - exp(-Color * exposure);

//    ldrColor = pow(ldrColor, vec3(1.0 / gamma));

    vec3 ambient = vec3(0.3 * ssaoTexture * Color);
  
    vec3 lightDir = normalize(lights.PositionView - FragPos);
    vec3 lightWorldDir = normalize(lights.Position - WorldPos);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * lights.Color  * Color;

    float distance = length(lights.PositionView - FragPos);
    float attenuation = 1.0 / (1.0 + lights.Linear * distance + lights.Quadratic * distance * distance);
    diffuse *= attenuation;

    float shadow = shadows ? ShadowCalculation(WorldPos, WorldNormal, lightWorldDir) : 0.0;                 
    vec3 lighting = (ambient + (1.0 - shadow) * diffuse);


    FragColor = vec4(lighting, 1.0);
}
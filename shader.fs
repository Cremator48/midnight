#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec2 TexCoord;
in vec3 FragPos;  
flat in ivec4 BoneIDs;
in vec4 Weights;
  
uniform vec3 lightPos; 
uniform sampler2D diffuse_texture;
uniform int glDisplayBoneIndex;

void main()
{
//    vec3 lightColor = vec3(1.0);
    vec3 lightColor = texture(diffuse_texture, TexCoord.xy).xyz;

    
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
            
    vec3 result = (ambient + diffuse) * vec3(1.0);
  FragColor = vec4(result, 1.0);


    bool find = false;

    for(int i = 0; i < 4; i++)
    {
        if(BoneIDs[i] == glDisplayBoneIndex)
        {
            if(Weights[i] >= 0.7)
            {
   //             FragColor = vec4(1.0, 0.0, 0.0, 0.0) * Weights[i];
            } else if(Weights[i] >= 0.4)
            {
   //             FragColor = vec4(0.0, 1.0, 0.0, 0.0) * Weights[i];
            } else if(Weights[i] >= 0.1)
            {
    //            FragColor = vec4(1.0, 1.0, 0.0, 0.0) * Weights[i];
            }

            find = true;
            break;
        }
    }

    if(!find)
    {
     //   FragColor =  texture(diffuse_texture, TexCoord.xy) * vec4(0.00001) + vec4(0.0, 0.0, 1.0, 0.0);
    }
} 
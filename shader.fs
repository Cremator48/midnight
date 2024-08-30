#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedoSpec;
layout (location = 3) out vec3 gWorldPosition;
layout (location = 4) out vec3 gWorldNormal;

in VS_OUT {
    vec3 FragPos;
    vec3 WorldPos;
    vec2 TexCoords;
    vec3 Normal;
    vec3 WorldNormal;
} fs_in;

uniform sampler2D texture_diffuse0;

void main()
{           
    
    gPosition = fs_in.FragPos;
    gWorldPosition = fs_in.WorldPos;
 
    
    gNormal = normalize(fs_in.Normal);
    gWorldNormal = normalize(fs_in.WorldNormal);
 
    
    gAlbedoSpec.rgb = texture(texture_diffuse0, fs_in.TexCoords).rgb;

    gWorldPosition.rgb = fs_in.WorldPos;
//    gAlbedoSpec.rgb = vec3(0.95);
}
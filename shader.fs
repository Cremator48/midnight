#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedoSpec;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
} fs_in;

uniform sampler2D texture_diffuse0;

void main()
{           
    
    gPosition = fs_in.FragPos;
 
    
    gNormal = normalize(fs_in.Normal);
 
    gAlbedoSpec.rgb = texture(texture_diffuse0, fs_in.TexCoords).rgb;
//    gAlbedoSpec.rgb = vec3(0.95);
}
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeights;


out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;


const int MAX_BONES = 100;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 gBones[MAX_BONES];

void main()
{
    mat4 BoneTransform =  gBones[aBoneIDs[0]] * aWeights.x;
    BoneTransform      += gBones[aBoneIDs[1]] * aWeights.y;
    BoneTransform      += gBones[aBoneIDs[2]] * aWeights.z;
    BoneTransform      += gBones[aBoneIDs[3]] * aWeights.w;


    
    TexCoord = aTexCoord;
            
    mat4 wvp = projection * view * model;

    vec4 PosL = BoneTransform * vec4(aPos, 1.0);
    gl_Position = wvp * PosL;

}
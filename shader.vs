#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeights;


out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;
flat out ivec4 BoneIDs;
out vec4 Weights;

const int MAX_BONES = 100;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 gBones[MAX_BONES];

void main()
{
    mat4 BoneTransform =  gBones[aBoneIDs[0]] * aWeights[0];
    BoneTransform      += gBones[aBoneIDs[1]] * aWeights[1];
    BoneTransform      += gBones[aBoneIDs[2]] * aWeights[2];
    BoneTransform      += gBones[aBoneIDs[3]] * aWeights[3];


    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal; 
    TexCoord = aTexCoord;
    BoneIDs = aBoneIDs;
    Weights= aWeights;
    


 //   gl_Position = projection * view * vec4(FragPos, 1.0);
    vec4 PosL = BoneTransform * vec4(aPos, 1.0);
    gl_Position = projection * view * model * PosL;

}
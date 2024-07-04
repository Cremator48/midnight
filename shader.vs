#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;
out vec4 FragPosLightSpace;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform bool reverse_normals;


void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = aTexCoords;  
    // Normal = transpose(inverse(mat3(model))) * aNormal;

     if(reverse_normals)
     {
        Normal = transpose(inverse(mat3(model))) * (-1.0 * aNormal);
     }
    else
     {
        Normal = transpose(inverse(mat3(model))) * aNormal;
     }
    
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 offsets;

out VS_OUT {
    vec2 TexCoords;
    vec3 Normal;
    vec3 FragPos;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{

    vs_out.TexCoords = aTexCoords;  

    vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;


    vs_out.FragPos = vec3(model * vec4(aPos + offsets, 1.0)); 
    
    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}
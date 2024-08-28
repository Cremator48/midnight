#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 WorldPos;
    vec2 TexCoords;
    vec3 Normal;
    vec3 WorldNormal;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform bool inverseNormals;

void main()
{
    vec4 viewPos = view * model * vec4(aPos, 1.0);
    vs_out.WorldPos =  vec3(model * vec4(aPos, 1.0));
    vs_out.FragPos = viewPos.xyz; 
    vs_out.TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(view * model)));
    if(inverseNormals == true)
    {
        vs_out.Normal = normalMatrix * (-1.0 * aNormal);
        vs_out.WorldNormal = transpose(inverse(mat3(model))) * (-1.0 * aNormal);
    }
    else
    {
        vs_out.Normal = normalMatrix * aNormal;
        vs_out.WorldNormal = transpose(inverse(mat3(model))) * aNormal;
    }
    
    
    gl_Position = projection * viewPos;
}


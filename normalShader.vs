#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
 
out VS_OUT {
    vec3 normal;
    mat3 TBN;
} vs_out;
 
uniform mat4 view;
uniform mat4 model;
 
void main()
{ 
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));

    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    vs_out.TBN = transpose(mat3(T, B, N));

    gl_Position = view * model * vec4(aPos, 1.0); 
   
    vs_out.normal = normalize(vec3(vec4(normalMatrix * aNormal, 0.0)));
}
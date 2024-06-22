#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec2 TexCoords;
    vec3 Normal;
    vec3 FragPos;
} gs_in[]; 


out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

 
void main() {    

    gl_Position = gl_in[0].gl_Position;
    TexCoords = gs_in[0].TexCoords;
    Normal = gs_in[0].Normal;
    FragPos = gs_in[0].FragPos;
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    TexCoords = gs_in[1].TexCoords;
    Normal = gs_in[1].Normal;
    FragPos = gs_in[1].FragPos;
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    TexCoords = gs_in[2].TexCoords;
    Normal = gs_in[2].Normal;
    FragPos = gs_in[2].FragPos;
    EmitVertex();

    EndPrimitive();

}
#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;
 
in VS_OUT {
    vec3 normal;
    mat3 TBN;
} gs_in[];

out GS_OUT {
    mat3 TBN1;
} fs_out;
 
const float MAGNITUDE = 0.4;
  
uniform mat4 projection;
 
void GenerateLine(int index)
{
    gl_Position = projection * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = projection * (gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}
 
void main()
{
    fs_out.TBN1 = gs_in[0].TBN;
    fs_out.TBN1 = gs_in[1].TBN;
    fs_out.TBN1 = gs_in[2].TBN;

    GenerateLine(0); // нормаль первой вершины
    GenerateLine(1); // нормаль второй вершины
    GenerateLine(2); // нормаль третьей вершины
} 
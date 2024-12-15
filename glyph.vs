#version 330 core
layout (location = 0) in vec4 VertexCoordsPlusTextureCoords;

out vec2 TexCoords;
 
void main()
{
    gl_Position = vec4(vec2(VertexCoordsPlusTextureCoords.xy), 0.0, 1.0);
    TexCoords = vec2(VertexCoordsPlusTextureCoords.z, VertexCoordsPlusTextureCoords.w);
}  
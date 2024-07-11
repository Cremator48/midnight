#version 330 core
out vec4 FragColor;
 
in GS_OUT {
    mat3 TBN1;
} fs_in;

void main()
{
    FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}  
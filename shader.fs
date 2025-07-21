 #version 330 core
 out vec4 FragColor;

 in vec2 TexCoord;

 uniform sampler2D diffuse_texture;

 void main()
 {
    // vec3 lightColor = texture(diffuse_texture, TexCoord.xy).xyz;

    FragColor = vec4(texture(diffuse_texture, TexCoord.xy).xyz, 1.0);

} 
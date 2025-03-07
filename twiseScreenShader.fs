#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;

uniform sampler2D ssaoScreenTexture;
uniform float exposure;

void main()
{   

    vec3 ssaoScreenTextureVec = texture(ssaoScreenTexture, TexCoords).rgb;

    ssaoScreenTextureVec *= exposure;

    FragColor = vec4(ssaoScreenTextureVec, 1.0);

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
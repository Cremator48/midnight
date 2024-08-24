#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D usualScreenTexture;
uniform sampler2D blurScreenTexture;

void main()
{   
    vec3 hdrColor = texture(usualScreenTexture, TexCoords).rgb;
    vec3 blur = texture(blurScreenTexture, TexCoords).rgb;
    vec3 result = hdrColor + blur;

    FragColor = vec4(result, 1.0);
}
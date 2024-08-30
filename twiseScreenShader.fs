#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;

uniform sampler2D ssaoScreenTexture;

const float offset = 1.0 / 300.0;
// uniform float sinTime;


void main()
{   

    vec2 offsetsTF[100];
    int numOfArray = 0;

    for(float row = -5; row < 5; ++row)
    {
        for(float column = -5; column < 5; ++column)
        {
            offsetsTF[numOfArray] = vec2(offset * row, offset * column);
            numOfArray += 1;
        }
    }

    float maxColorOfPixel[100];

    for(int i = 0; i < 100; ++i)
    {
        vec3 ColorOfPixel = texture(ssaoScreenTexture, vec2(0.5, 0.5) + offsetsTF[i]).rgb;
        maxColorOfPixel[i] = max(max(ColorOfPixel.r, ColorOfPixel.g), ColorOfPixel.b);
    }

    float middleColor = 0;
    for(int i = 0; i < 100; ++i)
    {
        middleColor += maxColorOfPixel[i];
    }

    middleColor /= 100.0;

    float exposure = 1.0;

    while(middleColor > 1.0)
    {
        middleColor *= exposure;
        exposure -= 0.1;
    }

    if(middleColor > 0.9)
    {
        // exposure *= clamp(abs(sinTime * 0.5), 0.3, 10.0);
    }

    
    vec3 ssaoScreenTextureVec = texture(ssaoScreenTexture, TexCoords).rgb;

    ssaoScreenTextureVec *= exposure;

    FragColor = vec4(ssaoScreenTextureVec, 1.0);

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
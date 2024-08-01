#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform vec3 color;

void main()
{           
    FragColor = vec4(color * 100, 1.0);
    float brightness = dot(FragColor.rgb, vec3(1.0, 1.0, 1.0));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
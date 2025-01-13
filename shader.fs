﻿#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos; 

void main()
{
    vec3 lightColor = vec3(1.0);

    
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
            
    vec3 result = (ambient + diffuse) * vec3(1.0);
    FragColor = vec4(result, 1.0);
} 
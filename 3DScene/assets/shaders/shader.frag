#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D tex;
uniform vec3 keyLightDir; 
uniform vec3 keyLightColor;
uniform vec3 viewPos; // Camera's position

void main() {
    /*
    *
    * Phong lighting model calculations
    *
    */ 
    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * keyLightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, keyLightDir), 0.0); 
    vec3 diffuse = diff * keyLightColor;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-keyLightDir, norm); 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * keyLightColor;

    // Combine the results
    vec3 objectColor = texture(tex, TexCoord).rgb;
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}

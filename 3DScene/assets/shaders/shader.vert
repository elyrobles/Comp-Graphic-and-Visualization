#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color; 
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 uv;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main(){
    gl_Position = projection * view * model * vec4(position, 1.0);
    FragPos = vec3(model * vec4(position, 1.0)); // World position of vertex
    Normal = normalize(mat3(transpose(inverse(model))) * normal); // Transform normal to world space and normalize
    TexCoord = uv;
}

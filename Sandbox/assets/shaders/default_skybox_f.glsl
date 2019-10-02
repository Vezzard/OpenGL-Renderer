#version 330 core
out vec4 color;

struct Material {
    samplerCube diffuse;
};

in vec2 v_TexCoord;

uniform Material u_material;

void main()
{   
    color = texture(u_material.diffuse, v_TexCoord);
}
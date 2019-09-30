#version 330 core
out vec4 color;

struct Material {
    sampler2D diffuse;
};

in vec2 v_TexCoord;

uniform Material u_material;

uniform float u_dbgPPOffset;
uniform int u_dbgPPEffect;

void main()
{   
	float offset = u_dbgPPOffset;
	vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    float kernel1[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );
    
	float kernel2[9] = float[](
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16  
	);

    float kernel3[9] = float[](
         1,  1,  1,
         1, -8,  1,
         1,  1,  1
    );

	if (u_dbgPPEffect == 0){
		color = texture(u_material.diffuse, v_TexCoord);
		return;
	}

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(u_material.diffuse, v_TexCoord.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++) {
		float k;
		if (u_dbgPPEffect == 1) {
			k = kernel1[i];
		} 
		if (u_dbgPPEffect == 2) {
			k = kernel2[i];
		}
		if (u_dbgPPEffect == 3) {
			k = kernel3[i];
		}

        col += sampleTex[i] * k;
	}

    color = vec4(col, 1.f);
}
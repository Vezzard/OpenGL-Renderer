#version 330 core
			
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Bitangent;
layout(location = 4) in vec2 a_UV;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 v_TexCoord;
out vec3 v_FragPos;
out vec3 v_Normal;

void main()
{
	v_TexCoord	= a_UV;
	v_FragPos	= vec3(u_Transform * vec4(a_Position, 1.0f));
	v_Normal	= inverse(transpose(mat3(u_Transform))) * a_Normal;
	gl_Position = u_ViewProjection * vec4(v_FragPos, 1.0);	
}
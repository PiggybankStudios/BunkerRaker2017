#version 130

uniform mat4 WorldMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform vec4 SourceRectangle;

in vec3 inPosition;
in vec4 inColor;
in vec2 inTexCoord;

out vec3 fPosition;
out vec4 fColor;
out vec2 fTexCoord;

void main()
{
	fColor = inColor;
	fTexCoord = SourceRectangle.xy + (inTexCoord * SourceRectangle.zw);
	mat4 transformMatrix = ProjectionMatrix * (ViewMatrix * WorldMatrix);
	gl_Position = transformMatrix * vec4(inPosition, 1.0);
	fPosition = gl_Position.xyz;
}
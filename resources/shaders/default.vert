#version 460
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;

out vec3 color;

uniform mat4 viewProjectionMat;
uniform mat4 modelMat;

void main()
{
	color = vertexColor;
	gl_Position = viewProjectionMat * modelMat * vec4(vertexPosition, 1.0);
}
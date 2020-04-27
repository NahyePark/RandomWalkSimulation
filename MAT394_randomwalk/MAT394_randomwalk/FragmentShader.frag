#version 330 core

out vec3 FragColor;

//in vec3 ourColor;
//in vec3 FragPos;
//in vec3 Normal;

uniform vec3 color;

void main(){

	// Output color = color specified in the vertex shader, 
	// interpolated between all 3 surrounding vertices
	FragColor = color;

}
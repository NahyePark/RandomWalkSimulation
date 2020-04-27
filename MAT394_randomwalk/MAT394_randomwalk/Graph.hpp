
/* Start Header -------------------------------------------------------
File Name: Graph.hpp
Purpose: Random walk class, sphere, axes
Language: C++
Platform: MSVC2019 window
Project: Random Walk Simulation
Author: Nahye Park
Creation date: 03/26/2020
End Header --------------------------------------------------------*/
#pragma once

#ifndef GRAPH_HPP
#define GRAPH_HPP

#define TRIALS 1000

enum LineType {
	X,
	Y,
	Z,
	NONE
};

class Sphere {
public:
	Sphere();
	void Draw(GLuint shader, glm::vec3 ViewPos);

	glm::mat4 m_Model;
	glm::mat4 m_Projection;
	glm::mat4 m_View;
	glm::vec3 color;

	glm::vec3 scale, center;
private:

	std::vector<glm::vec3> m_vertices;
	std::vector<GLuint> m_indices;

	GLuint m_VertexArray;
	GLuint m_vertexbuffer;
	GLuint m_elementbuffer;
	unsigned m_elementSize;
};

class Line {
public:
	Line();
	Line(GLfloat _size, LineType _direction, glm::vec3 _color);
	
	void Draw(GLuint shader, glm::vec3 ViewPos);
	void SetStartEnd(glm::vec3 _s, glm::vec3 _e) 
	{ 
		m_line.front() = _s;
		m_line.back() = _e;
	}

	void Clear();

	glm::mat4 m_Model;
	glm::mat4 m_Projection;
	glm::mat4 m_View;
	glm::vec3 color;

private:
	GLfloat m_size;
	LineType m_direction;
	std::vector<glm::vec3> m_line;

	GLuint m_VertexArray;
	GLuint m_vertexbuffer;

};

enum Direction {
	X_UP,
	Y_UP,
	Z_UP,
	X_DOWN,
	Y_DOWN,
	Z_DOWN
};

class RandomWalk {

public:
	RandomWalk() {
		points.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
		points.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
		startPosition = points.front();
		steps = 0;
		looperased = false;
		num_loop = 0;
		size_loop = 0;
		biggest_loop = 0;
		loop_exist = false;
		
		limit = false;
		limit_min = glm::vec3(-200, -200, -200);
		limit_max = glm::vec3(200, 200, 200);
	}

	void Walk();
	void RemoveLast();
	float Distance();
	void Reset();
	void CheckLoop();

	
	std::vector<glm::vec3> points;
	int steps;
	glm::vec3 startPosition;
	glm::vec3 limit_min;
	glm::vec3 limit_max;
	bool limit;

	//loop erased rw
	bool looperased;
	int num_loop;
	int size_loop;
	int biggest_loop;
	bool loop_exist;
	std::vector<glm::vec3> loop;
	

};


void NormalSimulation(int steps, RandomWalk rw, float& distance);
void LoopErasedSimulation(int steps, RandomWalk rw, float& distance,float& largetst_loop, float& erased_loop);
void ProbabilityToReturn(RandomWalk rw, float& prob, int steps);


#endif
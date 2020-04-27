
/* Start Header -------------------------------------------------------
File Name: Graph.cpp
Purpose: Random Walk Simulation, Drawing axes and graphs.
Language: C++
Platform: MSVC2019 window
Project: Random Walk Simulation
Author: Nahye Park
Creation date: 03/26/2020
End Header --------------------------------------------------------*/

#define _CRT_SECURE_NO_DEPRECATE
#include <vector>
#include <stdio.h>

#include <string>
#include <cstring>
#include <stdlib.h> 
#include <time.h>   


// Include GLEW
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "Graph.hpp"
#include "shader.hpp"

#define PI 3.141592654

Line::Line()
{
	m_size = 1.f;
	m_direction = NONE;
	m_Model = glm::mat4(1.0);
	m_line.push_back(glm::vec3(0, 0, 0));
	m_line.push_back(glm::vec3(0, 0, 0));
	color = glm::vec3(0.2f, 0.3f, 0.3f);

	glGenVertexArrays(1, &m_VertexArray);
	glGenBuffers(1, &m_vertexbuffer);

	glBindVertexArray(m_VertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, m_line.size() * sizeof(glm::vec3), &m_line[0], GL_STATIC_DRAW);

	//attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindVertexArray(0);
}

Line::Line(GLfloat _size, LineType _direction, glm::vec3 _color)
{
	m_size = _size;
	m_direction = _direction;
	color = _color;
	m_Model = glm::mat4(1.0);

	switch (_direction)
	{
	case X:
		m_line.push_back(glm::vec3(-_size, 0, 0));
		m_line.push_back(glm::vec3(_size, 0, 0));
		break;
	case Y:
		m_line.push_back(glm::vec3(0, -_size, 0));
		m_line.push_back(glm::vec3(0, _size, 0));
		break;
	case Z:
		m_line.push_back(glm::vec3(0, 0, -_size));
		m_line.push_back(glm::vec3(0, 0, _size));
		break;
	}

	glGenVertexArrays(1, &m_VertexArray);
	glGenBuffers(1, &m_vertexbuffer);

	glBindVertexArray(m_VertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, m_line.size() * sizeof(glm::vec3), &m_line[0], GL_STATIC_DRAW);

	//attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindVertexArray(0);


}

void Line::Draw(GLuint shader, glm::vec3 ViewPos)
{

	glUseProgram(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &m_Model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &m_View[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &m_Projection[0][0]);
	glUniform3fv(glGetUniformLocation(shader, "viewPos"), 1, &ViewPos[0]);
	glUniform3fv(glGetUniformLocation(shader, "color"), 1, &color[0]);


	glBindVertexArray(m_VertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, m_line.size() * sizeof(glm::vec3), &m_line[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


	glDrawArrays(GL_LINES, 0, m_line.size());
	glBindVertexArray(0);

	glBindVertexArray(0);
	glUseProgram(0);

}

void Line::Clear()
{
	glDeleteBuffers(1, &m_vertexbuffer);
	glDeleteVertexArrays(1, &m_VertexArray);
}

void RandomWalk::Walk()
{
	glm::vec3 laststep = points.back();
	bool done = false;

	while (!done)
	{
		int newdirection = rand() % 6;
		switch (newdirection)
		{
		case Direction::X_UP:
			++laststep.x;
			break;
		case Direction::Y_UP:
			++laststep.y;
			break;
		case Direction::Z_UP:
			++laststep.z;
			break;
		case Direction::X_DOWN:
			--laststep.x;
			break;
		case Direction::Y_DOWN:
			--laststep.y;
			break;
		case Direction::Z_DOWN:
			--laststep.z;
			break;
		}

		if (!limit)
			done = true;
		else
		{
			if (laststep.x > limit_max.x || laststep.y > limit_max.y || laststep.z > limit_max.z
				|| laststep.x < limit_min.x || laststep.y < limit_min.y || laststep.z < limit_min.z)
			{
				laststep = points.back();
			}
			else
				done = true;

		}

	}

	++steps;
	points.push_back(laststep);

}

void RandomWalk::RemoveLast()
{
	if (steps == 0)
		return;

	points.pop_back();
	--steps;

}

float RandomWalk::Distance()
{
	return glm::distance(startPosition, points.back());
}

void RandomWalk::Reset()
{
	points.clear();
	points.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	points.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	startPosition = points.front();
	steps = 0;
	num_loop = 0;
	size_loop = 0;
	biggest_loop = 0;
	loop_exist = false;


}

void RandomWalk::CheckLoop()
{
	std::vector<glm::vec3>::iterator samepoint = std::find(points.begin()+1, points.end(), points.back());
	if (samepoint == --points.end())
	{
		loop_exist = false;
		loop.clear();
	}
	else
	{
		loop_exist = true;
		std::copy(samepoint, points.end(), std::back_inserter(loop));
		points.erase(samepoint, --points.end());

		size_loop = (int)loop.size()-1;
		if (biggest_loop < size_loop)
			biggest_loop = size_loop;
		
	}

}

Sphere::Sphere()
{
	scale = glm::vec3(1.f, 1.f, 1.f);
	center = glm::vec3(0.f, 0.f, 0.f);
	const unsigned int X_SEGMENTS = 8;
	const unsigned int Y_SEGMENTS = 8;

	for (unsigned int y = 0; y <= static_cast<int>(Y_SEGMENTS); ++y)
	{
		for (unsigned int x = 0; x <= static_cast<int>(X_SEGMENTS); ++x)
		{
			float xSegment = (float)x / (float)X_SEGMENTS;
			float ySegment = (float)y / (float)Y_SEGMENTS;
			float xPos = static_cast<float>(std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI));
			float yPos = static_cast<float>(std::cos(ySegment * PI));
			float zPos = static_cast<float>(std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI));

			m_vertices.push_back(glm::vec3(xPos, yPos, zPos));

		}
	}

	int k1, k2 = 0;
	for (int i = 0; i < static_cast<int>(X_SEGMENTS); ++i)
	{
		k1 = i * (Y_SEGMENTS + 1);     // beginning of current stack
		k2 = k1 + Y_SEGMENTS + 1;      // beginning of next stack

		for (int j = 0; j < static_cast<int>(Y_SEGMENTS); ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				m_indices.push_back(k1);
				m_indices.push_back(k2);
				m_indices.push_back(k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (X_SEGMENTS - 1))
			{
				m_indices.push_back(k1 + 1);
				m_indices.push_back(k2);
				m_indices.push_back(k2 + 1);
			}
		}
	}

	color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_Model = glm::mat4();

	glGenVertexArrays(1, &m_VertexArray);
	glGenBuffers(1, &m_vertexbuffer);
	glGenBuffers(1, &m_elementbuffer);

	glBindVertexArray(m_VertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), &m_vertices[0], GL_STATIC_DRAW);

	//attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);
	m_elementSize = (unsigned)m_indices.size();

	glBindVertexArray(0);
}

void Sphere::Draw(GLuint shader, glm::vec3 ViewPos)
{
	glm::mat4 identity_translate(1.0);
	glm::mat4 identity_scale(1.0);

	m_Model = glm::translate(identity_translate, center) * glm::scale(identity_scale, scale);

	glUseProgram(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &m_Model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &m_View[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &m_Projection[0][0]);
	glUniform3fv(glGetUniformLocation(shader, "viewPos"), 1, &ViewPos[0]);
	glUniform3fv(glGetUniformLocation(shader, "color"), 1, &color[0]);

	glBindVertexArray(m_VertexArray);
	glDrawElements(GL_TRIANGLES, m_elementSize, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
}

void NormalSimulation(int steps, RandomWalk rw, float& distance)
{
	float sum = 0;
	RandomWalk copy = rw;
	for (int i = 0; i < TRIALS; ++i)
	{
		copy.Reset();
		for (int j = 0; j < steps; ++j)
		{
			copy.Walk();

		}

		sum += copy.Distance();

	}

	distance = sum / (float)TRIALS;
}

void LoopErasedSimulation(int steps, RandomWalk rw, float& distance, float& largest_loop, float& erased_loop)
{
	int largetest_loop_sum = 0;
	int erased_loop_sum = 0;
	float sum = 0;
	RandomWalk copy = rw;
	for (int i = 0; i < TRIALS; ++i)
	{
		copy.Reset();
		for (int j = 0; j < steps; ++j)
		{
			copy.Walk();
			copy.CheckLoop();
			if (copy.loop_exist)
				++copy.num_loop;

		}

		sum += copy.Distance();
		largetest_loop_sum += copy.biggest_loop;
		erased_loop_sum += copy.num_loop;
	}

	distance = sum / (float)TRIALS;
	largest_loop = (float)largetest_loop_sum / (float)TRIALS;
	erased_loop = (float)erased_loop_sum / (float)TRIALS;

}

void ProbabilityToReturn(RandomWalk rw, float& prob, int steps)
{
	int num_return = 0;

	RandomWalk copy = rw;
	for (int i = 0; i < TRIALS; ++i)
	{
		copy.Reset();
		for (int j = 0; j < steps; ++j)
		{
			copy.Walk();
			if (copy.points.back() == glm::vec3(0, 0, 0))
			{
				++num_return;
				break;
			}

		}
	}

	prob = (float)num_return / (float) TRIALS;


}

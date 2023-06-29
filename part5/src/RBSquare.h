#pragma once

#include "platform_includes.h"

class Shader;

class Square {
public:
	Square(int steps);
	~Square();

public:
	void SetPosition(float x, float y, float z) { _position = glm::vec3(x, y, z); }
	glm::vec3 GetPosition() { return _position; }
	void SetRotation(float x, float y, float z) { _rotation = glm::vec3(x, y, z); }
	glm::vec3 GetRotation() { return _rotation; }
	void SetScale(float x, float y, float z) { _scale = glm::vec3(x, y, z); }
	glm::vec3 GetScale() { return _scale; }
	void SetColor(glm::vec3 color) { _color = color; }
	glm::vec3 GetColor() { return _color; }

	void ApplyShader(Shader& shader);
	void Render(glm::mat4 projectionMatrix);

	void Dump();
private:
	GLuint _progId;
	GLuint _vaoId;
	GLuint _projId;
	GLuint _colorId;
	glm::vec3 _color;
	int _steps = 0;
	int _verts = 0;

	glm::vec3 _position;
	glm::vec3 _rotation;
	glm::vec3 _scale;

	// Part 3: Added to support laserpointer
	glm::mat4 _transform;

	int _indices_n;
};

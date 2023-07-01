#include "RBTexture.h"
#include "RBShader.h"

#include <iostream>

#define GLM_FORCE_RADIANS
#include "../glm/gtc/matrix_transform.hpp"

const char APP_TAG[] = "C-ENGINE-CYL";
#include <ml_logging.h>

#define BOTTOM_RADIUS 0.5
#define TOP_RADIUS 0.5

// static void MakeTexture(GLfloat *vertices, int numSteps) {
// 	float step = 2 * M_PI / static_cast<float>(numSteps);
// 	float a = 0;
// 	int idx = 0;

// 	for (int i = 0; i < numSteps + 1; i++) {
// 		float x = cosf(a);
// 		float z = sinf(a);

// 		vertices[idx++] = x * BOTTOM_RADIUS;
// 		vertices[idx++] = 0;
// 		vertices[idx++] = z * BOTTOM_RADIUS;
// 		vertices[idx++] = x * TOP_RADIUS;
// 		vertices[idx++] = 1.0;
// 		vertices[idx++] = z * TOP_RADIUS;

// 		a += step;
// 	}
// }

Texture::Texture() {
	// _steps = steps;
	// _verts = (_steps + 1) * 2; 

	_position = glm::vec3(0);
	_rotation = glm::vec3(0);
	_scale = glm::vec3(0.25);
}

Texture::~Texture() {
	glDeleteVertexArrays(1, &_vaoId);
}

void Texture::ApplyShader(Shader& shader) {
	_progId = shader.GetProgramID();
	glUseProgram(_progId);

	_colorId = glGetUniformLocation(_progId, "color");
	_projId = glGetUniformLocation(_progId, "projFrom3D");
	GLuint location = glGetAttribLocation(_progId, "coord3D");

  float a = 1.0f;
  float multiplyer = 1.0f;
  a *= multiplyer;

  float vertices[] = {
    // positions          // texture coords
     a,  a, 0.0f,   1.0f, 1.0f, // top right
     a, -a, 0.0f,   1.0f, 0.0f, // bottom right
    -a, -a, 0.0f,   0.0f, 0.0f, // bottom left
    -a,  a, 0.0f,   0.0f, 1.0f  // top left 
  };

  unsigned int indices[] = {  
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
  };
  
  // 1. bind Vertex Array Object
  glGenVertexArrays(1, &_vaoId);
	glBindVertexArray(_vaoId);

	unsigned int VBO, EBO;
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // texture coord attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // load and create a texture 
  // -------------------------
  
  glGenTextures(1, &_texture);
  glBindTexture(GL_TEXTURE_2D, _texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
  // glBindBuffer(GL_ARRAY_BUFFER, 0); 

	// glBindVertexArray(0);
	// glUseProgram(0);

	// ML_LOG_TAG(Debug, APP_TAG, "Uniform location (%d, %d, %d), program %d", _colorId, _projId, location, _progId);
	glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);



  // load image, create texture and generate mipmaps
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis
  unsigned char *data = stbi_load("texture/container.jpg", &width, &height, &nrChannels, 0);

  if (data)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
    ML_LOG(Fatal, "Failed to load texture");
  }
  stbi_image_free(data);
  
}

void Texture::Render(glm::mat4 projectionMatrix) {
	glUseProgram(_progId);

	glm::mat4 translate = glm::translate(glm::mat4(1.0f), _position);
	glm::mat4 scale = glm::scale(_scale);
	glm::mat4 transform = projectionMatrix * translate * scale;

  glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
  glBindTexture(GL_TEXTURE_2D, _texture);

  glBindVertexArray(_vaoId);
  glUniformMatrix4fv(_projId, 1, GL_FALSE, &transform[0][0]);
	// glUniform3f(_colorId, _color[0], _color[1], _color[2]);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

	glUseProgram(0);
}

void Texture::Dump() {
	ML_LOG_TAG(Debug, APP_TAG, "---------------- Dump of Texture ----------------");
	ML_LOG_TAG(Debug, APP_TAG, "Prog ID:  %d", _progId);
	ML_LOG_TAG(Debug, APP_TAG, "VAO ID:   %d", _vaoId);
	ML_LOG_TAG(Debug, APP_TAG, "Proj ID:  %d", _projId);
	ML_LOG_TAG(Debug, APP_TAG, "Color ID: %d", _colorId);
	ML_LOG_TAG(Debug, APP_TAG, "Steps:    %d",    _steps);
	ML_LOG_TAG(Debug, APP_TAG, "Verts:    %d",    _verts);
	ML_LOG_TAG(Debug, APP_TAG, "Color:    %f,%f,%f", _color[0], _color[1], _color[2]);
	ML_LOG_TAG(Debug, APP_TAG, "Position: %f,%f,%f", _position.x, _position.y, _position.z);
	ML_LOG_TAG(Debug, APP_TAG, "Rotation: %f,%f,%f", _rotation.x, _rotation.y, _rotation.z);
	ML_LOG_TAG(Debug, APP_TAG, "Scale:    %f,%f,%f", _scale.x, _scale.y, _scale.z);
	ML_LOG_TAG(Debug, APP_TAG, "--------------------------------------------------");
}
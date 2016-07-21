#include "VertexArray.h"

namespace GraphicsLibrary
{
	VertexArray::VertexArray(vector<vec3> vertices, vector<vec3> normals)
	{
		glGenVertexArrays(1, &this->id);
		glBindVertexArray(this->id);

		this->vertices = new Buffer();
		this->vertices->Bind();
		this->vertices->LoadData(vertices);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), static_cast<GLvoid*>(0));
		glEnableVertexAttribArray(0);

		this->normals = new Buffer();
		this->normals->Bind();
		this->normals->LoadData(normals);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), static_cast<GLvoid*>(0));
		glEnableVertexAttribArray(1);

		Buffer::Unbind();
		glBindVertexArray(0);
	}
	
	VertexArray::~VertexArray()
	{
		delete this->vertices;
		delete this->normals;
	}

	void VertexArray::Draw(ShaderProgram* shaderProgram)
	{
		shaderProgram->Use();

		glBindVertexArray(this->id);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		ShaderProgram::Disable();
	}
}
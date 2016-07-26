#include "VertexArray.h"

namespace GraphicsLibrary
{
	VertexArray::VertexArray(vector<vec3> vertices, vector<vec3> normals)
	{
		glGenVertexArrays(1, &this->id);
		glBindVertexArray(this->id);

		this->vertices = new VertexBuffer();
		this->vertices->Bind();
		this->vertices->LoadData(vertices);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), static_cast<GLvoid*>(0));
		glEnableVertexAttribArray(0);

		this->normals = new VertexBuffer();
		this->normals->Bind();
		this->normals->LoadData(normals);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), static_cast<GLvoid*>(0));
		glEnableVertexAttribArray(1);

		VertexBuffer::Unbind();
		glBindVertexArray(0);

		this->vertexCount = static_cast<int>(vertices.size());

		this->debugVertices = vertices;
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
		glDrawArrays(GL_TRIANGLES, 0, this->vertexCount);
		glBindVertexArray(0);

		ShaderProgram::Disable();
	}
}
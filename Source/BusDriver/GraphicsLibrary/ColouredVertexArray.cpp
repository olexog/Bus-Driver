#include "ColouredVertexArray.h"

namespace GraphicsLibrary
{
	ColouredVertexArray::ColouredVertexArray(vector<vec3> vertices, vector<vec3> normals, vector<vec3> ambientColours, vector<vec3> diffuseColours, vector<vec3> specularColours)
		: VertexArray(vertices, normals)
	{
		glBindVertexArray(this->id);

		this->ambientColours = new VertexBuffer();
		this->ambientColours->Bind();
		this->ambientColours->LoadData(ambientColours);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), static_cast<GLvoid*>(0));
		glEnableVertexAttribArray(2);

		this->diffuseColours = new VertexBuffer();
		this->diffuseColours->Bind();
		this->diffuseColours->LoadData(diffuseColours);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), static_cast<GLvoid*>(0));
		glEnableVertexAttribArray(3);

		this->specularColours = new VertexBuffer();
		this->specularColours->Bind();
		this->specularColours->LoadData(specularColours);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), static_cast<GLvoid*>(0));
		glEnableVertexAttribArray(4);

		VertexBuffer::Unbind();
		glBindVertexArray(0);
	}

	ColouredVertexArray::~ColouredVertexArray()
	{
		delete this->ambientColours;
		delete this->diffuseColours;
		delete this->specularColours;
	}

	void ColouredVertexArray::Draw(ShaderProgram* shaderProgram)
	{
		shaderProgram->Use();

		glBindVertexArray(this->id);
		glDrawArrays(GL_TRIANGLES, 0, this->vertexCount);
		glBindVertexArray(0);

		ShaderProgram::Disable();
	}
}
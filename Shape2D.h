#pragma once

#include "../mgl/mgl.hpp"
#include <memory>
#include <stdexcept>

constexpr auto TRIANGLE = 0;
constexpr auto SQUARE = 1;
constexpr auto PARALLELOGRAM = 2;

typedef struct {
	GLfloat XYZW[4];
	GLfloat RGBA[4];
} Vertex;

class Shape2D {
	private:
		Vertex* vertices;
		int vertex_count;
		GLubyte* indices;
		int index_count;
		GLuint vao, vbo[2];
		int shapeType;

        const GLuint POSITION = 0, COLOR = 1;

		void createShapeBuffers();
		void triangle();
		void square();
		void parallelogram();

	public:
		Shape2D(int shape);
        void draw();
		void destroy();
		float getSideLength();
		GLuint get_vao() const { return vao; }
};

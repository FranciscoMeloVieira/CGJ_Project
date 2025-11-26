#include "Shape2D.h"


void Shape2D::createShapeBuffers() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    {
        glGenBuffers(2, vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        {
            glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(Vertex), vertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(POSITION);
            glVertexAttribPointer(POSITION, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                reinterpret_cast<GLvoid*>(0));
            glEnableVertexAttribArray(COLOR);
            glVertexAttribPointer(
                COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                reinterpret_cast<GLvoid*>(sizeof(vertices[0].XYZW)));
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                GL_STATIC_DRAW);
        }
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(2, vbo);
}


void Shape2D::triangle() {
    vertices = new Vertex[3]{
        {{-0.333333f, -0.333333f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
        {{ 0.666666f, -0.333333f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
        {{-0.333333f,  0.666666f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}}
    };
    vertex_count = 3;
    indices = new GLubyte[3]{ 0, 1, 2 };
    index_count = 3;
    createShapeBuffers();
}

void Shape2D::square() {
    vertices = new Vertex[4]{
        {{-0.5f, -0.5f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
        {{ 0.5f, -0.5f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
        {{ 0.5f,  0.5f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
        {{-0.5f,  0.5f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}}
    };
    vertex_count = 4;
    indices = new GLubyte[6]{ 0, 1, 2, 0, 2, 3 };
    index_count = 6;
    createShapeBuffers();
}

void Shape2D::parallelogram() {
    vertices = new Vertex[4]{
    {{-0.353553f, -0.353553f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
    {{ 1.060660f, -0.353553f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
    {{ 0.353553f,  0.353553f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
    {{-1.060660f,  0.353553f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f} }
    };
    vertex_count = 4;
    indices = new GLubyte[6]{ 0, 1, 2, 0, 2, 3 };
    index_count = 6;
    createShapeBuffers();
}

void Shape2D::draw() {
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_BYTE,
        reinterpret_cast<GLvoid*>(0));
}

Shape2D::Shape2D(int shape) {

    if (shape == TRIANGLE) {
        triangle();
    }
    else if (shape == SQUARE) {
        square();
    }
    else if (shape == PARALLELOGRAM) {
        parallelogram();
    }
    else {
        throw std::invalid_argument("Invalid shape type");
    }
}

void Shape2D::destroy() {
    glBindVertexArray(vao);
    glDisableVertexAttribArray(POSITION);
    glDisableVertexAttribArray(COLOR);

    glDeleteBuffers(2, vbo);
    glDeleteVertexArrays(1, &vao);
}

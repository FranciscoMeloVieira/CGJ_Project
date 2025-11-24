////////////////////////////////////////////////////////////////////////////////
//
// Basic Triangle rendering
//
// This program demonstrates fundamental Modern OpenGL concepts by rendering
// two triangle instances directly in Clip Space. It serves as an introductory
// example for understanding the OpenGL graphics pipeline and basic shader
// programming.
//
// Key Concepts Demonstrated:
// - Vertex Array Objects (VAOs) and Vertex Buffer Objects (VBOs)
// - Shader program creation and management
// - Attribute and uniform handling
// - Basic transformation matrices
// - Clip space rendering without model/view/projection matrices
//
// Copyright (c) 2013-25 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#include <memory>
#include <vector>

#include "../mgl/mgl.hpp"


typedef struct {
    GLfloat XYZW[4];
    GLfloat RGBA[4];
} Vertex;

typedef struct {
    Vertex* vertices;
	int vertex_count;
    GLubyte* indices;
	int index_count;
    GLuint vao, vbo[2];
} Shape;


////////////////////////////////////////////////////////////////////////// MYAPP

class MyApp : public mgl::App {
public:
  MyApp() = default;
  ~MyApp() override = default;

  void initCallback(GLFWwindow *win) override;
  void displayCallback(GLFWwindow *win, double elapsed) override;
  void windowCloseCallback(GLFWwindow *win) override;
  void windowSizeCallback(GLFWwindow *win, int width, int height) override;

private:
  const GLuint POSITION = 0, COLOR = 1;
  GLuint VaoId, VboId[2];
  std::unique_ptr<mgl::ShaderProgram> Shaders = nullptr;
  GLint MatrixId;
  std::vector<Shape> shapes;

  void createShaderProgram();
  void createBufferObjects();
  void createShapeBuffers(Shape& shape);
  void destroyBufferObjects();
  void drawScene();
  void drawShape(const Shape& shape, const glm::mat4& transform);
};

//////////////////////////////////////////////////////////////////////// SHADERs

void MyApp::createShaderProgram() {
  Shaders = std::make_unique<mgl::ShaderProgram>();
  Shaders->addShader(GL_VERTEX_SHADER, "clip-vs.glsl");
  Shaders->addShader(GL_FRAGMENT_SHADER, "clip-fs.glsl");

  Shaders->addAttribute(mgl::POSITION_ATTRIBUTE, POSITION);
  Shaders->addAttribute(mgl::COLOR_ATTRIBUTE, COLOR);
  Shaders->addUniform("Matrix");

  Shaders->create();

  MatrixId = Shaders->Uniforms["Matrix"].index;
}

//////////////////////////////////////////////////////////////////// VAOs & VBOs


const Vertex Triangle_Vertices[] = {
    {{-0.333333f, -0.333333f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
    {{ 0.666667f, -0.333333f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
    {{-0.333333f,  0.666667f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}}};

const GLubyte Triangle_Indices[] = { 0, 1, 2 };

const Vertex Square_Vertices[] = {
    {{-0.5f, -0.5f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
    {{ 0.5f, -0.5f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
    {{ 0.5f,  0.5f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
	{{-0.5f,  0.5f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f} }
};

const GLubyte Square_Indices[] = { 0, 1, 2, 0, 2, 3};

const Vertex Parallelogram_Vertices[] = {
    {{-0.75f, -0.5f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
    {{ 0.25f, -0.5f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
    {{ 0.75f,  0.5f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
    {{-0.25f,  0.5f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f} }
};

const GLubyte Parallelogram_Indices[] = { 0, 1, 2, 0, 2, 3 };

void MyApp::createBufferObjects() {

	// Triangle
	auto triangle = std::make_unique<Shape>();
	triangle->vertices = const_cast<Vertex*>(Triangle_Vertices);
    triangle->vertex_count = 3;
    triangle->indices = const_cast<GLubyte*>(Triangle_Indices);
    triangle->index_count = 3;
    createShapeBuffers(*triangle);
	shapes.push_back(*triangle);

	// Square
	auto square = std::make_unique<Shape>();
    square->vertices = const_cast<Vertex*>(Square_Vertices);
    square->vertex_count = 4;
    square->indices = const_cast<GLubyte*>(Square_Indices);
    square->index_count = 6;
	createShapeBuffers(*square);
	shapes.push_back(*square);

	// Parallelogram
    auto parallelogram = std::make_unique<Shape>();
    parallelogram->vertices = const_cast<Vertex*>(Parallelogram_Vertices);
    parallelogram->vertex_count = 4;
    parallelogram->indices = const_cast<GLubyte*>(Parallelogram_Indices);
	parallelogram->index_count = 6;
    createShapeBuffers(*parallelogram);
	shapes.push_back(*parallelogram);
}

void MyApp::createShapeBuffers(Shape& shape) {
    glGenVertexArrays(1, &shape.vao);
    glBindVertexArray(shape.vao);
    {
        glGenBuffers(2, shape.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, shape.vbo[0]);
        {
            glBufferData(GL_ARRAY_BUFFER, shape.vertex_count * sizeof(Vertex), shape.vertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(POSITION);
            glVertexAttribPointer(POSITION, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                reinterpret_cast<GLvoid*>(0));
            glEnableVertexAttribArray(COLOR);
            glVertexAttribPointer(
                COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                reinterpret_cast<GLvoid*>(sizeof(shape.vertices[0].XYZW)));
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.vbo[1]);
        {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(shape.indices), shape.indices,
                GL_STATIC_DRAW);
        }
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(2, shape.vbo);
}

void MyApp::destroyBufferObjects() {
    for (auto& shape : shapes) {
        glBindVertexArray(shape.vao);
        glDisableVertexAttribArray(POSITION);
        glDisableVertexAttribArray(COLOR);

        glDeleteBuffers(2, shape.vbo);
        glDeleteVertexArrays(1, &shape.vao);
    }
    glBindVertexArray(0);
}


////////////////////////////////////////////////////////////////////////// SCENE

// Transformation Matrices
const glm::mat4 I(1.0f);


void MyApp::drawScene() {
  // Drawing directly in clip space

    drawShape(shapes[2], I);
}

void MyApp::drawShape(const Shape& shape, const glm::mat4& transform) {
    glBindVertexArray(shape.vao);
    Shaders->bind();

    glUniformMatrix4fv(MatrixId, 1, GL_FALSE, glm::value_ptr(transform));
    glDrawElements(GL_TRIANGLES, shape.index_count, GL_UNSIGNED_BYTE,
        reinterpret_cast<GLvoid*>(0));

    Shaders->unbind();
    glBindVertexArray(0);
}

////////////////////////////////////////////////////////////////////// CALLBACKS

void MyApp::initCallback(GLFWwindow *win) {
  createBufferObjects();
  createShaderProgram();
}

void MyApp::windowCloseCallback(GLFWwindow *win) { destroyBufferObjects(); }

void MyApp::windowSizeCallback(GLFWwindow *win, int winx, int winy) {
  glViewport(0, 0, winx, winy);
}

void MyApp::displayCallback(GLFWwindow *win, double elapsed) { drawScene(); }

/////////////////////////////////////////////////////////////////////////// MAIN

int main(int argc, char *argv[]) {
  mgl::Engine &engine = mgl::Engine::getInstance();
  engine.setApp(new MyApp());
  engine.setOpenGL(4, 6);
  engine.setWindow(1000, 1000, "Hello Modern 2D World", 0, 1);
  engine.init();
  engine.run();
  exit(EXIT_SUCCESS);
}

//////////////////////////////////////////////////////////////////////////// END

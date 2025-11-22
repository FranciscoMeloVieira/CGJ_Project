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


const Vertex Vertices[] = {
    {{0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
    {{1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
    {{0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}}};

const Vertex Vertices2[] = {
    {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
    {{1.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
    {{0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}} };

const GLubyte Indices[] = {0, 1, 2};

void MyApp::createBufferObjects() {
	auto shape = std::make_unique<Shape>();
	shape->vertices = const_cast<Vertex*>(Vertices);
	shape->vertex_count = 3;
    shape->indices = const_cast<GLubyte*>(Indices);
	shape->index_count = 3;
    createShapeBuffers(*shape);
	shapes.push_back(*shape);

	auto shape2 = std::make_unique<Shape>();
	shape2->vertices = const_cast<Vertex*>(Vertices2);
    shape2->vertex_count = 3;
	shape2->indices = const_cast<GLubyte*>(Indices);
    shape2->index_count = 3;
	createShapeBuffers(*shape2);
	shapes.push_back(*shape2);
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

        glDeleteBuffers(2, shape.vbo);      // delete VBOs
        glDeleteVertexArrays(1, &shape.vao); // delete VAO
    }
    glBindVertexArray(0);
}


////////////////////////////////////////////////////////////////////////// SCENE

const glm::mat4 I(1.0f);
const glm::mat4 M =
    glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -1.0f, 0.0f));

void MyApp::drawScene() {
  // Drawing directly in clip space

	drawShape(shapes[0], M);
	drawShape(shapes[1], I);
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

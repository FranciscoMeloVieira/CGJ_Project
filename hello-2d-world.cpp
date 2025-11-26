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
#include "Shape2D.h"



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
  GLint ColorId; // Added: location/index for the Color uniform
  std::vector<Shape2D> shapes;

  void createShaderProgram();
  void createBufferObjects();
  void destroyBufferObjects();
  void drawScene();
};

//////////////////////////////////////////////////////////////////////// SHADERs

void MyApp::createShaderProgram() {
  Shaders = std::make_unique<mgl::ShaderProgram>();
  Shaders->addShader(GL_VERTEX_SHADER, "clip-vs.glsl");
  Shaders->addShader(GL_FRAGMENT_SHADER, "clip-fs.glsl");

  Shaders->addAttribute(mgl::POSITION_ATTRIBUTE, POSITION);
  Shaders->addAttribute(mgl::COLOR_ATTRIBUTE, COLOR);

  // Register both uniforms used by the application
  Shaders->addUniform("Matrix");
  Shaders->addUniform("Color");

  Shaders->create();

  // Retrieve uniform locations/indices after program creation
  MatrixId = Shaders->Uniforms["Matrix"].index;
  ColorId = Shaders->Uniforms["Color"].index; 
}

//////////////////////////////////////////////////////////////////// VAOs & VBOs


const Vertex Triangle_Vertices[] = {
    {{-0.333333f, -0.333333f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
    {{ 0.666666f, -0.333333f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
    {{-0.333333f,  0.666666f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}}};

const GLubyte Triangle_Indices[] = { 0, 1, 2 };

const Vertex Square_Vertices[] = {
    {{-0.5f, -0.5f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
    {{ 0.5f, -0.5f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
    {{ 0.5f,  0.5f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
	{{-0.5f,  0.5f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f} }
};

const GLubyte Square_Indices[] = { 0, 1, 2, 0, 2, 3};

const Vertex Parallelogram_Vertices[] = {
    {{-0.353553f, -0.353553f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
    {{ 1.060660f, -0.353553f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
    {{ 0.353553f,  0.353553f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
    {{-1.060660f,  0.353553f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f} }
};

const GLubyte Parallelogram_Indices[] = { 0, 1, 2, 0, 2, 3 };

void MyApp::createBufferObjects() {
	Shape2D triangle_shape(TRIANGLE);
	shapes.push_back(std::move(triangle_shape));

	Shape2D square_shape(SQUARE);
	shapes.push_back(std::move(square_shape));

	Shape2D parallelogram_shape(PARALLELOGRAM);
	shapes.push_back(std::move(parallelogram_shape));
}


void MyApp::destroyBufferObjects() {
    for (auto& shape : shapes) {
		shape.destroy();
    }
    glBindVertexArray(0);
}


////////////////////////////////////////////////////////////////////////// SCENE

// Calculations
// Center of the Tangram is the square center. Moving the square moves the entire Tangram.
const float global_scale = 0.5f;


// Square calculations
// Variables
const float square_side = 0.5f * global_scale * glm::length(glm::vec2(Square_Vertices[1].XYZW[0] - Square_Vertices[0].XYZW[0], Square_Vertices[1].XYZW[1] - Square_Vertices[0].XYZW[1]));

const float square_diagonal = glm::sqrt(2 * glm::pow(square_side, 2));

// Positioning
const float square_x_offset = 0.0f;

const float square_y_offset = 0.0f;


// First triangle calculations
// Variables
const float triangle_side = global_scale * glm::length(
    glm::vec2(Triangle_Vertices[1].XYZW[0] - Triangle_Vertices[0].XYZW[0],
	Triangle_Vertices[1].XYZW[1] - Triangle_Vertices[0].XYZW[1]));

const float triangle_hypotenuse = glm::sqrt(glm::pow(triangle_side, 2) * 2);

const float triangle_height = glm::sqrt(glm::pow(triangle_side, 2) - glm::pow(triangle_hypotenuse / 2, 2));

const float centroid = (triangle_side / 3);  // Triangle is isosceles so the x and y centroids are equal

const float triangle_centroid_diagonal = glm::sqrt(2 * glm::pow(centroid, 2));

const float triangle_top_vertex_position = Triangle_Vertices[2].XYZW[1] * global_scale - 0.5f;

// Positioning
const float first_triangle_x_offset = -(square_diagonal / 2 - centroid) + square_x_offset;

const float first_triangle_y_offset = -(triangle_side - centroid) + square_y_offset;


// Second triangle calculations
// Variables
const float second_triangle_side = 0.5f * triangle_side;

const float second_triangle_hypotenuse = glm::sqrt(glm::pow(second_triangle_side, 2) * 2);

const float second_triangle_centroid = (second_triangle_side / 3);

const float second_triangle_height = glm::sqrt(glm::pow(second_triangle_side, 2) - glm::pow(second_triangle_hypotenuse / 2, 2));

const float second_triangle_centroid_diagonal = glm::sqrt(2 * glm::pow(second_triangle_centroid, 2));

// Positioning
const float second_triangle_x_offset = -(centroid - (second_triangle_height - second_triangle_centroid_diagonal)) + first_triangle_x_offset;

const float second_triangle_y_offset = (triangle_side + (second_triangle_hypotenuse / 2 - centroid)) + first_triangle_y_offset;


// Third triangle calculations
// Variables
const float third_triangle_hypotenuse = triangle_hypotenuse;

const float third_triangle_centroid_diagonal = triangle_centroid_diagonal;

const float third_triangle_height = triangle_height;

// Positioning
const float third_triangle_x_offset = third_triangle_hypotenuse / 2 + square_x_offset;

const float third_triangle_y_offset = -(square_diagonal / 2 - (third_triangle_height - third_triangle_centroid_diagonal)) + square_y_offset;


// Fourth triangle calculations
// Variables
const float fourth_triangle_side = triangle_side / glm::sqrt(2);

const float fourth_triangle_centroid = (fourth_triangle_side / 3);

// Positioning
const float fourth_triangle_x_offset = -fourth_triangle_centroid + square_x_offset;

const float fourth_triangle_y_offset = (fourth_triangle_side - fourth_triangle_centroid) + (square_diagonal / 2) + square_y_offset;


// Fifth triangle calculations
// Variables
const float fifth_triangle_side = second_triangle_side;

const float fifth_triangle_hypotenuse = second_triangle_hypotenuse;

const float fifth_triangle_centroid_diagonal = second_triangle_centroid_diagonal;

const float fifth_triangle_height = second_triangle_height;

// Positioning
const float fifth_triangle_x_offset = -(fourth_triangle_side - fourth_triangle_centroid) + fourth_triangle_x_offset;

const float fifth_triangle_y_offset = -(fifth_triangle_height - (fourth_triangle_centroid + (fifth_triangle_height - fifth_triangle_centroid_diagonal))) + fourth_triangle_y_offset;


//Parallelogram
// Variables
const float parallelogram_heigth = (Parallelogram_Vertices[2].XYZW[1] - Parallelogram_Vertices[0].XYZW[1]) * global_scale / 2;

// Positioning
const float parallelogram_x_offset = (triangle_side - centroid / 2) + first_triangle_x_offset;

const float parallelogram_y_offset = -(centroid + parallelogram_heigth / 2) + first_triangle_y_offset;



// Transformation Matrices
// 
const glm::mat4 I(1.0f);
const glm::mat4 global_rotation = glm::mat4_cast(glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, 10.0f))));

const glm::mat4 first_triangle_transform = 
    global_rotation
    * glm::mat4(glm::translate(I, glm::vec3(first_triangle_x_offset, first_triangle_y_offset, 0.0f)))
	* glm::mat4_cast(glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, 0.0f))))
	* glm::mat4(glm::scale(I, glm::vec3(global_scale, global_scale, 1.0f)));

const glm::mat4 square_transform =
    global_rotation
    * glm::mat4(glm::translate(I, glm::vec3(square_x_offset, square_y_offset, 0.0f)))
    * glm::mat4_cast(glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, 45.0f))))
	* glm::mat4(glm::scale(I, glm::vec3(global_scale / 2, global_scale / 2, 1.0f)));

const glm::mat4 second_triangle_transform =
    global_rotation
    * glm::mat4(glm::translate(I, glm::vec3(second_triangle_x_offset, second_triangle_y_offset, 0.0f)))
    * glm::mat4_cast(glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, 135.0f))))
	* glm::mat4(glm::scale(I, glm::vec3(global_scale / 2, global_scale / 2, 1.0f)));

const glm::mat4 third_triangle_transform =
    global_rotation
    * glm::mat4(glm::translate(I, glm::vec3(third_triangle_x_offset, third_triangle_y_offset, 0.0f)))
    * glm::mat4_cast(glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, -135.0f))))
	* glm::mat4(glm::scale(I, glm::vec3(global_scale, global_scale, 1.0f)));

const glm::mat4 fourth_triangle_transform =
    global_rotation
    * glm::mat4(glm::translate(I, glm::vec3(fourth_triangle_x_offset, fourth_triangle_y_offset, 0.0f)))
    * glm::mat4_cast(glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, 180.0f))))
	* glm::mat4(glm::scale(I, glm::vec3(global_scale / glm::sqrt(2), global_scale / glm::sqrt(2), 1.0f)));

const glm::mat4 fifth_triangle_transform =
    global_rotation
    * glm::mat4(glm::translate(I, glm::vec3(fifth_triangle_x_offset, fifth_triangle_y_offset, 0.0f)))
    * glm::mat4_cast(glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, -135.0f))))
	* glm::mat4(glm::scale(I, glm::vec3(global_scale / 2, global_scale / 2, 1.0f)));

const glm::mat4 parallelogram_transform =
    global_rotation
    * glm::mat4(glm::translate(I, glm::vec3(parallelogram_x_offset, parallelogram_y_offset, 0.0f)))
    * glm::mat4_cast(glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, 0.0f))))
	* glm::mat4(glm::scale(I, glm::vec3(global_scale / 2, global_scale / 2, 1.0f)));


void MyApp::drawScene() {

  Shaders->bind();

  // TRIANGLES
  glBindVertexArray(shapes[TRIANGLE].get_vao());
  glUniformMatrix4fv(MatrixId, 1, GL_FALSE, glm::value_ptr(first_triangle_transform));
  glUniform4fv(ColorId, 1, glm::value_ptr(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)));
  shapes[TRIANGLE].draw();

  glUniformMatrix4fv(MatrixId, 1, GL_FALSE, glm::value_ptr(second_triangle_transform));
  glUniform4fv(ColorId, 1, glm::value_ptr(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)));
  shapes[TRIANGLE].draw();

  glUniformMatrix4fv(MatrixId, 1, GL_FALSE, glm::value_ptr(third_triangle_transform));
  glUniform4fv(ColorId, 1, glm::value_ptr(glm::vec4(0.3f, 0.6f, 1.0f, 1.0f)));
  shapes[TRIANGLE].draw();

  glUniformMatrix4fv(MatrixId, 1, GL_FALSE, glm::value_ptr(fourth_triangle_transform));
  glUniform4fv(ColorId, 1, glm::value_ptr(glm::vec4(0.5f, 0.0f, 0.5f, 1.0f)));
  shapes[TRIANGLE].draw();

  glUniformMatrix4fv(MatrixId, 1, GL_FALSE, glm::value_ptr(fifth_triangle_transform));
  glUniform4fv(ColorId, 1, glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));
  shapes[TRIANGLE].draw();

  // SQUARE
  glBindVertexArray(shapes[SQUARE].get_vao());
  glUniformMatrix4fv(MatrixId, 1, GL_FALSE, glm::value_ptr(square_transform));
  glUniform4fv(ColorId, 1, glm::value_ptr(glm::vec4(0.0f, 0.7f, 0.0f, 1.0f)));
  shapes[SQUARE].draw();

  // PARALLELOGRAM
  glBindVertexArray(shapes[PARALLELOGRAM].get_vao());
  glUniformMatrix4fv(MatrixId, 1, GL_FALSE, glm::value_ptr(parallelogram_transform));
  glUniform4fv(ColorId, 1, glm::value_ptr(glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)));
  shapes[PARALLELOGRAM].draw();

  // Unbind
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

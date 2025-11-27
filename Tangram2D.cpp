////////////////////////////////////////////////////////////////////////////////
//
// Trangram Puzzle - 2D Modern OpenGL Application
// 
// This application renders a Tangram puzzle using OpenGL techniques.
// The Tangram is composed of seven geometric shapes: five triangles of
// different sizes, one square, and one parallelogram. Each shape is assigned
// a distinct color. The shapes are transformed and positioned to form a
// tangram in the shape of a dragon.
// 
// Computer Graphics for Games
// Group 18
// Francisco Vieira - 103360
// Diogo Pereira - 	116314
// 
//
////////////////////////////////////////////////////////////////////////////////


#include <memory>
#include <vector>
#include <unordered_map>

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
  GLint MatrixId, ColorId; 
  std::vector<Shape2D> shapes;
  

  void createShaderProgram();
  void createBufferObjects();
  void destroyBufferObjects();
  void drawScene();
  std::unordered_map<std::string, glm::mat4> calculations();
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

std::unordered_map<std::string, glm::mat4> MyApp::calculations() {
	// Global scale for pieces. (Scales above 0.5f may cause pieces to go out of view)
    const float global_scale = 0.5f;

    /* Tangram piece sizes:
		-The square side length is half the biggest triangle side length.
		-The parallelogram side length is also half the biggest triangle side length.
		-The small triangles side length is half the biggest triangle side length.
		-The medium triangle side length is the same as the biggest triangle side length divided by sqrt(2).
	   This sizes are fixed and should not be changed to maintain the Tangram proportions.
    */

    const float square_ratio = 0.5f;
    const float medium_triangle_ratio = 1 / glm::sqrt(2);
	const float small_triangle_ratio = 0.5f;
	const float parallelogram_ratio = 0.5f;

    // Square calculations
    // Variables
    const float square_side = square_ratio * global_scale * shapes[SQUARE].getSideLength();

    const float square_diagonal = glm::sqrt(2 * glm::pow(square_side, 2));

    // Positioning
    // Center of the Tangram is the square center. Moving the square moves the entire Tangram.
    const float square_x_offset = 0.0f;

    const float square_y_offset = 0.0f;


	// First triangle calculations (Magenta triangle)
    // Variables
    const float triangle_side = global_scale * shapes[TRIANGLE].getSideLength();

    const float triangle_hypotenuse = glm::sqrt(glm::pow(triangle_side, 2) * 2);

    const float triangle_height = glm::sqrt(glm::pow(triangle_side, 2) - glm::pow(triangle_hypotenuse / 2, 2));

    const float centroid = (triangle_side / 3);  // Triangle is isosceles so the x and y centroids are equal

    const float triangle_centroid_diagonal = glm::sqrt(2 * glm::pow(centroid, 2));

    // Positioning
    const float first_triangle_x_offset = -(square_diagonal / 2 - centroid) + square_x_offset;

    const float first_triangle_y_offset = -(triangle_side - centroid) + square_y_offset;


	// Second triangle calculations (Cyan triangle)
    // Variables
    const float second_triangle_side = small_triangle_ratio * triangle_side;

    const float second_triangle_hypotenuse = glm::sqrt(glm::pow(second_triangle_side, 2) * 2);

    const float second_triangle_centroid = (second_triangle_side / 3);

    const float second_triangle_height = glm::sqrt(glm::pow(second_triangle_side, 2) - glm::pow(second_triangle_hypotenuse / 2, 2));

    const float second_triangle_centroid_diagonal = glm::sqrt(2 * glm::pow(second_triangle_centroid, 2));

    // Positioning
    const float second_triangle_x_offset = -(centroid - (second_triangle_height - second_triangle_centroid_diagonal)) + first_triangle_x_offset;

    const float second_triangle_y_offset = (triangle_side + (second_triangle_hypotenuse / 2 - centroid)) + first_triangle_y_offset;


	// Third triangle calculations (Light Blue triangle)
    // Variables
    const float third_triangle_hypotenuse = triangle_hypotenuse;

    const float third_triangle_centroid_diagonal = triangle_centroid_diagonal;

    const float third_triangle_height = triangle_height;

    // Positioning
    const float third_triangle_x_offset = third_triangle_hypotenuse / 2 + square_x_offset;

    const float third_triangle_y_offset = -(square_diagonal / 2 - (third_triangle_height - third_triangle_centroid_diagonal)) + square_y_offset;


	// Fourth triangle calculations (Purple triangle)
    // Variables
    const float fourth_triangle_side = triangle_side * medium_triangle_ratio;

    const float fourth_triangle_centroid = (fourth_triangle_side / 3);

    // Positioning
    const float fourth_triangle_x_offset = -fourth_triangle_centroid + square_x_offset;

    const float fourth_triangle_y_offset = (fourth_triangle_side - fourth_triangle_centroid) + (square_diagonal / 2) + square_y_offset;


	// Fifth triangle calculations (Red triangle)
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
	const float parallelogram_side = shapes[PARALLELOGRAM].getSideLength() * global_scale * parallelogram_ratio;

    const float parallelogram_heigth = parallelogram_side * glm::sin(glm::radians(45.0f));

    // Positioning
    const float parallelogram_x_offset = (triangle_side - centroid / 2) + first_triangle_x_offset;

    const float parallelogram_y_offset = -(centroid + parallelogram_heigth / 2) + first_triangle_y_offset;


    // Transformation Matrices
	std::unordered_map<std::string, glm::mat4> matrices = {};
    
    // All transformation are done in the correct scale, rotate and translate order. An additional rotation is done afterwards because the tangram is slighty tilted to the left.
    const glm::mat4 I(1.0f);
    const glm::mat4 global_rotation = glm::mat4_cast(glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, 10.0f))));

    const glm::mat4 first_triangle_transform =
        global_rotation
        * glm::mat4(glm::translate(I, glm::vec3(first_triangle_x_offset, first_triangle_y_offset, 0.0f)))
        * glm::mat4_cast(glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, 0.0f))))
        * glm::mat4(glm::scale(I, glm::vec3(global_scale, global_scale, 1.0f)));

    matrices.insert({"first_triangle_transform", first_triangle_transform});

    const glm::mat4 square_transform =
        global_rotation
        * glm::mat4(glm::translate(I, glm::vec3(square_x_offset, square_y_offset, 0.0f)))
        * glm::mat4_cast(glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, 45.0f))))
        * glm::mat4(glm::scale(I, glm::vec3(global_scale / 2, global_scale / 2, 1.0f)));

	matrices.insert({ "square_transform", square_transform });

    const glm::mat4 second_triangle_transform =
        global_rotation
        * glm::mat4(glm::translate(I, glm::vec3(second_triangle_x_offset, second_triangle_y_offset, 0.0f)))
        * glm::mat4_cast(glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, 135.0f))))
        * glm::mat4(glm::scale(I, glm::vec3(global_scale / 2, global_scale / 2, 1.0f)));

	matrices.insert({ "second_triangle_transform", second_triangle_transform });

    const glm::mat4 third_triangle_transform =
        global_rotation
        * glm::mat4(glm::translate(I, glm::vec3(third_triangle_x_offset, third_triangle_y_offset, 0.0f)))
        * glm::mat4_cast(glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, -135.0f))))
        * glm::mat4(glm::scale(I, glm::vec3(global_scale, global_scale, 1.0f)));

	matrices.insert({ "third_triangle_transform", third_triangle_transform });

    const glm::mat4 fourth_triangle_transform =
        global_rotation
        * glm::mat4(glm::translate(I, glm::vec3(fourth_triangle_x_offset, fourth_triangle_y_offset, 0.0f)))
        * glm::mat4_cast(glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, 180.0f))))
        * glm::mat4(glm::scale(I, glm::vec3(global_scale / glm::sqrt(2), global_scale / glm::sqrt(2), 1.0f)));

	matrices.insert({ "fourth_triangle_transform", fourth_triangle_transform });

    const glm::mat4 fifth_triangle_transform =
        global_rotation
        * glm::mat4(glm::translate(I, glm::vec3(fifth_triangle_x_offset, fifth_triangle_y_offset, 0.0f)))
        * glm::mat4_cast(glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, -135.0f))))
        * glm::mat4(glm::scale(I, glm::vec3(global_scale / 2, global_scale / 2, 1.0f)));

	matrices.insert({ "fifth_triangle_transform", fifth_triangle_transform });

    const glm::mat4 parallelogram_transform =
        global_rotation
        * glm::mat4(glm::translate(I, glm::vec3(parallelogram_x_offset, parallelogram_y_offset, 0.0f)))
        * glm::mat4_cast(glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, 0.0f))))
        * glm::mat4(glm::scale(I, glm::vec3(global_scale / 2, global_scale / 2, 1.0f)));

	matrices.insert({ "parallelogram_transform", parallelogram_transform });

	return matrices;
}

void MyApp::drawScene() {

// Get transformation matrices
  const auto matrices = calculations();

  Shaders->bind();

  // TRIANGLES
  glBindVertexArray(shapes[TRIANGLE].get_vao());
  glUniformMatrix4fv(MatrixId, 1, GL_FALSE, glm::value_ptr(matrices.at("first_triangle_transform")));
  glUniform4fv(ColorId, 1, glm::value_ptr(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f))); // Magenta
  shapes[TRIANGLE].draw();

  glUniformMatrix4fv(MatrixId, 1, GL_FALSE, glm::value_ptr(matrices.at("second_triangle_transform")));
  glUniform4fv(ColorId, 1, glm::value_ptr(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f))); // Cyan
  shapes[TRIANGLE].draw();

  glUniformMatrix4fv(MatrixId, 1, GL_FALSE, glm::value_ptr(matrices.at("third_triangle_transform")));
  glUniform4fv(ColorId, 1, glm::value_ptr(glm::vec4(0.3f, 0.6f, 1.0f, 1.0f))); // Light Blue
  shapes[TRIANGLE].draw();

  glUniformMatrix4fv(MatrixId, 1, GL_FALSE, glm::value_ptr(matrices.at("fourth_triangle_transform")));
  glUniform4fv(ColorId, 1, glm::value_ptr(glm::vec4(0.5f, 0.0f, 0.5f, 1.0f))); // Purple
  shapes[TRIANGLE].draw();

  glUniformMatrix4fv(MatrixId, 1, GL_FALSE, glm::value_ptr(matrices.at("fifth_triangle_transform")));
  glUniform4fv(ColorId, 1, glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f))); // Red
  shapes[TRIANGLE].draw();

  // SQUARE
  glBindVertexArray(shapes[SQUARE].get_vao());
  glUniformMatrix4fv(MatrixId, 1, GL_FALSE, glm::value_ptr(matrices.at("square_transform")));
  glUniform4fv(ColorId, 1, glm::value_ptr(glm::vec4(0.0f, 0.7f, 0.0f, 1.0f))); // Green
  shapes[SQUARE].draw();

  // PARALLELOGRAM
  glBindVertexArray(shapes[PARALLELOGRAM].get_vao());
  glUniformMatrix4fv(MatrixId, 1, GL_FALSE, glm::value_ptr(matrices.at("parallelogram_transform")));
  glUniform4fv(ColorId, 1, glm::value_ptr(glm::vec4(1.0f, 0.5f, 0.0f, 1.0f))); // Orange
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
  engine.setWindow(1000, 1000, "Tangram 2D", 0, 1);
  engine.init();
  engine.run();
  exit(EXIT_SUCCESS);
}

//////////////////////////////////////////////////////////////////////////// END

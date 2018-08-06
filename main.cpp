// STBI for image loading
#include "Includes.h"
#include "Rectangle.hpp"

int WIDTH = 800, HEIGHT = 600;

GLFWwindow* window;
glm::mat4 projection;

GLuint shaderProgram;

Rectangle* rectangles[40][20];

int rounds = 0;
int points = 0;

void printOut(char* string) {
    std::cout << string << std::endl;
}

// Callback function called upon click of mouse
void mouseClick(GLFWwindow* window, int button, int action, int mods) {

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

        double x, y;
        glfwGetCursorPos(window, &x, &y);

        //std::cout << "Mouse at:" << x << ":" << y << std::endl;

        int selectionX = x / 40;
        int selectionY = y / 15;


        if (rectangles[selectionY][selectionX]->show == 0) {
            return;
        }

        rectangles[selectionY][selectionX]->show = 0;

        int removed = 0;
        int multipliers[] = {10, 5, 2, 1};

        for (int r = 0; r < 40; r = r + 1) {
            for (int c = 0; c < 20; c = c + 1) {
                double distanceRed = rectangles[c][r]->r - rectangles[selectionY][selectionX]->r;
                double distanceGreen = rectangles[c][r]->g - rectangles[selectionY][selectionX]->g;
                double distanceBlue = rectangles[c][r]->b - rectangles[selectionY][selectionX]->b;

                double distance = sqrt(pow(distanceRed, 2) + pow(distanceGreen, 2) + pow(distanceBlue, 2));
                double distanceMax = sqrt(pow((0-255), 2) + pow((0-255), 2) + pow((0-255), 2));

                if (distance / distanceMax <= 0.15) {
                    rectangles[c][r]->show = 0;
                    removed = removed + 1;
                }
            }
        }

        points = points + (removed * multipliers[rounds]);
        rounds = rounds + 1;

        std::cout << "ROUND " << rounds << ":\nRemoved " << removed << " rectangles\nPoints: " << removed * multipliers[rounds-1] << "\n" << std::endl;

        if (rounds == 4) {
            std::cout << "Game Over! Total points: " << points << std::endl;
            return;
        }
    }
}

int main() {

    srand((unsigned) time (0));
	glfwInit();

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL features
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // Prints the version of OpenGL
    const char* glfwversion = glfwGetVersionString();
    printf("GLFW %s\n", glfwversion);

    // Creates window with starting dimensions set in WIDTH and HEIGHT
    window = glfwCreateWindow(WIDTH, HEIGHT, "Colors", nullptr, nullptr);

    // Get framebuffer size to render viewport in proper scale - useful for HiDPI screens
    int fbWidth;
    int fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

    if (window == nullptr) {
        std::cout << "Failed to create GLFW Window" << std::endl;
        glfwTerminate();

        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW." << std::endl;
        return EXIT_FAILURE;
    }

    // Sets up initial ortho and viewport
    projection =  glm::ortho(0.0f, (float) WIDTH, (float) HEIGHT, 0.0f, 0.1f, 100.0f);
    glViewport(0, 0, fbWidth, fbHeight);

    for (int r = 0; r < 40; r = r + 1) {
        for (int c = 0; c < 20; c = c + 1) {
            double red = rand() % 255;
            double green = rand() % 255;
            double blue = rand() % 255;

            // Eliminate colors too dark because of background
            if (red <= 50 && green <= 50 && blue <= 50) {
                red = red + 30;
                green = green + 30;
                blue = blue + 30;
            }

            rectangles[r][c] = new Rectangle(red, green, blue, 1);
        }
    }

    // Vertices - height: 800/40 = 20, width = 800/20 = 40
    GLfloat vertices[] = {
            +0.0f,   +0.0f,  -0.1f, // top-left
            +0.0f,  +15.0f,  -0.1f, // bottom-left
            +40.0f, +15.0f,  -0.1f, // bottom-right
            +40.0f,  +0.0f,  -0.1f  // top-right
    };

    GLfloat identity[] = {
        +1.0f, +0.0f, +0.0f, +0.0f,
        +0.0f, +1.0f, +0.0f, +0.0f,
        +0.0f, +0.0f, +1.0f, +0.0f,
        +0.0f, +0.0f, +0.0f, +1.0f
    };

    unsigned int indices[6];

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 3;
    indices[3] = 1;
    indices[4] = 2;
    indices[5] = 3;

    GLuint vao, vbo, ebo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    const GLchar* vertexShader =
    "#version 410 core\n"
    "layout(location=0) in vec3 vp;"
    "uniform mat4 proj;"
    "uniform mat4 matrix;"
    "uniform float tx;"
    "uniform float ty;"
    "void main () {"
    "   gl_Position = proj * vec4(vp.x + tx, vp.y + ty, vp.z, 1.0);"
    "}";
    
    const GLchar* fragmentShader =
    "#version 410 core\n"
    "uniform vec3 color;"
    "out vec4 fragColor;"
    "void main () {"
    "   fragColor = vec4(color, 1.0f);"
    "}";

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertexShader, nullptr);
	glCompileShader(vs);
    
    GLint success;
    GLchar infoLog[512];
    
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    
    if (!success) {
        glGetShaderInfoLog(vs, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragmentShader, nullptr);
	glCompileShader(fs);
    
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    
    if (!success) {
        glGetShaderInfoLog(fs, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vs);
	glAttachShader(shaderProgram, fs);
	glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    std::cout << infoLog << std::endl;

    glDeleteShader(vs);
    glDeleteShader(fs);

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "proj"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "matrix"), 1, GL_FALSE, identity);
    //glUniform3f(glGetUniformLocation(shaderProgram, "color"), 100.0f/255.0f, 50.0f/255.0f, 60.0f/255.0f);

    float tx;
    float ty;

    while (!glfwWindowShouldClose(window)) {

        // Sets mouse click callback
        glfwSetMouseButtonCallback(window, mouseClick);

        // Clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ty = 0.0f;

        glBindVertexArray(vao);

        for (int r = 0; r < 40; r = r + 1) {
            tx = 0.0f;
            for (int c = 0; c < 20; c = c + 1) {
                if (rectangles[r][c]->show == 1) {
                    glUniform1f(glGetUniformLocation(shaderProgram, "tx"), tx);
                    glUniform1f(glGetUniformLocation(shaderProgram, "ty"), ty);
                    glUniform3f(glGetUniformLocation(shaderProgram, "color"), rectangles[r][c]->r/255, rectangles[r][c]->g/255, rectangles[r][c]->b/255);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                }
                tx = tx + 40.0f;
            }
            ty = ty + 15.0f;
        }


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);

    glDeleteBuffers(1, &vbo);

    glDeleteBuffers(1, &ebo);

    glfwTerminate();

	return EXIT_SUCCESS;
}

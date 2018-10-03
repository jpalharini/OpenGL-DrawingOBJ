#include "Includes.h"
#include "Mesh.h"
#include "math_funcs.h"
#define _USE_MATH_DEFINES
#include <math.h>


using namespace std;

const int WIDTH = 800;
const int HEIGHT = 600;

#define ONE_DEG_IN_RAD ( 2.0 * M_PI ) / 360.0

Mesh* read (string filename) {
    Mesh* mesh = new Mesh;
    Group *g = nullptr;

    ifstream arq(filename);

    while(!arq.eof()) {
        string line;
        getline(arq,line);
        stringstream sline;
        // Load string into stream (memory)
        sline << line;
        string temp;
        // Reads identifier of object being read
        sline >> temp;
        // If vertex
        if (temp == "mtllib") {
            continue;
        } else if (temp == "v") {
            float x, y, z;
            sline >> x >> y >> z;
            glm::vec3 *v = new glm::vec3(x, y, z);
            mesh->vertex.push_back(v);
        } else if (temp == "vt") {
            float x, y;
            sline >> x >> y;
            glm::vec2 *v = new glm::vec2(x, y);
            mesh->mappings.push_back(v);
        } else if (temp == "vn") {
            float x, y, z;
            sline >> x >> y >> z;
            glm::vec3 *v = new glm::vec3(x, y, z);
            mesh->normals.push_back(v);
        } else if (temp == "g") {
            string inName;
            sline >> inName;
            g = new Group(inName, "default");
        } else if (temp == "usemtl") {
            if (g == nullptr) {
                g = new Group("default", "default");
            }
            string inMaterial;
            sline >> inMaterial;
            g->material = inMaterial;
        } else if (temp == "f") { // If face
            if (g == nullptr) {
                g = new Group("default", "default");
            }
            Face *f = new Face();
            while(!sline.eof()) {
                string token;
                sline >> token;
                stringstream stoken;
                stoken << token;
                string aux[3];
                int countParam = 0;
                do {
                    getline(stoken, aux[countParam], '/');
                    countParam = countParam + 1;
                } while (!stoken.eof());
                for (int i = 0; i < 3; i = i + 1) {
                    switch (i) {
                        case 0:
                            if (aux[i].empty()) {
                                f->verts.push_back(-1);
                            } else {
                                f->verts.push_back(stoi(aux[i])-1);
                            }
                            break;
                        case 1:
                            if (aux[i].empty()) {
                                f->texts.push_back(-1);
                            } else {
                                f->texts.push_back(stoi(aux[i])-1);
                            }
                            break;
                        case 2:
                            if (aux[i].empty()) {
                                f->norms.push_back(-1);
                            } else {
                                f->norms.push_back(stoi(aux[i])-1);
                            }
                            break;
                    }
                }
            }
            g->faces.push_back(f);
        }
    }
    mesh->groups.push_back(g);
    return mesh;
}

void loadVBO(Group* group) {
    GLuint vao = group->vao;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    for (unsigned int i = 0; i < group->faces.size(); i = i+1) {
        glGenBuffers (1, &group->faces.at(i)->vbo);
        glBindBuffer (GL_ARRAY_BUFFER, group->faces.at(i)->vbo);
        glBufferData (GL_ARRAY_BUFFER, group->faces.at(i)->verts.size() * sizeof (glm::vec3), &group->faces.at(i)->verts, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    }
}

int main () {
    GLFWwindow* window = NULL;

    const GLubyte* renderer;
    const GLubyte* version;

    if (!glfwInit ()) {
        fprintf (stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }

    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint (GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow (WIDTH, HEIGHT, "CGR - GLSL - 03 - Moving Triangle", NULL, NULL);

    if (!window) {
        fprintf (stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent (window);
    /* start GLEW extension handler */
    glewExperimental = GL_TRUE;
    glewInit ();

    /* get version info */
    renderer = glGetString (GL_RENDERER); /* get renderer string */
    version = glGetString (GL_VERSION); /* version as a string */
    printf ("Renderer: %s\n", renderer);
    printf ("OpenGL version supported %s\n", version);

    int fbWidth;
    int fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

    float matrix[] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
    };

    GLfloat points[] = {
            0.0f, 0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f
    };

    GLfloat colours[] = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint points_vbo;
    glGenBuffers(1, &points_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    GLuint colours_vbo;
    glGenBuffers(1, &colours_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, colours_vbo);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colours, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    /* these are the strings of code for the shaders
    the vertex shader positions each vertex point */
    const char* vertex_shader =
            "#version 410\n"
            "layout(location=0) in vec3 vp;"
            "layout(location=1) in vec3 vc;"
            "uniform mat4 projection, view;"
            "out vec3 color;"
            "void main () {"
            "   color = vc;"
            "	gl_Position = projection * view * vec4 (vp, 1.0);"
            "}";

    /* the fragment shader colours each fragment (pixel-sized area of the
    triangle) */
    const char* fragment_shader =
            "#version 410\n"
            "in vec3 color;"
            "out vec4 frag_color;"
            "void main () {"
            "	frag_color = vec4 (color, 1.0);"
            "}";

    /* GL shader objects for vertex and fragment shader [components] */
    GLuint vs, fs;
    /* GL shader programme object [combined, to link] */
    GLuint shader_programme;

    /* tell GL to only draw onto a pixel if the shape is closer to the viewer */
    glEnable (GL_DEPTH_TEST); /* enable depth-testing */
    glDepthFunc (GL_LESS); /*depth-testing interprets a smaller value as "closer"*/

    vs = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (vs, 1, &vertex_shader, NULL);
    glCompileShader (vs);

    fs = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (fs, 1, &fragment_shader, NULL);
    glCompileShader (fs);

    shader_programme = glCreateProgram ();
    glAttachShader (shader_programme, fs);
    glAttachShader (shader_programme, vs);
    glLinkProgram (shader_programme);

    // Projection matrix
    float near = 0.1f;									// clipping plane
    float far = 100.0f;									// clipping plane
    float fov = 67.0f * ONE_DEG_IN_RAD; // convert 67 degrees to radians
    float aspect = (float) WIDTH / (float) HEIGHT; // aspect ratio
    // matrix components
    float range = tan(fov * 0.5f) * near;
    float Sx = (2.0f * near) / (range * aspect + range * aspect);
    float Sy = near / range;
    float Sz = -(far + near) / (far - near);
    float Pz = -(2.0f * far * near) / (far - near);

//    GLfloat projection[] = {
//            Sx,    +0.0f, +0.0f,  +0.0f,
//            +0.0f, Sy,    +0.0f,  +0.0f,
//            +0.0f, +0.0f, Sz,     Pz,
//            +0.0f, +0.0f, -1.0f,  +0.0f
//    };

    GLfloat projection[] = {
            Sx,	 0.0f, 0.0f, 0.0f,
            0.0f, Sy, 0.0f, 0.0f,
            0.0f, 0.0f, Sz,	-1.0f,
            0.0f, 0.0f, Pz,	0.0f
    };

//    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 2.0f);
//    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
//    glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
//
//    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
//    glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
//    glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
//
//    GLfloat view[] = {
//            +cameraRight.x,     +cameraRight.y,     +cameraRight.z,     -cameraPos.x,
//            +cameraUp.x,        +cameraUp.y,        +cameraUp.z,        -cameraPos.y,
//            -cameraDirection.x, -cameraDirection.y, -cameraDirection.z, -cameraPos.z,
//            +0.0f,               +0.0f,             +0.0f,              +1.0f
//    };

    /* create VIEW MATRIX */
    float cam_speed = 1.0f;			 // 1 unit per second
    float cam_yaw_speed = 10.0f; // 10 degrees per second
    float cam_pos[] = { 0.0f, 0.0f, 3.0f }; // don't start at zero, or we will be too close
    float cam_yaw = 0.0f;				// y-rotation in degrees
    mat4 T = translate(identity_mat4(), vec3( -cam_pos[0], -cam_pos[1], -cam_pos[2] ) );
    mat4 R = rotate_y_deg( identity_mat4(), -cam_yaw );
    mat4 view_mat = R * T;

    GLint projectionLocation = glGetUniformLocation(shader_programme, "projection");
    GLint viewLocation = glGetUniformLocation(shader_programme, "view");

    glUseProgram (shader_programme);

    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, projection);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, view_mat.m);

    Mesh* readMesh = read("../Resources/mesa01.obj");
    loadVBO(readMesh->groups[0]);

    //cout << "Mesh should be in memory" << endl;

    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK);		// cull back face
    glFrontFace(GL_CW);

    while (!glfwWindowShouldClose (window)) {

        //glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, matrix);

        /* wipe the drawing surface clear */
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, WIDTH, HEIGHT);

//        glBindVertexArray (readMesh->groups[0]->vao);
//        /* draw points 0-3 from the currently bound VAO with current in-use shader*/
//        glDrawArrays (GL_TRIANGLES, 0, 12*3);

        glBindVertexArray(readMesh->groups[0]->vao);
        // draw points 0-3 from the currently bound VAO with current in-use shader
        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* update other events like input handling */
        glfwPollEvents ();
        /* put the stuff we've been drawing onto the display */
        glfwSwapBuffers (window);
        if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)){
            glfwSetWindowShouldClose(window, 1);
        }
    }

    /* close GL context and any other GLFW resources */
    glfwTerminate();
    return 0;
}
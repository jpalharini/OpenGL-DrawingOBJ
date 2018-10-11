#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"

#include "Includes.h"
#include "Mesh.h"
#include "math_funcs.h"
#include "Material.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include "Utils.h"

using namespace std;

const int WIDTH = 1280;
const int HEIGHT = 720;

vector<Material*> materials;

#define ONE_DEG_IN_RAD (( 2.0 * M_PI ) / 360.0)

GLuint loadTexture(const char* filename) {
    // Enabling texture processing
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 2);

    int texWidth, texHeight, nrChannels;
    unsigned char* data;

    GLuint texture;

    // Loading image with filename from parameter
    data = stbi_load(filename, &texWidth, &texHeight, &nrChannels, 0);

    glGenTextures(1, &texture);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, texture);

    if (data) {
        // The image I chose has no alpha channel, so using GL_RGB to process it
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);

    return texture;
}

// Read a MTL file
void readMTL(const string filename) {
    Material* m = nullptr;

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

        if (temp == "newmtl") {
            // If a material was already loaded, add it to vector of materials before creating a new one
            if (m != nullptr) {
                materials.push_back(m);
            }
            string mtlName;
            sline >> mtlName;
            m = new Material(mtlName);
        } else if (temp == "Ka") {
            float r, g, b;
            sline >> r >> g >> b;
            m->ka = new glm::vec3(r, g, b);
        } else if (temp == "Kd") {
            float r, g, b;
            sline >> r >> g >> b;
            m->kd = new glm::vec3(r, g, b);
        } else if (temp == "Ks") {
            float r, g, b;
            sline >> r >> g >> b;
            m->ks = new glm::vec3(r, g, b);
        } else if (temp == "Ns") {
            float ns;
            sline >> ns;
            m->ns = ns;
        } else if (temp == "map_Kd") {
            string textureFile;
            sline >> textureFile;
            m->texture = loadTexture(textureFile.c_str());
        }
    }
    materials.push_back(m);
}

// Read OBJ file
Mesh* readOBJ(const string filename) {
    auto mesh = new Mesh;
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
        // If it's a material, call readMTL()
        if (temp == "mtllib") {
            string mtlFile;
            sline >> mtlFile;
            readMTL(mtlFile);
        // If vertex
        } else if (temp == "v") {
            float x, y, z;
            sline >> x >> y >> z;
            auto *v = new glm::vec3(x, y, z);
            mesh->vertex.push_back(v);
        // If texture coordinate
        } else if (temp == "vt") {
            float x, y;
            sline >> x >> y;
            auto *v = new glm::vec2(x, y);
            mesh->mappings.push_back(v);
        // If normal
        } else if (temp == "vn") {
            float x, y, z;
            sline >> x >> y >> z;
            auto *v = new glm::vec3(x, y, z);
            mesh->normals.push_back(v);
        } else if (temp == "g") {
            if (g != nullptr) {
                mesh->groups.push_back(g);
            }
            string inName;
            sline >> inName;
            g = new Group(inName, "default");
        // Check if a material is referenced
        } else if (temp == "usemtl") {
            // If a group doesn't exist, create a default one
            if (g == nullptr) {
                g = new Group("default", "default");
            }
            string inMaterial;
            sline >> inMaterial;
            g->material = inMaterial;
        // If face
        } else if (temp == "f") {
            if (g == nullptr) {
                g = new Group("default", "default");
            }
            auto *f = new Face();
            // Read all faces until EOF
            while(!sline.eof()) {
                string token;
                sline >> token;
                if (token.empty()) {
                    continue;
                }
                stringstream stoken;
                stoken << token;
                string aux[3];
                // Count parameters of face
                int countParam = -1;
                do {
                    countParam = countParam + 1;
                    getline(stoken, aux[countParam], '/');
                } while (!stoken.eof());
                for (int i = 0; i < 3; i = i + 1) {
                    switch (i) {
                        // In position 0, it's an index for vertex
                        case 0:
                            if (aux[i].empty()) {
                                f->verts.push_back(-1);
                            } else {
                                f->verts.push_back(stoi(aux[i])-1);
                            }
                            break;
                        // In position 1, it's an index for texture mapping
                        case 1:
                            if (aux[i].empty()) {
                                f->texts.push_back(-1);
                            } else {
                                f->texts.push_back(stoi(aux[i])-1);
                            }
                            break;
                        // In position 2, it's an index for normals
                        case 2:
                            if (aux[i].empty()) {
                                f->norms.push_back(-1);
                            } else {
                                f->norms.push_back(stoi(aux[i])-1);
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
            // Add face to group
            g->faces.push_back(f);
        }
    }
    // Add group to mesh
    mesh->groups.push_back(g);
    return mesh;
}

// Load vertices, texture mappings and normals into the groups' VAOs
void loadVertices(Mesh* mesh) {

    for (Group* g : mesh->groups) {
        vector<float> vs;
        vector<float> vn;
        vector<float> vt;

        for (Face* f : g->faces) {
            for (int i = 0; i < f->verts.size(); i = i + 1) {
                int vi = f->verts[i];
                glm::vec3* v = mesh->vertex[vi];
                vs.push_back(v->x);
                vs.push_back(v->y);
                vs.push_back(v->z);
            }

            for (int i = 0; i < f->verts.size(); i = i + 1) {
                int vi = f->norms[i];
                glm::vec3* v = mesh->normals[vi];
                vn.push_back(v->x);
                vn.push_back(v->y);
                vn.push_back(v->z);
            }

            for (int i = 0; i < f->verts.size(); i = i + 1) {
                int vi = f->texts[i];
                glm::vec2* v = mesh->mappings[vi];
                vt.push_back(v->x);
                vt.push_back(v->y);
            }
        }

        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // VBO for vertices
        GLuint vboVerts;
        glGenBuffers (1, &vboVerts);
        glBindBuffer (GL_ARRAY_BUFFER, vboVerts);
        glBufferData (GL_ARRAY_BUFFER, vs.size() * sizeof (float), vs.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        // VBO for normals
        GLuint vboNorms;
        glGenBuffers (1, &vboNorms);
        glBindBuffer (GL_ARRAY_BUFFER, vboNorms);
        glBufferData (GL_ARRAY_BUFFER, vn.size() * sizeof (float), vn.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        // VBO for texture mappings
        GLuint vboTexts;
        glGenBuffers (1, &vboTexts);
        glBindBuffer (GL_ARRAY_BUFFER, vboTexts);
        glBufferData (GL_ARRAY_BUFFER, vt.size() * sizeof (float), vt.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

        // Store VAO for group
        g->vao = vao;
    }
}

int main () {
    GLFWwindow* window = nullptr;

    const GLubyte* renderer;
    const GLubyte* version;

    if (!glfwInit ()) {
        fprintf (stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }

    // Sets GLFW/OpenGL version to 4.1
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    // Disables resizing, because it's not properly implemented (shame) - TODO: implement window resizing
    glfwWindowHint (GLFW_RESIZABLE, GLFW_FALSE);

    // Sets up the window
    window = glfwCreateWindow(WIDTH, HEIGHT, "CG - TGA", nullptr, nullptr);

    if (!window) {
        fprintf (stderr, "ERROR: could not open window\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent (window);
    glewExperimental = GL_TRUE;
    glewInit ();

    // Get version info for renderer, shows GPU model
    renderer = glGetString (GL_RENDERER);
    version = glGetString (GL_VERSION);
    printf ("Renderer: %s\n", renderer);
    printf ("OpenGL version supported %s\n", version);

    // Gets the framebuffer size, useful for HiDPI displays
    int fbWidth;
    int fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

    // Loads vertex and fragment shaders from external files - makes it easier to program :)
    char vertex_shader[1024 * 256];
    char fragment_shader[1024 * 256];
    parse_file_into_str("../src/shaders/VertexShader.glsl", vertex_shader, 1024 * 256);
    parse_file_into_str("../src/shaders/FragmentShader.glsl", fragment_shader, 1024 * 256);

    GLuint vsID, fsID;
    GLuint shaderProgram;

    // Sets up depth testing
    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LESS);

    vsID = glCreateShader (GL_VERTEX_SHADER);
    auto vs = (const GLchar *) vertex_shader;
    glShaderSource (vsID, 1, &vs, nullptr);
    glCompileShader (vsID);

    fsID = glCreateShader (GL_FRAGMENT_SHADER);
    auto fs = (const GLchar *) fragment_shader;
    glShaderSource (fsID, 1, &fs, nullptr);
    glCompileShader (fsID);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, fsID);
    glAttachShader(shaderProgram, vsID);
    glLinkProgram(shaderProgram);

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

    GLfloat projection[] = {
            Sx,	 0.0f, 0.0f, 0.0f,
            0.0f, Sy, 0.0f, 0.0f,
            0.0f, 0.0f, Sz,	-1.0f,
            0.0f, 0.0f, Pz,	0.0f
    };


    /* create VIEW MATRIX */
    float cam_speed = 3.0f;			 // 1 unit per second
    float cam_yaw_speed = 10.0f; // 10 degrees per second
    float cam_pos[] = {0.0f, 11.5f, 20.0f}; // don't start at zero, or we will be too close
    float cam_yaw = 0.0f;				// y-rotation in degrees
    mat4 T = translate(identity_mat4(), vec3( -cam_pos[0], -cam_pos[1], -cam_pos[2] ) );
    mat4 R = rotate_y_deg( identity_mat4(), -cam_yaw );
    mat4 view = R * T;

    mat4 model = identity_mat4();

    GLint projectionLocation = glGetUniformLocation(shaderProgram, "projection");
    GLint viewLocation = glGetUniformLocation(shaderProgram, "view");
    GLint modelLocation = glGetUniformLocation(shaderProgram, "model");

    glUseProgram (shaderProgram);

    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, projection);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, view.m);
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, model.m);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    // Rendering points in counter-clockwise
    glFrontFace(GL_CCW);

    // Reads the mesh from file
    Mesh* readMesh = readOBJ("../resources/cenaPaintball.obj");
    // Load VAOs and VBOs from read mesh
    loadVertices(readMesh);

    // Define background color - default is black, values below are for a dark grey which is useful for troubleshooting
    glClearColor(0.2f, 0.2f, 0.2f, 0.2f);

    // Gets logging from shaders
    int max_length = 2048;
    int actual_length = 0;
    char logVS[2048];
    glGetShaderInfoLog(vsID, max_length, &actual_length, logVS);
    printf("Vertex Shader info log:\n%s\n", logVS);

    char logFS[2048];
    glGetShaderInfoLog(fsID, max_length, &actual_length, logFS);
    printf("Fragment Shader info log:\n%s\n", logFS);

    char logSP[2048];
    glGetProgramInfoLog(shaderProgram, max_length, &actual_length, logSP);
    printf("Shader Program info log:\n%s", logSP);

    while (!glfwWindowShouldClose (window)) {
        // To make movements smoother, we measure the framerate using this logic
        static double previous_seconds = glfwGetTime();
        double current_seconds = glfwGetTime();
        double elapsed_seconds = current_seconds - previous_seconds;
        previous_seconds = current_seconds;

        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Sets up the viewport size, note that we use framebuffer values to accomodate HiDPI displays
        glViewport(0, 0, WIDTH*2, HEIGHT*2);

        glUseProgram (shaderProgram);

        GLuint texture;
        for (Group* g : readMesh->groups) {
            for (Material* m : materials) {
                if (m->name.data() == g->material) {
                    texture = m->texture;
                    glUniform3f(glGetUniformLocation(shaderProgram, "Ka"), m->ka->r, m->ka->g, m->ka->b);
                    glUniform3f(glGetUniformLocation(shaderProgram, "Kd"), m->kd->r, m->kd->g, m->kd->b);
                    glUniform3f(glGetUniformLocation(shaderProgram, "Ks"), m->ks->r, m->ks->g, m->ks->b);
                    glUniform1f(glGetUniformLocation(shaderProgram, "Ns"), m->ns);
                }
            }
            glActiveTexture(GL_TEXTURE0);

            glBindTexture(GL_TEXTURE_2D, texture);

            glUniform1i(glGetUniformLocation(shaderProgram, "theTexture"), 0);

            glBindVertexArray(g->vao);
            // draw points 0-3 from the currently bound VAO with current in-use shader
            glDrawArrays(GL_TRIANGLES, 0, g->faces.size()*3);
        }



        /* update other events like input handling */
        glfwPollEvents ();

        // Camera movements are processed below
        bool cam_moved = false;
        if (glfwGetKey(window, GLFW_KEY_A)) {
            cam_pos[0] -= cam_speed * elapsed_seconds;
            cam_moved = true;
        }
        if (glfwGetKey(window, GLFW_KEY_D)) {
            cam_pos[0] += cam_speed * elapsed_seconds;
            cam_moved = true;
        }
        if (glfwGetKey(window, GLFW_KEY_PAGE_UP)) {
            cam_pos[1] += cam_speed * elapsed_seconds;
            cam_moved = true;
        }
        if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN)) {
            cam_pos[1] -= cam_speed * elapsed_seconds;
            cam_moved = true;
        }
        if (glfwGetKey(window, GLFW_KEY_W)) {
            cam_pos[2] -= cam_speed * elapsed_seconds;
            cam_moved = true;
        }
        if (glfwGetKey(window, GLFW_KEY_S)) {
            cam_pos[2] += cam_speed * elapsed_seconds;
            cam_moved = true;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT)) {
            cam_yaw += cam_yaw_speed * elapsed_seconds;
            cam_moved = true;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
            cam_yaw -= cam_yaw_speed * elapsed_seconds;
            cam_moved = true;
        }

        // Updating view matrix to reflect camera movement
        if (cam_moved) {
            mat4 T = translate(identity_mat4(), vec3(-cam_pos[0], -cam_pos[1], -cam_pos[2]));
            mat4 R = rotate_y_deg(identity_mat4(), -cam_yaw);
            mat4 view = R * T;
            glUniformMatrix4fv(viewLocation, 1, GL_FALSE, view.m);
        }

        glfwSwapBuffers (window);

        // If ESC is pressed, close
        if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)){
            glfwSetWindowShouldClose(window, 1);
        }
    }

    glfwTerminate();
    return 0;
}
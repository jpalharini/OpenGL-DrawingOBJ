#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"

#include "Includes.h"
#include "Mesh.h"
#include "math_funcs.h"
#include "Material.h"

#define _USE_MATH_DEFINES
#include <cmath>


using namespace std;

const int WIDTH = 800;
const int HEIGHT = 600;

vector<Material*> materials;

#define ONE_DEG_IN_RAD (( 2.0 * M_PI ) / 360.0)

GLuint loadTexture(const char* filename) {
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 2);

    int texWidth, texHeight, nrChannels;
    unsigned char* data;

    GLuint texture;

    data = stbi_load(filename, &texWidth, &texHeight, &nrChannels, 0);

    glGenTextures(1, &texture);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, texture);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture for tiles" << std::endl;
    }

    stbi_image_free(data);

    return texture;
}

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
        // Reads identifier of object being readOBJ
        sline >> temp;
        // If vertex
        if (temp == "mtllib") {
            string mtlFile;
            sline >> mtlFile;
            readMTL(mtlFile);
        } else if (temp == "v") {
            float x, y, z;
            sline >> x >> y >> z;
            auto *v = new glm::vec3(x, y, z);
            mesh->vertex.push_back(v);
        } else if (temp == "vt") {
            float x, y;
            sline >> x >> y;
            auto *v = new glm::vec2(x, y);
            mesh->mappings.push_back(v);
        } else if (temp == "vn") {
            float x, y, z;
            sline >> x >> y >> z;
            auto *v = new glm::vec3(x, y, z);
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
            auto *f = new Face();
            while(!sline.eof()) {
                string token;
                sline >> token;
                if (token.empty()) {
                    continue;
                }
                stringstream stoken;
                stoken << token;
                string aux[3];
                int countParam = -1;
                do {
                    countParam = countParam + 1;
                    getline(stoken, aux[countParam], '/');
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
                        default:
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

void loadVertices(Mesh* mesh) {

    for (Group* g : mesh->groups) {
        vector<float> vs;
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
                int vi = f->texts[i];
                glm::vec2* v = mesh->mappings[vi];
                vt.push_back(v->x);
                vt.push_back(v->y);
            }
        }

        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        GLuint vboVerts;
        glGenBuffers (1, &vboVerts);
        glBindBuffer (GL_ARRAY_BUFFER, vboVerts);
        glBufferData (GL_ARRAY_BUFFER, vs.size() * sizeof (float), vs.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        // vboTexts
        GLuint vboTexts;
        glGenBuffers (1, &vboTexts);
        glBindBuffer (GL_ARRAY_BUFFER, vboTexts);
        glBufferData (GL_ARRAY_BUFFER, vt.size() * sizeof (float), vt.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

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

    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint (GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow (WIDTH, HEIGHT, "CGR - GLSL - 03 - Moving Triangle", nullptr, nullptr);

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

    /* these are the strings of code for the shaders
    the vertex shader positions each vertex point */
    const char* vertex_shader =
            "#version 410\n"
            "layout(location=0) in vec3 vp;"
            "layout(location=1) in vec3 vn;"
            "layout(location=2) in vec2 vt;"
            "uniform mat4 projection, view;"
            "out vec2 texCoord;"
            "void main () {"
            "   texCoord = vt;"
            "	gl_Position = projection * view * vec4 (vp, 1.0);"
            "}";

    /* the fragment shader colours each fragment (pixel-sized area of the
    triangle) */
    const char* fragment_shader =
            "#version 410\n"
            "in vec2 texCoord;"
            "uniform sampler2D theTexture;"
            "out vec4 color;"
            "void main () {"
            "	color = texture(theTexture, texCoord);"
            "}";

    /* GL shader objects for vertex and fragment shader [components] */
    GLuint vs, fs;
    /* GL shader programme object [combined, to link] */
    GLuint shader_programme;

    /* tell GL to only draw onto a pixel if the shape is closer to the viewer */
    glEnable (GL_DEPTH_TEST); /* enable depth-testing */
    glDepthFunc (GL_LESS); /*depth-testing interprets a smaller value as "closer"*/

    vs = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (vs, 1, &vertex_shader, nullptr);
    glCompileShader (vs);

    fs = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (fs, 1, &fragment_shader, nullptr);
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

    GLfloat projection[] = {
            Sx,	 0.0f, 0.0f, 0.0f,
            0.0f, Sy, 0.0f, 0.0f,
            0.0f, 0.0f, Sz,	-1.0f,
            0.0f, 0.0f, Pz,	0.0f
    };


    /* create VIEW MATRIX */
    float cam_speed = 1.0f;			 // 1 unit per second
    float cam_yaw_speed = 10.0f; // 10 degrees per second
    float cam_pos[] = {0.0f, 3.0f, 15.0f }; // don't start at zero, or we will be too close
    float cam_yaw = 0.0f;				// y-rotation in degrees
    mat4 T = translate(identity_mat4(), vec3( -cam_pos[0], -cam_pos[1], -cam_pos[2] ) );
    mat4 R = rotate_y_deg( identity_mat4(), -cam_yaw );
    mat4 view_mat = R * T;

    GLint projectionLocation = glGetUniformLocation(shader_programme, "projection");
    GLint viewLocation = glGetUniformLocation(shader_programme, "view");

    glUseProgram (shader_programme);

    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, projection);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, view_mat.m);

    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK);		// cull back face
    glFrontFace(GL_CCW);

    Mesh* readMesh = readOBJ("../resources/mesa01.obj");
    loadVertices(readMesh);

    //cout << "Mesh should be in memory" << endl;

    glClearColor( 0.2, 0.2, 0.2, 1.0 );

    while (!glfwWindowShouldClose (window)) {

        //glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, matrix);

        /* wipe the drawing surface clear */
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, WIDTH*2, HEIGHT*2);


        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_2D, materials[0]->texture);

        glUniform1i(glGetUniformLocation(shader_programme, "theTexture"), 0);

        glBindVertexArray(readMesh->groups[0]->vao);
        // draw points 0-3 from the currently bound VAO with current in-use shader
        glDrawArrays(GL_TRIANGLES, 0, readMesh->groups[0]->faces.size()*3);

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
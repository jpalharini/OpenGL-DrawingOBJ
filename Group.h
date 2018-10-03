//
// Created by João Pedro Berton Palharini on 8/26/18.
//

#ifndef OPENGL_CG_GROUP_H
#define OPENGL_CG_GROUP_H


#include "Face.hpp"

class Group {
public:
    string name;
    string material;
    vector<Face*> faces;
    GLuint vao;
    GLuint vbo;

    Group (string n, string m);
};

#endif //OPENGL_CG_GROUP_H

//
// Created by João Pedro Berton Palharini on 10/3/18.
//

#ifndef OPENGL_CG_MATERIAL_H
#define OPENGL_CG_MATERIAL_H


#include "Includes.h"

class Material {
public:
    string name;
    glm::vec3* ka;
    glm::vec3* kd;
    glm::vec3* ks;
    float ns;
    GLuint texture;

    Material (string nameI);

    Material (string name, glm::vec3* kaI, glm::vec3* kdI, glm::vec3* ksI, float nsI);
};


#endif //OPENGL_CG_MATERIAL_H

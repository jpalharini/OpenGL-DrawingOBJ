//
// Created by João Pedro Berton Palharini on 10/3/18.
//

#include "Material.h"

Material::Material(string nameI) {
    name = nameI;
}

Material::Material(string nameI, glm::vec3* kaI, glm::vec3* kdI, glm::vec3* ksI, float nsI) {
    name = nameI;
    ka = kaI;
    kd = kdI;
    ks = ksI;
    ns = nsI;
}
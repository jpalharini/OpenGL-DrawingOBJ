//
//  Layer.hpp
//  OpenGL-BabySteps
//
//  Created by João Pedro Berton Palharini on 4/23/18.
//  Copyright © 2018 Unisinos. All rights reserved.
//
#include <stdio.h>

// GLFW
#include "Includes.h"

using namespace std;

class Rectangle {
public:
    double r;
    double g;
    double b;
    bool show;

    Rectangle(double inR, double inG, double inB, bool inShow);
};

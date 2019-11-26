//
//  World.cpp
//  Chuang
//
//  Created by 孙川 on 2019/11/11.
//  Copyright © 2019 孙川. All rights reserved.


#include "World.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

#include "imgui/imgui.h"
#include "DebugDraw.hpp"
#include "glm/glm.hpp"

World::World()
{

}

World::~World()
{

}

json World::OnDataRecieve(json &data)
{
    std::string s = data.dump();
    std::cout << "Data Recieved: "<< s << std::endl;
    
    json j = "{ \"happy\": true, \"pi\": 3.141 }"_json;
    return j;
}

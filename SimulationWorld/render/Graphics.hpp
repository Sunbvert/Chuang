//
//  Graphics.hpp
//  Chuang
//
//  Created by 孙川 on 2019/11/7.
//  Copyright © 2019 孙川. All rights reserved.
//

#ifndef Graphics_hpp
#define Graphics_hpp

#include <stdio.h>

class Graphics {
public:
    Graphics() {};
    virtual ~Graphics() {}
    
    virtual void OnUpdate(float deltaTime) {}
    virtual void OnRender() {}
    virtual void OnImGuiRender() {}
};

#endif /* Graphics_hpp */

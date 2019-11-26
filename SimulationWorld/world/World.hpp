//
//  World.hpp
//  Chuang
//
//  Created by 孙川 on 2019/11/11.
//  Copyright © 2019 孙川. All rights reserved.
//

#ifndef World_hpp
#define World_hpp

#include <stdio.h>

#include "MqConnCallback.hpp"
#include "json.hpp"
using json = nlohmann::json;

class World : MqConnCallback
{
public:
    World();
    ~World();
    
protected:
    json OnDataRecieve(json &data) override;
private:
};

#endif /* World_hpp */

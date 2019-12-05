//
//  MqConnInterface.hpp
//  Chuang
//
//  Created by 孙川 on 2019/11/25.
//  Copyright © 2019 孙川. All rights reserved.
//

#ifndef MqConnCallback_hpp
#define MqConnCallback_hpp

#include "json.hpp"
using json = nlohmann::json;

class MqConnCallback
{
public:
    virtual json OnDataRecieve(json &data) = 0;
};

#endif /* MqConnInterface_hpp */

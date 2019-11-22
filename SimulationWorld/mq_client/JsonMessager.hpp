//
//  JsonMessager.hpp
//  Chuang
//
//  Created by 孙川 on 2019/11/21.
//  Copyright © 2019 孙川. All rights reserved.
//

#ifndef JsonMessager_hpp
#define JsonMessager_hpp

#include <stdio.h>

#include "FibonacciRpcClient.hpp"
#include "json.hpp"

using json = nlohmann::json;

class JsonMessager
{
public:
    JsonMessager();
    ~JsonMessager();
    
    
};

#endif /* JsonMessager_hpp */

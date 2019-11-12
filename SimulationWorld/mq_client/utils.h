//
//  utils.h
//  amqpTest
//
//  Created by 孙川 on 2019/11/6.
//  Copyright © 2019 孙川. All rights reserved.
//
//#pragma once
#ifndef utils_h
#define utils_h

void die(const char *fmt, ...);
extern void die_on_error(int x, char const *context);
extern void die_on_amqp_error(amqp_rpc_reply_t x, char const *context);

extern void amqp_dump(void const *buffer, size_t len);

//extern uint64_t now_microseconds(void);
//extern void microsleep(int usec);

#endif /* utils_h */

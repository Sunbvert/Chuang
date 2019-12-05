/*
This is the api reference of the Chuang project
Used for communication between Python and C++ application
Via AMPQ protocol and RabbitMQ services

Author: 孙川
Create Data: 2019年12月4日
*/

// API when Python app is server
// Call:
{
    "call": "make",
    "num_envs": 8,   // num of environment to run  in threads n parallel, accept numbers
}
// Response:
{
    "response": "make",
    "status": 1  // accept only 1 and 0, 1 for success, 0 for fail
    "observation_space": [403, 1],
    "action_space": [3, 1]  // spaces, accept array of 2 numbers
}
// Call:
{
    "call": "step",
    "action": [0.0, 0.0, ...],
}
// Response:
{
    "response": "step",
    "observation": [0.0, 0.0, ...],
    "reward": 100.0,
    "done": false,
}
// Call:
{
    "call": "reset",
}
// Response:
{
    "response": "reset",
    "observation": [0.0, 0.0, ...],
}

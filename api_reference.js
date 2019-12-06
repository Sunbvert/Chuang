/*
This is the api reference of the Chuang project
Used for communication between Python and C++ application
Via AMPQ protocol and RabbitMQ services

Author: 孙川
Create Data: 2019年12月4日
*/

// API when Cpp app is server
// Call:
{
    "call": "make",
    "num_envs": 8,   // num of environment to run  in threads n parallel, accept numbers
    "test": false
}
// Response:
{
    "response": "make",
    "status": 1  // accept only 1 and 0, 1 for success, 0 for fail
    "observation_space": [403, 1],
    "action_space": [3, 1],  // spaces, accept array of 2 numbers
}
// Call:
{
    "call": "step",
    "action": [0.0, 0.0, ...], // one dimensional array of actions
    "test": false  // accept true or false for whether render on screen
}
// Response:
{
    "response": "step",
    "observation": [0.0, 0.0, ...],  // two dimensional array of observations
    "reward": 100.0,  // one dimensional
    "done": false,
}
// Call:
{
    "call": "reset",
    "test": true  // accept true or false for whether render on screen
}
// Response:
{
    "response": "reset",
    "observation": [0.0, 0.0, ...],  // two dimensional array of observations
}

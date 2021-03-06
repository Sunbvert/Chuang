-- Box2D premake5 script.
-- https://premake.github.io/

workspace 'Chuang'
    configurations('Debug', 'Release')
    location 'Build'
    cppdialect 'C++11'
    platforms { 'x86_64' }

project 'Chuang'
    kind 'ConsoleApp'

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
        
    includedirs
    {
         'SimulationWorld/mq_client',
         'SimulationWorld/render',
         'SimulationWorld/world',
         'SimulationWorld/test',
         'SimulationWorld/vendor/cute',
         'SimulationWorld/vendor/stb_image',
         'SimulationWorld/vendor',
         'SimulationWorld/Mutilthreading',
         'SimulationWorld/vendor/nlohmann'
     }
    language 'C++'
    files
    {
        'Res/Texture/*',
        'Res/Shader/*.shader',
        'SimulationWorld/mq_client/*',
        'SimulationWorld/render/*',
        'SimulationWorld/world/*',
        'SimulationWorld/test/*',
        'SimulationWorld/vendor/cute/*',
        'SimulationWorld/vendor/imgui/*',
        'SimulationWorld/vendor/glm/*',
        'SimulationWorld/vendor/stb_image/*',
        'SimulationWorld/*.cpp',
        'SimulationWorld/*.hpp',
        'SimulationWorld/vendor/nlohmann/*',
        'SimulationWorld/Mutilthreading/*'
    }

    filter { 'system:macosx' }
        libdirs
        {
             'ExternalLibraries/macosx/Box2D/Lib',
             'ExternalLibraries/macosx/RabbitMQ/lib',
             'ExternalLibraries/macosx/GLEW/lib',
             'ExternalLibraries/macosx/GLFW/lib'
         }
         includedirs
         {
              'ExternalLibraries/macosx/Box2D/Include',
              'ExternalLibraries/macosx/RabbitMQ/header'
         }
        links
        {
            'Box2D',
            'Opengl.framework',
            'rabbitmq',
            'GLEW',
            'glfw'
        }
    filter {}

    filter { 'system:linux' }
        libdirs
        {
             'ExternalLibraries/linux/Box2D/Lib',
             'ExternalLibraries/linux/RabbitMQ/lib'
         }
         includedirs
         {
              'ExternalLibraries/linux/Box2D/Include',
              'ExternalLibraries/linux/RabbitMQ/header'
         }
        links
        {
            'Box2D',
            'GL',
            'GLEW',
            'glfw3',
            'X11',
            'Xrandr',
            'Xinerama',
            'Xcursor',
            'pthread',
            'dl',
            'rabbitmq'
        }
    filter {}

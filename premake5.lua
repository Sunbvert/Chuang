-- Box2D premake5 script.
-- https://premake.github.io/

workspace 'Chuang'
    configurations('Debug', 'Release')

project 'Chuang'
    kind 'ConsoleApp'
    libdirs { "ExternalLibraries/Box2D/Lib", "ExternalLibraries/RabbitMQ/lib" }
    includedirs { '.' }
    language 'C++'

    files
    {
        'Res/Texture/*',
        'Res/Shader/*',
        'SimulationWorld/mq_client/*',
        'SimulationWorld/render/*',
        'SimulationWorld/world/*',
        'SimulationWorld/test/*',
        'SimulationWorld/vendor/cute/*',
        'SimulationWorld/vendor/imgui/*',
        'SimulationWorld/vendor/glm/*',
        'SimulationWorld/vendor/stb_image/*',
        'SimulationWorld/*.cpp',
        'SimulationWorld/*.hpp'
    }

    filter { 'system:linux' }
        links
        {
            'Box2D',
            'GL',
            'GLEW',
            'glfw'
        }

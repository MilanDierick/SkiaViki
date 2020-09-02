workspace "SkiaViki"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["Skia"] = "Dependencies/skia"
IncludeDir["GLFW"] = "Dependencies/GLFW/include"

-- Library directories relative to root folder (solution directory)
LibraryDir = {}
LibraryDir["Skia"] = "Dependencies/skia-wasm/out/skia-wasm/release"

include "Dependencies/GLFW"

project "Playground"
    location "Playground"
    kind "ConsoleApp"
    language "C++"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    --pchheader "pch.h"
    --pchsource "pch.cpp"

    files
    {
        "%{prj.name}/**.h",
        "%{prj.name}/**.cpp"
    }

    includedirs
    {
        "%{IncludeDir.Skia}",
        "%{IncludeDir.GLFW}"
    }

    links
    {
        "%{LibraryDir.Skia}/libskia.a",
        "GLFW",
        "opengl32.lib"
    }

    filter "system:windows"
    cppdialect "C++17"
    staticruntime "On"
    systemversion "latest"

    defines
    {
        --"HL_PLATFORM_WINDOWS",
        --"HL_BUILD_DLL"
    }

    postbuildcommands
    {
        --("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
    }

    filter "configurations:Debug"
        --defines "HL_DEBUG"
        symbols "On"

    filter "configurations:Release"
        --defines "HL_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        --defines "HL_DIST"
        optimize "On"

    


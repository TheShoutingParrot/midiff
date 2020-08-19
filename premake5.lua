workspace "midiff"
        configurations { "Debug" }

        kind "ConsoleApp"
        language "C"

	includedirs { "include/" }

project "midiff"
        targetdir "bin/%{cfg.buildcfg}"

        files { "include/*.h", "src/*.c" }

        filter "configurations:Debug"
                defines { "DEBUG" }
                symbols "On"

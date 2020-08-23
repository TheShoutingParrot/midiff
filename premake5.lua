workspace "midiff"
        configurations { "Debug", "Release" }

        kind "ConsoleApp"
        language "C"

	includedirs { "include/" }

project "midiff"
        targetdir "bin/%{cfg.buildcfg}"

        files { "include/*.h", "src/*.c" }

        filter "configurations:Debug"
                defines { "DEBUG" }
		buildoptions { "-g", "-Wall", "-Wunused" }
                symbols "On"
	
	filter "configurations:Release"
		symbols "On"

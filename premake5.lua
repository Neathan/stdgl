
-- Variable definitions
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories
includes = {}
includes["GLFW"] = "libs/glfw/include"
includes["Glad"] = "libs/glad/include"
includes["GLM"] = "libs/glm"
includes["spdlog"] = "libs/spdlog/include"
includes["imgui"] = "libs/imgui"
includes["stb"] = "libs/stb"

-- ImGUI source files
imgui_src = {
	"libs/imgui/imgui.cpp",
	"libs/imgui/imgui_draw.cpp",
	"libs/imgui/imgui_widgets.cpp",
	"libs/imgui/imgui_demo.cpp",
	"libs/imgui/imgui_tables.cpp",
	"libs/imgui/backends/imgui_impl_glfw.cpp",
	"libs/imgui/backends/imgui_impl_opengl3.cpp"
}

glad_src = {
	"libs/glad/src/glad.c"
}

lib_src = {imgui_src, glad_src}

libs = {}
libs["GLFW"] = "libs/glfw/build/src/Release/"


-- Workspace and project setup
workspace "stdgl"
	configurations {"Debug", "Release"}
	platforms { "Win64" }

	startproject "stdgl_examples"

-- stdgl.lib
project "stdgl"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin/int/" .. outputdir .. "/%{prj.name}")

	files {"src/**.h", "src/**.cpp", lib_src}
	
	defines {
	}

	includedirs {
		"/src",
		"%{includes.GLFW}",
		"%{includes.Glad}",
		"%{includes.GLM}",
		"%{includes.spdlog}",
		"%{includes.imgui}",
		"%{includes.stb}"
	}
	
	libdirs {
		"%{libs.GLFW}"
	}

	links {
		"glfw3",
		"opengl32.lib"
	}
	
	flags {}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines { "DEBUG"}
		symbols "On"
	
	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

-- Example project
--  Used for testing
project "stdgl_examples"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin/int/" .. outputdir .. "/%{prj.name}")
	location "%{prj.name}"

	files {"%{prj.name}/src/**.h", "%{prj.name}/src/**.cpp", lib_src}
	
	defines {
	}

	includedirs {
		"src",
		"%{prj.name}/src",
		"%{includes.GLFW}",
		"%{includes.Glad}",
		"%{includes.GLM}",
		"%{includes.spdlog}",
		"%{includes.imgui}",
		"%{includes.stb}"
	}
	
	libdirs {
		"bin/" .. outputdir .. "/stdgl",
		"%{libs.GLFW}"
	}

	links {
		"glfw3",
		"opengl32.lib",
		"stdgl.lib"
	}
	
	flags {}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines { "DEBUG"}
		symbols "On"
	
	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
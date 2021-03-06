
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
includes["assimp"] = "libs/assimp/include"

includes["implot"] = "stdgl_examples/libs/implot"
includes["imguizmo"] = "stdgl_examples/libs/ImGuizmo"

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

stdgl_src = {
	"src/stdgl.h",
	"src/stdgl.cpp"
}

implot_src = {
	"stdgl_examples/libs/implot/implot.cpp",
	"stdgl_examples/libs/implot/implot_demo.cpp",
	"stdgl_examples/libs/implot/implot_items.cpp"
}

imguizmo_src = {
	"stdgl_examples/libs/ImGuizmo/ImGuizmo.cpp"
}

lib_src = { imgui_src, glad_src, stdgl_src, implot_src, imguizmo_src}

libs = {}
libs["GLFW"] = "libs/glfw/x64/release"
libs["assimp"] = "libs/assimp/x64"


-- Workspace and project setup
workspace "stdgl"
	configurations {"Debug", "Release"}
	platforms { "Win64" }

	startproject "stdgl_examples"


-- Example project
--  Used for testing
project "stdgl_examples"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin/int/" .. outputdir .. "/%{prj.name}")
	location "%{prj.name}"

	dependson {"stdgl"}

	files {"%{prj.name}/src/**.h", "%{prj.name}/src/**.cpp", lib_src}
	
	defines {
		"STDGL_LOG"
	}

	includedirs {
		"src",
		"%{prj.name}/src",
		"%{includes.GLFW}",
		"%{includes.Glad}",
		"%{includes.GLM}",
		"%{includes.spdlog}",
		"%{includes.imgui}",
		"%{includes.stb}",
		"%{includes.assimp}",
		"%{includes.implot}",
		"%{includes.imguizmo}"
	}
	
	libdirs {
		"%{libs.GLFW}",
		"%{libs.assimp}"
	}
	
	postbuildcommands {
		"{COPY} res/ %{cfg.targetdir}/res/"
	}

	links {
		"glfw3",
		"opengl32.lib",
	}
	
	flags {}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines { "DEBUG"}
		symbols "On"
		links {
			"assimp-vc142-mtd",
			"zlibstaticd"
		}
	
	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
		links {
			"assimp-vc142-mt",
			"zlibstatic"
		}


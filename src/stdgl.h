#pragma once

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

namespace stdgl {
	
	//---------------------------------------------------------------
	// [SECTION] Resource utilities
	//---------------------------------------------------------------

	std::string loadTextResource(const char* path);


	//---------------------------------------------------------------
	// [SECTION] Contexts / forward declarations
	//---------------------------------------------------------------

	

	//---------------------------------------------------------------
	// [SECTION] STDGL Context
	//---------------------------------------------------------------

	typedef unsigned int StdGLID;

	struct Context;

	StdGLID getID(const char* str, const char* str_end = nullptr);
	void pushID(const char* str);
	void pushID(StdGLID id);
	void popID();

	Context* createContext();
	void destroyContext(Context* context = nullptr);

	void setContext(Context* context);
	Context* getContext();


	//---------------------------------------------------------------
	// [SECTION] Setup
	//---------------------------------------------------------------

	bool setupGLFW();
	bool setupGLAD();
	bool setupDebug();


	//---------------------------------------------------------------
	// [SECTION] ImGui
	//---------------------------------------------------------------

	bool setupImGui(GLFWwindow* window, const char* glVersion = "#version 460");
	void imguiNewFrame();
	void renderImGui();
	void shutdownImGui();


	//---------------------------------------------------------------
	// [SECTION] Texture
	//---------------------------------------------------------------

	struct Texture {
		GLuint textureID;
		unsigned int width;
		unsigned int height;
		unsigned int channels;
		GLenum format;
		std::string sourcePath;
	};

	Texture loadTexture(const std::string& sourcePath);

	//---------------------------------------------------------------
	// [SECTION] Mesh
	//---------------------------------------------------------------

	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 textureCoordinate;
	};

	struct Mesh {
		std::vector<Vertex> vertices;

		GLuint vao; // Loaded vao
		GLuint vbo; // Loaded vbo
		GLenum mode; // Loaded mode
		GLsizei vertexCount; // Loaded vertex count
	};

	Mesh loadMesh(const std::vector<Vertex>& vertices, GLenum mode);


	//---------------------------------------------------------------
	// [SECTION] Camera utlities
	//---------------------------------------------------------------

	struct Camera {
		glm::mat4 transform;
		glm::mat4 projection;

		Camera() : transform(0), projection(0) {}
	};


	//---------------------------------------------------------------
	// [SECTION] Shader
	//---------------------------------------------------------------

	//// Creation functions
	bool beginShader(const char* name);
	void endShader();

	void shaderUseVertexFile(const std::string& path);
	void shaderUseFragmentFile(const std::string& path);

	void shaderBindAttribute(unsigned int index, const std::string& name);

	//// Use functions
	bool useShader(const char* name);
	void stopShader();

	void shaderLoadInt(const char* name, int value);
	void shaderLoadFloat(const char* name, float value);
	void shaderLoadVec2(const char* name, glm::vec2 value);
	void shaderLoadVec3(const char* name, glm::vec3 value);
	void shaderLoadVec4(const char* name, glm::vec4 value);
	void shaderLoadMat4(const char* name, glm::mat4 value);


	//---------------------------------------------------------------
	// [SECTION] Renderer
	//---------------------------------------------------------------

	struct RenderContext {
		Camera camera;

		RenderContext() : camera() {}
	};

	void beginRender();		// Begins a new draw command
	void endRender();		// Ends an active draw command

	void useCamera(const Camera& camera); // TODO: Rework - point to data instead of copy

	void drawMesh(const Mesh& mesh);

	// TODO: Implement.
	void drawQuad(glm::mat4 transform);  // Adds a draw quad action to the active draw command
	
	
	//---------------------------------------------------------------
	// [SECTION] Context definition
	//---------------------------------------------------------------

	struct Context {
		RenderContext renderContext;
		std::vector<StdGLID> idStack;

		StdGLID getID(const char* str, const char* str_end = nullptr);

		Context() : renderContext(), idStack() {}
	};


	//---------------------------------------------------------------
	// [SECTION] Utilities
	//---------------------------------------------------------------

	struct Timestep {
		const float deltaTime;
		const float timeSinceStart;
		const float fps;
	};

	// Marks a new frame
	Timestep newFrame();
	int getFPS();
	
}

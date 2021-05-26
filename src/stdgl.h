#pragma once

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <string>
#include <vector>

#include <optional>

#include <memory>

namespace stdgl {
	
	//---------------------------------------------------------------
	// [SECTION] Data stuctures
	//---------------------------------------------------------------

	struct Vec2 {
		int x, y;
	};


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
	bool setupOpenGL();
	bool setupSTB();

	bool setupInput(GLFWwindow* window, bool installFramebufferCallback = true);


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

	enum class TextureType {
		DIFFUSE,
		SPECULAR,
		AO,
		NORMAL
	};

	struct Texture {
		GLuint textureID;
		TextureType type;

		unsigned int width;
		unsigned int height;
		unsigned int channels;
		GLenum format;
		
		std::string sourcePath;
		/*
		Texture() = default;
		Texture(GLuint textureID, TextureType type, unsigned int width, unsigned int height,unsigned int channels, GLenum format, const std::string& sourcePath)
			: textureID(textureID), type(type), width(width), height(height), channels(channels), format(format), sourcePath(sourcePath) {}
			*/
	};

	Texture loadTexture(const std::string& sourcePath, const TextureType& type = TextureType::DIFFUSE);

	//---------------------------------------------------------------
	// [SECTION] Mesh
	//---------------------------------------------------------------

	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 textureCoordinate;
	};

	enum class MeshType {
		ArrayMesh, // No indices
		ElementMesh // Indices
	};

	struct Mesh {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		MeshType type;

		GLuint vao; // Loaded vao
		GLuint vbo; // Loaded vbo
		GLuint ebo; // Loaded ebo (ElementMesh)
		GLenum mode; // Loaded mode
		GLsizei vertexCount; // Loaded vertex count
		GLsizei indiceCount; // Loaded indice count
	};

	Mesh loadMesh(GLenum mode, const std::vector<Vertex>& vertices, const std::vector<Texture>& textures = {});
	Mesh loadMesh(GLenum mode, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures = {});


	//---------------------------------------------------------------
	// [SECTION] Model (a collection of meshes)
	//---------------------------------------------------------------

	struct Model {
		std::vector<Mesh> meshes;
		std::string sourcePath;

		/*
		Model() = default;
		Model(const std::vector<Mesh>& meshes, const std::string& sourcePath)
			: meshes(meshes), sourcePath(sourcePath) {}
		Model(stdgl::Model&) = default;
		*/
	};

	std::optional<Model> loadModel(const std::string& path);


	//---------------------------------------------------------------
	// [SECTION] Camera utlities
	//---------------------------------------------------------------

	struct Camera {
		glm::vec3 position;
		glm::vec3 rotation;
		glm::mat4 projection;

		float fov;
		float zNear, zFar;

		glm::mat4 getMatrix() const;

		glm::mat4 generateProjection() const;

		Camera(const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& rotation = glm::vec3(0.0f))
			: position(position), rotation(rotation), projection(1.0f), fov(glm::radians(80.0f)), zNear(0.01f), zFar(1000.0f) {}
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

	void shaderLoadCamera(const Camera& camera);


	//---------------------------------------------------------------
	// [SECTION] Framebuffer
	//---------------------------------------------------------------
	
	enum class FramebufferAttachmentType {
		COLOR,
		DEPTH,
		COLOR_RED_INT
	};

	bool beginFramebuffer(const char* name, int width = 0, int height = 0, bool extraAttachment = false);
	void buildFramebuffer();
	void endFramebuffer();
	GLuint addAttachment(FramebufferAttachmentType type, unsigned int attachmentIndex = 0);
	bool useFramebuffer(const char* name);
	void stopFramebuffer();

	GLuint getFramebufferID();
	GLuint getFramebufferTextureID(int attachmentIndex = 0, FramebufferAttachmentType type = FramebufferAttachmentType::COLOR);
	Vec2 getFramebufferSize();


	//---------------------------------------------------------------
	// [SECTION] Renderer
	//---------------------------------------------------------------

	struct RenderContext {
		std::shared_ptr<Camera> camera;

		int width, height;

		RenderContext() : camera(), width(0), height(0) {}
	};

	void beginRender(std::shared_ptr<Camera> camera);
	void endRender();

	void clearFramebuffer();

	void useCamera(std::shared_ptr<Camera> camera);
	std::shared_ptr<Camera> getCamera();

	void setRendererSize(int width, int height);

	void drawMesh(const Mesh& mesh, bool skipTextures = false);
	void drawModel(const Model& model, bool skipTextures = false);

	void bindTexture(const Texture& texture, int unit = 0, std::string name = "texture_diffuse");
	
	//---------------------------------------------------------------
	// [SECTION] Context definition
	//---------------------------------------------------------------

	struct Context {
		RenderContext renderContext;
		std::vector<StdGLID> idStack;

		StdGLID getID(const char* str, const char* str_end = nullptr);

		Context() : renderContext(), idStack({0}) {}
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

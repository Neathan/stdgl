#include "stdgl.h"


#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>

#include <spdlog/spdlog.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <map>
#include <cassert>
#include <fstream>

namespace stdgl {

	//---------------------------------------------------------------
	// [SECTION] Logging
	//---------------------------------------------------------------

	#ifdef STDGL_LOG
		#define STDGL_LOG_TRACE_F(fmt, ...)	spdlog::trace(fmt, __VA_ARGS__)
		#define STDGL_LOG_TRACE(msg)			spdlog::trace(msg)
		#define STDGL_LOG_DEBUG_F(fmt, ...)	spdlog::debug(fmt, __VA_ARGS__)
		#define STDGL_LOG_DEBUG(msg)			spdlog::debug(msg)
		#define STDGL_LOG_ERROR_F(fmt, ...)	spdlog::error(fmt, __VA_ARGS__)
		#define STDGL_LOG_ERROR(msg)			spdlog::error(msg)
	#else
		#define STDGL_LOG_TRACE_F(fmt, ...)
		#define STDGL_LOG_TRACE(msg)
		#define STDGL_LOG_DEBUG_F(fmt, ...)
		#define STDGL_LOG_DEBUG(msg)
		#define STDGL_LOG_ERROR_F(fmt, ...)
		#define STDGL_LOG_ERROR(msg)
	#endif

	//---------------------------------------------------------------
	// [SECTION] Assertion
	//---------------------------------------------------------------

	#define STDGL_ASSERT(x) assert(x)


	//---------------------------------------------------------------
	// [SECTION] Hash functions
	//---------------------------------------------------------------

	// -- IMGUI IMPLEMENTATION -- source: imgui.cpp
	// CRC32 needs a 1KB lookup table (not cache friendly)
	// Although the code to generate the table is simple and shorter than the table itself, using a const table allows us to easily:
	// - avoid an unnecessary branch/memory tap, - keep the ImHashXXX functions usable by static constructors, - make it thread-safe.
	static const unsigned int g_crc32LookupTable[256] = {
		0x00000000,0x77073096,0xEE0E612C,0x990951BA,0x076DC419,0x706AF48F,0xE963A535,0x9E6495A3,0x0EDB8832,0x79DCB8A4,0xE0D5E91E,0x97D2D988,0x09B64C2B,0x7EB17CBD,0xE7B82D07,0x90BF1D91,
		0x1DB71064,0x6AB020F2,0xF3B97148,0x84BE41DE,0x1ADAD47D,0x6DDDE4EB,0xF4D4B551,0x83D385C7,0x136C9856,0x646BA8C0,0xFD62F97A,0x8A65C9EC,0x14015C4F,0x63066CD9,0xFA0F3D63,0x8D080DF5,
		0x3B6E20C8,0x4C69105E,0xD56041E4,0xA2677172,0x3C03E4D1,0x4B04D447,0xD20D85FD,0xA50AB56B,0x35B5A8FA,0x42B2986C,0xDBBBC9D6,0xACBCF940,0x32D86CE3,0x45DF5C75,0xDCD60DCF,0xABD13D59,
		0x26D930AC,0x51DE003A,0xC8D75180,0xBFD06116,0x21B4F4B5,0x56B3C423,0xCFBA9599,0xB8BDA50F,0x2802B89E,0x5F058808,0xC60CD9B2,0xB10BE924,0x2F6F7C87,0x58684C11,0xC1611DAB,0xB6662D3D,
		0x76DC4190,0x01DB7106,0x98D220BC,0xEFD5102A,0x71B18589,0x06B6B51F,0x9FBFE4A5,0xE8B8D433,0x7807C9A2,0x0F00F934,0x9609A88E,0xE10E9818,0x7F6A0DBB,0x086D3D2D,0x91646C97,0xE6635C01,
		0x6B6B51F4,0x1C6C6162,0x856530D8,0xF262004E,0x6C0695ED,0x1B01A57B,0x8208F4C1,0xF50FC457,0x65B0D9C6,0x12B7E950,0x8BBEB8EA,0xFCB9887C,0x62DD1DDF,0x15DA2D49,0x8CD37CF3,0xFBD44C65,
		0x4DB26158,0x3AB551CE,0xA3BC0074,0xD4BB30E2,0x4ADFA541,0x3DD895D7,0xA4D1C46D,0xD3D6F4FB,0x4369E96A,0x346ED9FC,0xAD678846,0xDA60B8D0,0x44042D73,0x33031DE5,0xAA0A4C5F,0xDD0D7CC9,
		0x5005713C,0x270241AA,0xBE0B1010,0xC90C2086,0x5768B525,0x206F85B3,0xB966D409,0xCE61E49F,0x5EDEF90E,0x29D9C998,0xB0D09822,0xC7D7A8B4,0x59B33D17,0x2EB40D81,0xB7BD5C3B,0xC0BA6CAD,
		0xEDB88320,0x9ABFB3B6,0x03B6E20C,0x74B1D29A,0xEAD54739,0x9DD277AF,0x04DB2615,0x73DC1683,0xE3630B12,0x94643B84,0x0D6D6A3E,0x7A6A5AA8,0xE40ECF0B,0x9309FF9D,0x0A00AE27,0x7D079EB1,
		0xF00F9344,0x8708A3D2,0x1E01F268,0x6906C2FE,0xF762575D,0x806567CB,0x196C3671,0x6E6B06E7,0xFED41B76,0x89D32BE0,0x10DA7A5A,0x67DD4ACC,0xF9B9DF6F,0x8EBEEFF9,0x17B7BE43,0x60B08ED5,
		0xD6D6A3E8,0xA1D1937E,0x38D8C2C4,0x4FDFF252,0xD1BB67F1,0xA6BC5767,0x3FB506DD,0x48B2364B,0xD80D2BDA,0xAF0A1B4C,0x36034AF6,0x41047A60,0xDF60EFC3,0xA867DF55,0x316E8EEF,0x4669BE79,
		0xCB61B38C,0xBC66831A,0x256FD2A0,0x5268E236,0xCC0C7795,0xBB0B4703,0x220216B9,0x5505262F,0xC5BA3BBE,0xB2BD0B28,0x2BB45A92,0x5CB36A04,0xC2D7FFA7,0xB5D0CF31,0x2CD99E8B,0x5BDEAE1D,
		0x9B64C2B0,0xEC63F226,0x756AA39C,0x026D930A,0x9C0906A9,0xEB0E363F,0x72076785,0x05005713,0x95BF4A82,0xE2B87A14,0x7BB12BAE,0x0CB61B38,0x92D28E9B,0xE5D5BE0D,0x7CDCEFB7,0x0BDBDF21,
		0x86D3D2D4,0xF1D4E242,0x68DDB3F8,0x1FDA836E,0x81BE16CD,0xF6B9265B,0x6FB077E1,0x18B74777,0x88085AE6,0xFF0F6A70,0x66063BCA,0x11010B5C,0x8F659EFF,0xF862AE69,0x616BFFD3,0x166CCF45,
		0xA00AE278,0xD70DD2EE,0x4E048354,0x3903B3C2,0xA7672661,0xD06016F7,0x4969474D,0x3E6E77DB,0xAED16A4A,0xD9D65ADC,0x40DF0B66,0x37D83BF0,0xA9BCAE53,0xDEBB9EC5,0x47B2CF7F,0x30B5FFE9,
		0xBDBDF21C,0xCABAC28A,0x53B39330,0x24B4A3A6,0xBAD03605,0xCDD70693,0x54DE5729,0x23D967BF,0xB3667A2E,0xC4614AB8,0x5D681B02,0x2A6F2B94,0xB40BBE37,0xC30C8EA1,0x5A05DF1B,0x2D02EF8D,
	};

	// -- IMGUI IMPLEMENTATION -- source: imgui.cpp
	// Zero-terminated string hash, with support for ### to reset back to seed value
	StdGLID hashStr(const char* data_p, size_t data_size, unsigned int seed) {
		seed = ~seed;
		unsigned int crc = seed;
		const unsigned char* data = (const unsigned char*)data_p;
		const unsigned int* crc32_lut = g_crc32LookupTable;
		if (data_size != 0) {
			while (data_size-- != 0) {
				unsigned char c = *data++;
				if (c == '#' && data_size >= 2 && data[0] == '#' && data[1] == '#')
					crc = seed;
				crc = (crc >> 8) ^ crc32_lut[(crc & 0xFF) ^ c];
			}
		}
		else {
			while (unsigned char c = *data++) {
				if (c == '#' && data[0] == '#' && data[1] == '#')
					crc = seed;
				crc = (crc >> 8) ^ crc32_lut[(crc & 0xFF) ^ c];
			}
		}
		return ~crc;
	}


	//---------------------------------------------------------------
	// [SECTION] Context definition
	//---------------------------------------------------------------

	StdGLID Context::getID(const char* str, const char* str_end) {
		StdGLID seed = idStack.back();
		return hashStr(str, str_end ? (str_end - str) : 0, seed);
	}


	//---------------------------------------------------------------
	// [SECTION] Resource utilities
	//---------------------------------------------------------------

	std::string loadTextResource(const char* path) {
		STDGL_LOG_DEBUG_F("Loading file: {}", path);
		std::string content;
		std::string line;

		std::ifstream fileStream(path, std::ios::in);
		STDGL_ASSERT(fileStream.is_open());

		if (fileStream.is_open()) {
			while (getline(fileStream, line)) {
				content.append(line).append("\n");
			}
			fileStream.close();
		}
		else {
			STDGL_LOG_ERROR_F("Failed to open: {}", path);
		}
		return content;
	}


	//---------------------------------------------------------------
	// [SECTION] Context
	//---------------------------------------------------------------

	#ifndef g_stdglContext
	static Context* g_stdglContext = nullptr;
	#endif

	// Internal
	void initialize(Context* context) {
		STDGL_LOG_DEBUG("Context initialize");
	}

	void shutdown(Context* context) {
		STDGL_LOG_DEBUG("Context shutdown");
	}

	StdGLID getIDWithSeed(unsigned int seed, const char* str, const char* str_end = nullptr) {
		return hashStr(str, str_end ? (str_end - str) : 0, seed);
	}

	// Implementation
	StdGLID getID(const char* str, const char* str_end) {
		return g_stdglContext->getID(str, str_end);
	}

	void pushID(const char* str) {
		StdGLID id = g_stdglContext->getID(str);
		g_stdglContext->idStack.push_back(id);
	}

	void pushID(StdGLID id) {
		g_stdglContext->idStack.push_back(id);
	}


	void popID() {
		STDGL_ASSERT(g_stdglContext->idStack.size() > 0);
		g_stdglContext->idStack.pop_back();
	}

	Context* createContext() {
		Context* newContext = new Context();
		if (g_stdglContext == nullptr) {
			g_stdglContext = newContext;
		}
		initialize(newContext);
		return newContext;
	}

	void destroyContext(Context* context) {
		if (context == nullptr) {
			context = g_stdglContext;
		}
		shutdown(context);
		if (g_stdglContext == context) {
			g_stdglContext = nullptr;
		}
		delete context;
	}

	void setContext(Context* context) {
		g_stdglContext = context;
	}

	Context* getContext() {
		return g_stdglContext;
	}


	//---------------------------------------------------------------
	// [SECTION] Setup
	//---------------------------------------------------------------

	// OpenGL debug output function
	void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam) {
		// ignore non-significant error/warning codes
		if (id == 131169 || id == 131185 || id == 131218 || id == 131204) {
			STDGL_LOG_TRACE("---------------");
			STDGL_LOG_TRACE_F("Trace message ({}): {}", id, message);
			return;
		}

		STDGL_LOG_DEBUG("---------------");
		STDGL_LOG_DEBUG_F("Debug message ({}): {}", id, message);

		switch (source) {
			case GL_DEBUG_SOURCE_API:             STDGL_LOG_DEBUG("Source: API"); break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   STDGL_LOG_DEBUG("Source: Window System"); break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER: STDGL_LOG_DEBUG("Source: Shader Compiler"); break;
			case GL_DEBUG_SOURCE_THIRD_PARTY:     STDGL_LOG_DEBUG("Source: Third Party"); break;
			case GL_DEBUG_SOURCE_APPLICATION:     STDGL_LOG_DEBUG("Source: Application"); break;
			case GL_DEBUG_SOURCE_OTHER:           STDGL_LOG_DEBUG("Source: Other"); break;
		}

		switch (type) {
			case GL_DEBUG_TYPE_ERROR:               STDGL_LOG_DEBUG("Type: Error"); break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: STDGL_LOG_DEBUG("Type: Deprecated Behaviour"); break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  STDGL_LOG_DEBUG("Type: Undefined Behaviour"); break;
			case GL_DEBUG_TYPE_PORTABILITY:         STDGL_LOG_DEBUG("Type: Portability"); break;
			case GL_DEBUG_TYPE_PERFORMANCE:         STDGL_LOG_DEBUG("Type: Performance"); break;
			case GL_DEBUG_TYPE_MARKER:              STDGL_LOG_DEBUG("Type: Marker"); break;
			case GL_DEBUG_TYPE_PUSH_GROUP:          STDGL_LOG_DEBUG("Type: Push Group"); break;
			case GL_DEBUG_TYPE_POP_GROUP:           STDGL_LOG_DEBUG("Type: Pop Group"); break;
			case GL_DEBUG_TYPE_OTHER:               STDGL_LOG_DEBUG("Type: Other"); break;
		}

		switch (severity) {
			case GL_DEBUG_SEVERITY_HIGH:         STDGL_LOG_DEBUG("Severity: high"); break;
			case GL_DEBUG_SEVERITY_MEDIUM:       STDGL_LOG_DEBUG("Severity: medium"); break;
			case GL_DEBUG_SEVERITY_LOW:          STDGL_LOG_DEBUG("Severity: low"); break;
			case GL_DEBUG_SEVERITY_NOTIFICATION: STDGL_LOG_DEBUG("Severity: notification"); break;
		}
	}

	bool setupGLFW() {
		return glfwInit();
	}

	bool setupGLAD() {
		return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	}

	bool setupDebug() {
		// Requires: glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
		int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(glDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
			return true;
		}
		return false;
	}


	//---------------------------------------------------------------
	// [SECTION] ImGui
	//---------------------------------------------------------------

	bool setupImGui(GLFWwindow* window, const char* glVersion) {
		bool success = true;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		success &= ImGui_ImplGlfw_InitForOpenGL(window, true);
		success &= ImGui_ImplOpenGL3_Init(glVersion);

		return success;
	}

	void imguiNewFrame() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void renderImGui() {
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void shutdownImGui() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}


	//---------------------------------------------------------------
	// [SECTION] Texture
	//---------------------------------------------------------------

	//Texture loadTexture(const std::string& sourcePath) {
	//	return {}
	//}


	//---------------------------------------------------------------
	// [SECTION] Mesh
	//---------------------------------------------------------------

	static std::vector<GLuint> g_loadedVAOS;
	static std::vector<GLuint> g_loadedVBOS;

	GLuint createVAO() {
		GLuint vao;
		glGenVertexArrays(1, &vao);
		g_loadedVAOS.push_back(vao);
		return vao;
	}

	GLuint createVBO() {
		GLuint vbo;
		glGenBuffers(1, &vbo);
		g_loadedVBOS.push_back(vbo);
		return vbo;
	}

	Mesh loadMesh(const std::vector<Vertex>& vertices, GLenum mode) {
		GLuint vao = createVAO();
		GLuint vbo = createVBO();

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // Position
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal)); // Normal
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoordinate)); // Texture coordinate

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		return { vertices, vao, vbo, mode, (GLsizei)vertices.size() };
	}


	//---------------------------------------------------------------
	// [SECTION] Camera utlities
	//---------------------------------------------------------------


	//---------------------------------------------------------------
	// [SECTION] Shader
	//---------------------------------------------------------------

	// Internal
	struct ShaderData {
		bool initialized;

		GLuint programID;

		std::map<unsigned int, std::string> attributes;

		// TODO: convert to sources
		std::string vertexFilePath;
		std::string fragmentFilePath;
		
		ShaderData() : initialized(false), programID(0), vertexFilePath(""), fragmentFilePath("") {}
	};

	typedef std::map<StdGLID, ShaderData> ShaderDataMap;
	static ShaderDataMap g_shaderDataMap;
	static int g_shaderSeed = 0;


	GLuint compileShader(const char* shaderSource, GLenum shaderType) {
		GLuint shader = glCreateShader(shaderType);

		GLint compileResult = GL_FALSE;
		GLint infoLoglength;

		STDGL_LOG_TRACE_F("Compiling shader: {}", shaderSource);

		const char* shaderSourcePointer = shaderSource;
		glShaderSource(shader, 1, &shaderSourcePointer, NULL);
		glCompileShader(shader);

		glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult);
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLoglength);

		if (infoLoglength > 0) {
			std::vector<char> shaderErrorMessage((size_t)infoLoglength + 1);
			glGetShaderInfoLog(shader, infoLoglength, NULL, &shaderErrorMessage[0]);
			if (compileResult != GL_TRUE) {
				STDGL_LOG_ERROR_F("Shader Compilation Error: {}", &shaderErrorMessage[0]);
				return -1;
			}
		}
		return shader;
	}


	// Implementation
	//// Creation functions
	bool beginShader(const char* name) {
		StdGLID id = getIDWithSeed(g_shaderSeed, name);
		ShaderData& shaderData = g_shaderDataMap[id];
		if (!shaderData.initialized) {
			pushID(id);
			return true;
		}
		return false;
	}

	void endShader() {
		// TODO: Add shader building code (shader compilation and program linkage)
		StdGLID id = getID("");
		ShaderData& shaderData = g_shaderDataMap[id];

		// Check if shader was given enough information to compile
		STDGL_ASSERT(!shaderData.fragmentFilePath.empty());
		STDGL_ASSERT(!shaderData.vertexFilePath.empty());
		
		// Load source files
		std::string vertexShaderSource = loadTextResource(shaderData.vertexFilePath.c_str());
		std::string fragmentShaderSource = loadTextResource(shaderData.fragmentFilePath.c_str());


		// Compile source
		GLuint vertexShaderID = compileShader(vertexShaderSource.c_str(), GL_VERTEX_SHADER);
		GLuint fragmentShaderID = compileShader(fragmentShaderSource.c_str(), GL_FRAGMENT_SHADER);

		// Create program, attach shaders and link them
		shaderData.programID = glCreateProgram();
		glAttachShader(shaderData.programID, vertexShaderID);
		glAttachShader(shaderData.programID, fragmentShaderID);

		// Bind attributes
		for (auto const& [key, value] : shaderData.attributes) {
			glBindAttribLocation(shaderData.programID, key, value.c_str());
		}

		// Link program and check for errors
		glLinkProgram(shaderData.programID);

		GLint linkResult;
		GLint infoLogLength;

		glGetProgramiv(shaderData.programID, GL_LINK_STATUS, &linkResult);
		glGetProgramiv(shaderData.programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0) {
			std::vector<char> programErrorMessage((size_t)infoLogLength + 1);
			glGetProgramInfoLog(shaderData.programID, infoLogLength, nullptr, &programErrorMessage[0]);
			if (linkResult != GL_TRUE) {
				STDGL_LOG_ERROR_F("Shader link error:\n{}", &programErrorMessage[0]);
				glDeleteProgram(shaderData.programID);
			}
		}

		// Validate and clean
		glValidateProgram(shaderData.programID);

		glDetachShader(shaderData.programID, vertexShaderID);
		glDetachShader(shaderData.programID, fragmentShaderID);

		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);


		STDGL_LOG_DEBUG("Shader program created");

		shaderData.initialized = true;
		popID();
	}

	void shaderUseVertexFile(const std::string& path) {
		StdGLID id = getID("");
		ShaderData& shaderData = g_shaderDataMap[id];
		shaderData.vertexFilePath = path;
	}

	void shaderUseFragmentFile(const std::string& path) {
		StdGLID id = getID("");
		ShaderData& shaderData = g_shaderDataMap[id];
		shaderData.fragmentFilePath = path;
	}

	void shaderBindAttribute(unsigned int index, const std::string& name) {
		StdGLID id = getID("");
		ShaderData& shaderData = g_shaderDataMap[id];
		
		shaderData.attributes[index] = name;
	}

	//// Use functions
	bool useShader(const char* name) {
		StdGLID id = getIDWithSeed(g_shaderSeed, name);
		pushID(id);

		ShaderData& shaderData = g_shaderDataMap[id];

		glUseProgram(shaderData.programID);

		return true; // TODO: implement
	}

	void stopShader() {
		glUseProgram(0);
		popID();
	}

	void shaderLoadInt(const char* name, int value) {
		StdGLID id = getID("");
		ShaderData& shaderData = g_shaderDataMap[id];
		glUniform1i(glGetUniformLocation(shaderData.programID, name), value);
	}

	void shaderLoadFloat(const char* name, float value) {
		StdGLID id = getID("");
		ShaderData& shaderData = g_shaderDataMap[id];
		glUniform1f(glGetUniformLocation(shaderData.programID, name), value);
	}

	void shaderLoadVec2(const char* name, glm::vec2 value) {
		StdGLID id = getID("");
		ShaderData& shaderData = g_shaderDataMap[id];
		glUniform2f(glGetUniformLocation(shaderData.programID, name), value.x, value.y);
	}

	void shaderLoadVec3(const char* name, glm::vec3 value) {
		StdGLID id = getID("");
		ShaderData& shaderData = g_shaderDataMap[id];
		glUniform3f(glGetUniformLocation(shaderData.programID, name), value.x, value.y, value.z);
	}

	void shaderLoadVec4(const char* name, glm::vec4 value) {
		StdGLID id = getID("");
		ShaderData& shaderData = g_shaderDataMap[id];
		glUniform4f(glGetUniformLocation(shaderData.programID, name), value.x, value.y, value.z, value.w);
	}

	void shaderLoadMat4(const char* name, glm::mat4 value) {
		StdGLID id = getID("");
		ShaderData& shaderData = g_shaderDataMap[id];
		glUniformMatrix4fv(glGetUniformLocation(shaderData.programID, name), 1, GL_FALSE, glm::value_ptr(value));
	}



	//---------------------------------------------------------------
	// [SECTION] Renderer
	//---------------------------------------------------------------


	void beginRender() {
		// Bind the renderer
		RenderContext& renderContext = g_stdglContext->renderContext;
		glClear(GL_COLOR_BUFFER_BIT);

	}

	void endRender() {
		// Unbind the renderer
	}

	void useCamera(const Camera& camera) {
		g_stdglContext->renderContext.camera = camera;
	}

	void drawMesh(const Mesh& mesh) {
		glBindVertexArray(mesh.vao);
		glDrawArrays(mesh.mode, 0, mesh.vertexCount);
		glBindVertexArray(0);
	}


	//---------------------------------------------------------------
	// [SECTION] Utilities
	//---------------------------------------------------------------

	struct UtilityData {
		float lastFrameTime; // Delta timer

		float fps; // Last segements fps
		float fpsCounter; // Current segments fps
		float fpsTime; // Time in current segement

		UtilityData() : lastFrameTime(), fps(), fpsCounter(), fpsTime() {}
	};

	static UtilityData g_utilityData;

	Timestep newFrame() {
		float currentTime = glfwGetTime();
		float delta = currentTime - g_utilityData.lastFrameTime;
		g_utilityData.lastFrameTime = currentTime;

		++g_utilityData.fpsCounter;

		g_utilityData.fpsTime += delta;
		if (g_utilityData.fpsTime >= 1.0f) {
			g_utilityData.fps = g_utilityData.fpsCounter;

			g_utilityData.fpsTime -= 1.0f;
			g_utilityData.fpsCounter = 0;
		}
		
		return { delta, currentTime, g_utilityData.fps };
	}

	int getFPS() {
		return g_utilityData.fps;
	}


}

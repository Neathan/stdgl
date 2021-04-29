
#include <stdgl.h>

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

#include <implot.h>
#include <ImGuizmo.h>

const int WIDTH = 1920;
const int HEIGHT = 1080;

static std::vector<stdgl::Vertex> g_triangleData = {
	stdgl::Vertex{ glm::vec3( 0.0f, 1.0f, 0.0f), glm::vec3( 0.0f, 0.0f, 1.0f), glm::vec2(0.5f, 1.0f) },
	stdgl::Vertex{ glm::vec3(-1.0f,-1.0f, 0.0f), glm::vec3( 0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f) },
	stdgl::Vertex{ glm::vec3( 1.0f,-1.0f, 0.0f), glm::vec3( 0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f) },
};

struct ExampleData {

	std::unique_ptr<stdgl::Mesh> triangleMesh;
	std::unique_ptr<stdgl::Model> testModel;

	const GLubyte* vendor;
	const GLubyte* renderer;
	const GLubyte* version;

	std::shared_ptr<stdgl::Camera> camera;

	/*
	float deltaData[1000] = {0};
	int currentDeltaDataPoint = 0;
	float maxDeltaValue = 0;
	bool shouldFitDelta = false;
	*/
};

static ExampleData g_exampleData;


void init() {
	g_exampleData.triangleMesh = std::make_unique<stdgl::Mesh>(stdgl::loadMesh(GL_TRIANGLES, g_triangleData));

	g_exampleData.vendor = glGetString(GL_VENDOR);
	g_exampleData.renderer = glGetString(GL_RENDERER);
	g_exampleData.version = glGetString(GL_VERSION);

	g_exampleData.camera = std::make_shared<stdgl::Camera>();
	stdgl::useCamera(g_exampleData.camera);

	auto model = stdgl::loadModel("res/DamagedHelmet.glb");
	if(model) {
		g_exampleData.testModel = std::make_unique<stdgl::Model>(model.value());
	}

	g_exampleData.camera->transform = glm::translate(glm::mat4(1), glm::vec3(0, 0, -3));


}

void renderUI(const stdgl::Timestep& ts) {
	ImGui::Begin("Renderer");

	ImGui::Text("Vendor: %s", g_exampleData.vendor);
	ImGui::Text("Renderer: %s", g_exampleData.renderer);
	ImGui::Text("Version: %s", g_exampleData.version);
	ImGui::Separator();

	ImGui::Text("fps: %i", stdgl::getFPS());
	ImGui::Text("avg ms: %.2f", 1000.0f / stdgl::getFPS());
	ImGui::Text("ms: %.2f", ts.deltaTime * 1000.0f);
	/*
	if (g_exampleData.shouldFitDelta) {
		ImPlot::SetNextPlotLimitsY(0, g_exampleData.maxDeltaValue * 1.1, ImGuiCond_Always);
		g_exampleData.shouldFitDelta = false;
	}
	if (ImPlot::BeginPlot("Delta times", "Time (s)", "Delta time (s)")) {
		ImPlot::PlotLine<float>("Delta", g_exampleData.deltaData, 1000);
		ImPlot::EndPlot();
	}
	*/
	ImGui::End();

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(glm::value_ptr(g_exampleData.camera->transform),
		glm::value_ptr(g_exampleData.camera->projection),
		ImGuizmo::OPERATION::TRANSLATE,
		ImGuizmo::MODE::LOCAL,
		glm::value_ptr(g_exampleData.camera->transform));
}

void render() {

	if (stdgl::beginShader("basicShader")) {

		stdgl::shaderUseVertexFile("res/test_v.glsl");
		stdgl::shaderUseFragmentFile("res/test_f.glsl");

		stdgl::shaderBindAttribute(0, "in_position");
		stdgl::shaderBindAttribute(1, "in_normal");
		stdgl::shaderBindAttribute(2, "in_textureCoord");

		stdgl::endShader();
	}

	stdgl::beginRender();
	{
		stdgl::useShader("basicShader");
		{

			stdgl::drawModel(*g_exampleData.testModel);
		}
		stdgl::stopShader();
	}
	stdgl::endRender();

}

void update(const stdgl::Timestep& ts) {
	/*
	g_exampleData.currentDeltaDataPoint++;
	g_exampleData.currentDeltaDataPoint %= 1000;
	g_exampleData.deltaData[g_exampleData.currentDeltaDataPoint] = ts.deltaTime;

	if (ts.deltaTime > g_exampleData.maxDeltaValue || g_exampleData.currentDeltaDataPoint) {
		g_exampleData.maxDeltaValue = ts.deltaTime;
		g_exampleData.shouldFitDelta = true;
	}
	*/
}

int main() {
	// Set program output level
	spdlog::set_level(spdlog::level::trace);

	if (!stdgl::setupGLFW()) {
		spdlog::error("Failed to initialize GLFW");
		return -1;
	}

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "stdgl_example", nullptr, nullptr);
	
	glfwMakeContextCurrent(window);

	// glfwSwapInterval(0);

	if(!stdgl::setupGLAD()) {
		spdlog::error("Failed to initialize GLAD");
		return -1;
	}

	if (!stdgl::setupDebug()) {
		spdlog::error("Failed to initialize debug");
		return -1;
	}

	if (!stdgl::setupOpenGL()) {
		spdlog::error("Failed to setup OpenGL options");
		return -1;
	}

	if (!stdgl::setupSTB()) {
		spdlog::error("Failed to initialize STB");
		return -1;
	}

	stdgl::Context* context = stdgl::createContext();
	
	init();

	if (!stdgl::setupInput(window)) {
		spdlog::error("Failed to setup inputs");
		return -1;
	}

	if (!stdgl::setupImGui(window)) {
		spdlog::error("Failed to initialize ImGui");
		return -1;
	}
	ImPlot::CreateContext();


	while (!glfwWindowShouldClose(window)) {
		stdgl::Timestep ts = stdgl::newFrame();

		glfwPollEvents();

		update(ts);

		render();

		stdgl::imguiNewFrame();
		ImGuizmo::BeginFrame();
		renderUI(ts);
		stdgl::renderImGui();

		glfwSwapBuffers(window);
	}

	ImPlot::DestroyContext();

	stdgl::shutdownImGui();
	stdgl::destroyContext();
	glfwTerminate();
	return 0;
}
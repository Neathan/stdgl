
#include <stdgl.h>

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <implot.h>


const int WIDTH = 1920;
const int HEIGHT = 1080;

static std::vector<stdgl::Vertex> g_triangleData = {
	stdgl::Vertex{ glm::vec3( 0.0f, 1.0f, 0.0f), glm::vec3( 0.0f, 0.0f, 1.0f), glm::vec2(0.5f, 1.0f) },
	stdgl::Vertex{ glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3( 0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f) },
	stdgl::Vertex{ glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec3( 0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f) },
};

struct ExampleData {

	std::unique_ptr<stdgl::Mesh> triangleMesh;

	float deltaData[1000] = {0};
	int currentDeltaDataPoint = 0;
	float maxDeltaValue = 0;
	bool shouldFitDelta = false;


	const GLubyte* vendor;
	const GLubyte* renderer;
	const GLubyte* version;
};

static ExampleData g_exampleData;


void init() {
	g_exampleData.triangleMesh = std::make_unique<stdgl::Mesh>(stdgl::loadMesh(g_triangleData, GL_TRIANGLES));

	g_exampleData.vendor = glGetString(GL_VENDOR);
	g_exampleData.renderer = glGetString(GL_RENDERER);
	g_exampleData.version = glGetString(GL_VERSION);
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
}

void render() {

	if (stdgl::beginShader("basicShader")) {

		stdgl::shaderUseVertexFile("res/test_v.glsl");
		stdgl::shaderUseFragmentFile("res/test_f.glsl");

		stdgl::shaderBindAttribute(0, "in_position");

		stdgl::endShader();
	}

	stdgl::beginRender();
	{
		stdgl::useShader("basicShader");
		{
			stdgl::drawMesh(*g_exampleData.triangleMesh);
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
	spdlog::set_level(spdlog::level::debug);

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
	}

	stdgl::Context* context = stdgl::createContext();

	if (!stdgl::setupImGui(window)) {
		spdlog::error("Failed to initialize ImGui");
		return -1;
	}
	ImPlot::CreateContext();

	init();

	while (!glfwWindowShouldClose(window)) {
		stdgl::Timestep ts = stdgl::newFrame();

		glfwPollEvents();

		update(ts);

		render();

		stdgl::imguiNewFrame();
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
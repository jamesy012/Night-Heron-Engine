#include "GFXOpenGL.h"

#include "Window.h"
#include <GL/glew.h>
#include <GL/wglew.h>

#include <imgui-master\imgui.h>
#include <imgui-master\examples\imgui_impl_opengl3.h>

#include "ShaderGL.h"
#include "MeshGL.h"
#include "TextureGL.h"



static void openGLMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

bool GFXOpenGL::CreateWindowSetUpAPI() {
	m_CurrentGraphics = this;

	m_Window = new Window();
	m_Window->m_WindowTitle += " - OpenGL";
	return m_Window->CreateMainWindow() && InitGfx();
}

void GFXOpenGL::DestroyMainWindow() {

	//if (ourWindow->fullscreen) {
//	ChangeDisplaySettings(NULL, 0);
//	ShowCursor(TRUE);
//}
	if (m_Window->m_HRC) {
		if (!wglMakeCurrent(NULL, NULL)) { //make context null
			MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		if (!wglDeleteContext(m_Window->m_HRC)) { //delete context
			MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		m_Window->m_HRC = NULL;
	}

	m_Window->DestrowMainWindow();
}

void GFXOpenGL::ImGuiDraw() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GFXOpenGL::ImGuiNewFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	m_Window->ImGuiNewFrame();
	ImGui::NewFrame();
}

void GFXOpenGL::InitImGui_Internal() {
	ImGui_ImplOpenGL3_Init("#version 450");
}

void GFXOpenGL::SwapBuffer() {
	SwapBuffers(m_Window->m_HDC);
}

void GFXOpenGL::Clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GFXOpenGL::SetClearColor(float R, float G, float B, float A) {
	GFX::SetClearColor(R, G, B, A);
	glClearColor(R, G, B, A);
}

void GFXOpenGL::ResizeWindow_Internal(int a_Width, int a_Height) {
	glViewport(0, 0, a_Width, a_Height);
}

Shader * GFXOpenGL::CreateShader() {
	return new ShaderGL();
}

Mesh * GFXOpenGL::CreateMesh() {
	return new MeshGL();
}

Texture * GFXOpenGL::CreateTexture() {
	return new TextureGL();
}

ShaderUniformBlock* GFXOpenGL::CreateBuffer(void * a_Object, unsigned int a_Size) {

	ShaderUniformBlockGL* sub = new ShaderUniformBlockGL();
	sub->m_Size = a_Size;
	glGenBuffers(1, &sub->m_ID);

	glBindBuffer(GL_UNIFORM_BUFFER, sub->m_ID);
	glBufferData(GL_UNIFORM_BUFFER, a_Size, a_Object, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	static int counter = 0;
	sub->m_SlotID = counter++;

	glBindBufferBase(GL_UNIFORM_BUFFER, sub->m_SlotID, sub->m_ID);

	return sub;
}

bool GFXOpenGL::InitGfx() {

	m_Window->m_HRC = wglCreateContext(m_Window->m_HDC);
	if (!(m_Window->m_HRC)) {
		//DestroyOpenGLWindow();                         // Reset The Display
		MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!wglMakeCurrent(m_Window->m_HDC, m_Window->m_HRC))                        // Try To Activate The Rendering Context
	{
		//DestroyOpenGLWindow();                         // Reset The Display
		MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Return FALSE
	}

	GLenum err = glewInit();
	if (err != GLEW_OK) {
		return FALSE;
	}

	//wglSwapIntervalEXT(1);

	//glViewport(-1, -1, m_Window->m_WindowWidth/2, m_Window->m_WindowHeight);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback((GLDEBUGPROC)openGLMessageCallback, 0);
	glDebugMessageControl(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PUSH_GROUP, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
	glDebugMessageControl(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_POP_GROUP, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
	//131185 - Buffer detailed info : Buffer object _ID_ (bound to GL_ARRAY_BUFFER_ARB, usage hint is GL_STATIC_DRAW) will use VIDEO memory as the source for buffer object operations.
	const GLsizei numOfIds = 1;
	GLuint ids[numOfIds] = { 131185 };
	glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, numOfIds, ids, false);
	GLenum error = glGetError();

	{
		int glMajor, glMinor;
		const  GLubyte* glVersion;
		const  GLubyte* glVender;
		const  GLubyte* glRenderer;
		glGetIntegerv(GL_MAJOR_VERSION, &glMajor);
		glGetIntegerv(GL_MINOR_VERSION, &glMinor);
		glVersion = glGetString(GL_VERSION);
		glVender = glGetString(GL_VENDOR);
		glRenderer = glGetString(GL_RENDERER);
		printf("OpenGL - %s\n(%i.%i) Vender: %s, Renderer: %s\n\n", glVersion, glMajor, glMinor, glVender, glRenderer);
	}


	glEnable(GL_DEPTH_TEST);

	return true;
}


void openGLMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, const void * userParam) {
	switch (id) {
		case 131218: // NVIDIA: "shader will be recompiled due to GL state mismatches"
			return;
		default:
			break;
	}

	std::string errorMessage;
	switch (source) {
		case GL_DEBUG_SOURCE_API:
			errorMessage += "DEBUG_SOURCE_API/";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			errorMessage += "DEBUG_SOURCE_WINDOW_SYSTEM/";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			errorMessage += "DEBUG_SOURCE_SHADER_COMPILER/";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			errorMessage += "DEBUG_SOURCE_THIRD_PARTY/";
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			errorMessage += "DEBUG_SOURCE_APPLICATION/";
			break;
		case GL_DEBUG_SOURCE_OTHER:
			errorMessage += "DEBUG_SOURCE_OTHER/";
			break;
	}
	switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			errorMessage += "DEBUG_TYPE_ERROR/";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			errorMessage += "DEBUG_TYPE_DEPRECATED_BEHAVIOR/";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			errorMessage += "DEBUG_TYPE_UNDEFINED_BEHAVIOR/";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			errorMessage += "DEBUG_TYPE_PORTABILITY/";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			errorMessage += "DEBUG_TYPE_PERFORMANCE/";
			break;
		case GL_DEBUG_TYPE_MARKER:
			errorMessage += "DEBUG_TYPE_MARKER/";
			break;
		case GL_DEBUG_TYPE_PUSH_GROUP:
			errorMessage += "DEBUG_TYPE_PUSH_GROUP/";
			break;
		case GL_DEBUG_TYPE_POP_GROUP:
			errorMessage += "DEBUG_TYPE_POP_GROUP/";
			break;
		case GL_DEBUG_TYPE_OTHER:
			errorMessage += "DEBUG_TYPE_OTHER/";
			break;
	}
	switch (severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			errorMessage += "DEBUG_SEVERITY_NOTIFICATION/";
			break;
		case GL_DEBUG_SEVERITY_LOW:
			errorMessage += "DEBUG_SEVERITY_MEDIUM/";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			errorMessage += "DEBUG_SEVERITY_LOW/";
			break;
		case GL_DEBUG_SEVERITY_HIGH:
			errorMessage += "DEBUG_SEVERITY_HIGH/";
			break;
	}
	printf("GL_ERROR: %X(%i), %s\n\t%s\n", id, id, errorMessage.c_str(), message);
	//printf("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
	//	(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
	//	   type, severity, message);
}


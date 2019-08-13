#include "TestClearColour.h"

#include "Renderer.h"
#include "imgui/imgui.h"

namespace test
{
	ClearColour::ClearColour()
		: m_ClearColour{ 0.2f, 0.3f, 0.8f, 1.0f }
	{
	}

	ClearColour::~ClearColour()
	{
	}

	void ClearColour::OnUpdate(float deltaTime)
	{
	}

	void ClearColour::OnRender()
	{
		GLCall(glClearColor(m_ClearColour[0], m_ClearColour[1], m_ClearColour[2], m_ClearColour[3]));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));
	}

	void ClearColour::OnImGuiRender()
	{
		ImGui::ColorEdit4("Clear Colour", m_ClearColour);
	}
}
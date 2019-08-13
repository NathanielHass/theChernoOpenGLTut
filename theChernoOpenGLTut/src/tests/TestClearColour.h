#pragma once

#include "Test.h"

namespace test
{
	class ClearColour : public Test
	{
	private:
		float m_ClearColour[4];
	public:
		ClearColour();
		~ClearColour();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	};
}
#pragma once

#include "Walnut/Application.h"
#include "Walnut/Layer.h"
#include "Walnut/Networking/Client.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

#include "Renderer/Renderer.h"

namespace Cubed {

	class ClientLayer : public Walnut::Layer
	{
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(float ts) override;
		virtual void OnRender() override;
		virtual void OnUIRender() override;

	private:
		void OnDataRecieved(const Walnut::Buffer buffer);

	private:
		Renderer m_Renderer;

		glm::vec3 m_PlayerPosition{ 0, 0, -8 };
		glm::vec3 m_PlayerRotation{ 30, 45, 0 };
		glm::vec3 m_PlayerVelocity{ 0, 0, 0 };

		Camera m_Camera;

		Walnut::Client m_Client;
		std::string m_ServerAddress;
		uint32_t m_PlayerID;

		struct PlayerData
		{
			glm::vec3 Position;
			glm::vec3 Velocity;
		};

		std::mutex m_PlayerDataMutex;
		std::map<uint32_t, PlayerData> m_PlayerData;
	};

}

#pragma once

#include "Walnut/Application.h"
#include "Walnut/Layer.h"
#include "Walnut/Networking/Client.h"

#include <glm/glm.hpp>
#include <string>

namespace Cubed {

	class ClientLayer : public Walnut::Layer
	{
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(float ts);
		virtual void OnRender();
		virtual void OnUIRender();

	private:
		void OnDataRecieved(const Walnut::Buffer buffer);

	private:
		glm::vec2 m_Position{50, 50};
		glm::vec2 m_Velocity{};

		Walnut::Client m_Client;
		std::string m_ServerAddress;
		uint32_t m_PlayerID;

		struct PlayerData
		{
			glm::vec2 Position;
			glm::vec2 Velocity;
		};

		std::mutex m_PlayerDataMutex;
		std::map<uint32_t, PlayerData> m_PlayerData;
	};

}

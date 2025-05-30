#pragma once

#include <Walnut/Layer.h>
#include <Walnut/Networking/Server.h>
#include "HeadlessConsole.h"

#include <string_view>
#include <map>

#include <glm/glm.hpp>

namespace Cubed
{

	class ServerLayer : public Walnut::Layer
	{
	public:
		virtual void OnAttach();
		virtual void OnDetach();

		virtual void OnUpdate(float ts);
		virtual void OnUIRender();

	private:
		void OnConsoleSend(std::string_view message);

		void OnClientConnected(const Walnut::ClientInfo& client);
		void OnClientDisconnected(const Walnut::ClientInfo& client);
		void OnDataRecieved(const Walnut::ClientInfo& client, const Walnut::Buffer buffer);

	private: 
		HeadlessConsole m_Console;
		Walnut::Server m_Server{ 8192 };

		struct PlayerData
		{
			glm::vec3 Position;
			glm::vec3 Velocity;
		};

		std::mutex m_PlayerDataMutex;
		std::map<uint32_t, PlayerData> m_PlayerData;

	};


}

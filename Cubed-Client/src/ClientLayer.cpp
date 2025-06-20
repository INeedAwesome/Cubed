#include "ClientLayer.h"

#include <Walnut/Input/Input.h>
#include <Walnut/UI/UI.h>
#include <Walnut/ImGui/ImGuiTheme.h>
#include <Walnut/Serialization/BufferStream.h>

#include "ServerPacket.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"

#include "stdint.h"

static void DrawRect(glm::vec2 pos, glm::vec2 size, uint32_t color)
{
	ImDrawList* drawList = ImGui::GetBackgroundDrawList();

	ImVec2 min = ImGui::GetWindowPos() + ImVec2(pos.x, pos.y);
	ImVec2 max = min + ImVec2(size.x, size.y);

	drawList->AddRectFilled(min, max, color);
}

namespace Cubed 
{
	static Walnut::Buffer s_ScratchBuffer;

	void ClientLayer::OnAttach()
	{
		m_Client.SetDataReceivedCallback([this](const Walnut::Buffer buffer) 
			{ OnDataRecieved(buffer); }
		);

		s_ScratchBuffer.Allocate(10 * 1024 * 1024); // 10 MB

		m_Renderer.Init();
	}

	void ClientLayer::OnDetach()
	{
		m_Client.Disconnect();


		m_Renderer.Shutdown();
	}

	void ClientLayer::OnUpdate(float ts)
	{
		Walnut::Client::ConnectionStatus connectionStatus = m_Client.GetConnectionStatus();
		
		glm::vec3 dir{};
		if (Walnut::Input::IsKeyDown(Walnut::KeyCode::A))
			dir.x = -1;
		if (Walnut::Input::IsKeyDown(Walnut::KeyCode::D))
			dir.x = 1;
		if (Walnut::Input::IsKeyDown(Walnut::KeyCode::W))
			dir.y = -1;
		if (Walnut::Input::IsKeyDown(Walnut::KeyCode::S))
			dir.y = 1;

		dir = glm::length(dir) > 0 ? glm::normalize(dir) : glm::vec3{0, 0, 0};
		if (glm::length(dir) > 0)
		{
			const float speed = 500;
			m_PlayerVelocity = dir * speed;
		}
		m_PlayerVelocity = glm::mix(m_PlayerVelocity, glm::vec3(0), 5 * ts);
		m_PlayerPosition += m_PlayerVelocity * ts;

		m_PlayerRotation.y += ts * 30;

		if (connectionStatus == Walnut::Client::ConnectionStatus::Connected)
		{
			Walnut::BufferStreamWriter stream(s_ScratchBuffer, 0);
			stream.WriteRaw(PacketType::ClientUpdate);
			stream.WriteRaw<glm::vec3>(m_PlayerPosition);
			stream.WriteRaw<glm::vec3>(m_PlayerVelocity);
			m_Client.SendBuffer(stream.GetBuffer());
		}

	}

	void ClientLayer::OnRender()
	{
		// Lets draw some stuff

		// 1. Bind pipeline
		// 2. Bind vertex/index buffers
		// 3. draw call
		
		m_Renderer.BeginScene(m_Camera);

		Walnut::Client::ConnectionStatus connectionStatus = m_Client.GetConnectionStatus();
		//if (connectionStatus == Walnut::Client::ConnectionStatus::Connected)
		{
			m_Renderer.RenderCube(m_PlayerPosition, m_PlayerRotation);

			m_PlayerDataMutex.lock();
			std::map<uint32_t, PlayerData> playerData = m_PlayerData;
			m_PlayerDataMutex.unlock();

			for (const auto& [id, data] : playerData)
			{
				if (id == m_PlayerID)
					continue;
				m_Renderer.RenderCube(data.Position, {0, 0, 0});
			}
		}

		m_Renderer.EndScene();
	}

	void ClientLayer::OnUIRender()
	{
		m_Renderer.RenderUI();

		ImGui::Begin("Controls");

		ImGui::DragFloat3("Player Pos", glm::value_ptr(m_PlayerPosition), 0.05f);
		ImGui::DragFloat3("Player Rot", glm::value_ptr(m_PlayerRotation), 0.05f);

		ImGui::DragFloat3("Camera Pos", glm::value_ptr(m_Camera.Position), 0.05f);
		ImGui::DragFloat3("Camera Rot", glm::value_ptr(m_Camera.Rotation), 0.05f);

		ImGui::End();


		return;

		Walnut::Client::ConnectionStatus connectionStatus = m_Client.GetConnectionStatus();
		if (connectionStatus == Walnut::Client::ConnectionStatus::Connected)
		{
			if (0)
			{
				DrawRect(m_PlayerPosition, { 50, 50 }, 0xFFFFFFFF);

				m_PlayerDataMutex.lock();
				std::map<uint32_t, PlayerData> playerData = m_PlayerData;
				m_PlayerDataMutex.unlock();

				for (const auto& [id, data] : playerData)
				{
					if (id == m_PlayerID)
						continue;
					DrawRect(data.Position, { 50, 50 }, 0xFF33FF44);
				}
			}
		}
		else
		{
			bool readOnly = connectionStatus == Walnut::Client::ConnectionStatus::Connecting;
			ImGui::Begin("Connect To Server");

			ImGui::InputText("Server address", &m_ServerAddress, readOnly ? ImGuiInputTextFlags_ReadOnly : 0);
			if (connectionStatus == Walnut::Client::ConnectionStatus::FailedToConnect)
				ImGui::TextColored(ImColor(Walnut::UI::Colors::Theme::error), "Failed to connect!");
			else if (connectionStatus == Walnut::Client::ConnectionStatus::Connecting)
				ImGui::TextColored(ImColor(Walnut::UI::Colors::Theme::textDarker), "Connecting...");
			if (ImGui::Button("Connect"))
			{
				m_Client.ConnectToServer(m_ServerAddress);
			}

			ImGui::End();
		}

		

	}
	void ClientLayer::OnDataRecieved(const Walnut::Buffer buffer)
	{
		Walnut::BufferStreamReader stream(buffer, 0);
		PacketType type{};
		stream.ReadRaw<PacketType>(type);
		std::cout << "Data from server, packet is of type: " << PacketTypeToString(type)<< "\n";
		switch (type)
		{
		case PacketType::None:
			break;
		case PacketType::Message:
			break;
		case PacketType::ClientConnectionRequest:
			break;
		case PacketType::ConnectionStatus:
			break;
		case PacketType::ClientList:
			break;
		case PacketType::ClientConnect:
		{
			WL_INFO("Connection message from server");
			uint32_t idFromServer;
			stream.ReadRaw<uint32_t>(idFromServer);
			WL_INFO("We have connected! Server says our id is {}", idFromServer);
			WL_INFO("We say our id is {}", m_Client.GetID());
			m_PlayerID = idFromServer;
			break;
		}
		case PacketType::ClientUpdate:
		{
			m_PlayerDataMutex.lock();
			{
				stream.ReadMap(m_PlayerData);
			}
			m_PlayerDataMutex.unlock();
			break;
		}
		case PacketType::ClientDisconnect:
		{
			m_PlayerDataMutex.lock(); // Make other threads not be able to access the data.
			{
				uint32_t clientDisconnected = 0;
				stream.ReadRaw<uint32_t>(clientDisconnected);
				if (clientDisconnected != 0)
					m_PlayerData.erase(clientDisconnected);
			}
			m_PlayerDataMutex.unlock();
			break;
		}
		case PacketType::ClientUpdateResponse:
			break;
		case PacketType::MessageHistory:
			break;
		case PacketType::ServerShutdown:
			break;
		case PacketType::ClientKick:
			break;
		default:
			break;
		}
	}
}
#include "ServerLayer.h"

#include "Walnut/Core/Log.h"
#include "Walnut/Serialization/BufferStream.h"

#include "ServerPacket.h"

namespace Cubed {

	static Walnut::Buffer s_ScratchBuffer;

	void Cubed::ServerLayer::OnAttach()
	{
		s_ScratchBuffer.Allocate(10 * 1024 * 1024); // 10 MB

		m_Console.SetMessageSendCallback([this](std::string_view message) { 
			OnConsoleSend(message); 
		});

		m_Server.SetClientConnectedCallback([this](const Walnut::ClientInfo& clientInfo) {
			OnClientConnected(clientInfo);
		});

		m_Server.SetClientDisconnectedCallback([this](const Walnut::ClientInfo& clientInfo) {
			OnClientDisconnected(clientInfo);
		});

		m_Server.SetDataReceivedCallback([this](const Walnut::ClientInfo& clientInfo, const Walnut::Buffer buffer) {
			OnDataRecieved(clientInfo, buffer);
		});

		m_Server.Start();

	}

	void Cubed::ServerLayer::OnDetach()
	{
		m_Server.Stop();
	}

	void Cubed::ServerLayer::OnUpdate(float ts)
	{
		Walnut::BufferStreamWriter stream(s_ScratchBuffer);
		stream.WriteRaw(PacketType::ClientUpdate);
		m_PlayerDataMutex.lock(); // Make other threads not be able to access the data.
		{
			stream.WriteMap(m_PlayerData);
		}
		m_PlayerDataMutex.unlock();

		Walnut::Buffer packet{ stream.GetBuffer().Data, stream.GetStreamPosition() };
		m_Server.SendBufferToAllClients(stream.GetBuffer());

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(5ms);
	}

	void Cubed::ServerLayer::OnUIRender()
	{
	}

	void ServerLayer::OnConsoleSend(std::string_view message)
	{
		if (message.starts_with('/')) // command
		{
		
			std::cout << "You called the " << message << " command!" << std::endl;
		}
	}

	void ServerLayer::OnClientConnected(const Walnut::ClientInfo& client)
	{
		WL_INFO_TAG("Server", "Client connected! ID={}", client.ID);

		Walnut::BufferStreamWriter stream(s_ScratchBuffer, 0);
		// packet type
		// id

		stream.WriteRaw(PacketType::ClientConnect);
		stream.WriteRaw(client.ID);

		//m_Server.SendDataToClient(client.ID, stream.GetBuffer().Data);
		size_t length = stream.GetStreamPosition();
		Walnut::Buffer packet{ stream.GetBuffer().Data, length };

		m_Server.SendBufferToClient(client.ID, stream.GetBuffer()); // Doesnt work for some reason.
		//m_Server.SendBufferToClient(client.ID, packet);
	}

	void ServerLayer::OnClientDisconnected(const Walnut::ClientInfo& client)
	{
		WL_INFO_TAG("Server", "Client Disconnected! ID={}", client.ID);
	}

	void ServerLayer::OnDataRecieved(const Walnut::ClientInfo& client, const Walnut::Buffer buffer)
	{
		Walnut::BufferStreamReader stream(buffer, 0);
		PacketType type;
		stream.ReadRaw(type);
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
			break;
		case PacketType::ClientUpdate:
		{
			m_PlayerDataMutex.lock(); // Make other threads not be able to access the data.
			{
				PlayerData& playerData = m_PlayerData[client.ID];
				stream.ReadRaw<glm::vec2>(playerData.Position);
				stream.ReadRaw<glm::vec2>(playerData.Velocity);
			}
			m_PlayerDataMutex.unlock(); 
			break;
		}
		case PacketType::ClientDisconnect:
			break;
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

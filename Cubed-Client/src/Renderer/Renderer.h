#pragma once

#include "Vulkan.h"

#include <glm/glm.hpp>

#include <filesystem>

#include <imgui.h>



namespace Cubed
{
	struct Buffer
	{
		VkBuffer Handle = nullptr;
		VkDeviceMemory Memory = nullptr;
		VkDeviceSize BufferSize = 0;
		VkBufferUsageFlagBits Usage = VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
	};

	struct Camera
	{
		glm::vec3 Position{};
		glm::vec3 Rotation{};
	};

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Color;
		glm::vec3 Normal;
	};

	class Renderer
	{
	public:
		void Init();
		void Shutdown();

		void BeginScene(const Camera& camera);
		void EndScene();

		void Render();
		void RenderCube(const glm::vec3& position, const glm::vec3& rotation);
		void RenderUI();

	private:
		void InitPipeline();
		void InitBuffers();
		void CreateOrResizeBuffer(Buffer& buffer, uint64_t newSize);


		VkShaderModule LoadShader(const std::filesystem::path& path);

	private:
		// Buffers, pipeline, etc

		VkPipeline m_GraphicsPipeline = nullptr;
		VkPipelineLayout m_PipelineLayout = nullptr;

		Buffer m_VertexBuffer, m_IndexBuffer;

		struct PushConstants
		{
			glm::mat4 ViewProjection;
			glm::mat4 Transform;

		};

		PushConstants m_PushConstants;

	};
}


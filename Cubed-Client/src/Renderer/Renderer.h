#pragma once

#include "Vulkan.h"

#include <glm/glm.hpp>

#include <filesystem>

#include <imgui.h>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 color;
};

namespace Cubed
{
	struct Buffer
	{
		VkBuffer Handle = nullptr;
		VkDeviceMemory Memory = nullptr;
		VkDeviceSize BufferSize = 0;
		VkBufferUsageFlagBits Usage = VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
	};

	class Renderer
	{
	public:
		void Init();
		void Shutdown();

		void Render();
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
		glm::vec3 m_CubePosition;
		glm::vec3 m_CubeRotation;

	};
}


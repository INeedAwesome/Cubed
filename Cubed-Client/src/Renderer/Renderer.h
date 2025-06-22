#pragma once

#include "Vulkan.h"

#include <glm/glm.hpp>

#include <filesystem>
#include <memory>

#include <imgui.h>

#include "Texture.h"

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

	public:
		static uint32_t GetVulkanMemoryType(VkMemoryPropertyFlags properties, uint32_t type_bits);

	private:
		void InitPipeline();
		void InitBuffers();
		void CreateOrResizeBuffer(Buffer& buffer, uint64_t newSize);


		VkShaderModule LoadShader(const std::filesystem::path& path);

	private:
		// Buffers, pipeline, etc

		VkPipeline m_GraphicsPipeline = nullptr;
		VkPipelineLayout m_PipelineLayout = nullptr;

		VkDescriptorSetLayout m_DescriptorSetLayout = nullptr;
		VkDescriptorSet m_DescriptorSet = nullptr;

		Buffer m_VertexBuffer, m_IndexBuffer;

		struct PushConstants
		{
			glm::mat4 ViewProjection;
			glm::mat4 Transform;

		};

		PushConstants m_PushConstants;

		std::shared_ptr<Texture> m_Texture;

	};
}


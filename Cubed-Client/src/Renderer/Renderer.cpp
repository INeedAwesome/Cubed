#include "Renderer.h"

#include <array>
#include <fstream>

#include "Walnut/Application.h"
#include "Walnut/Core/Log.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Cubed
{

	static uint32_t ImGui_ImplVulkan_MemoryType(VkMemoryPropertyFlags properties, uint32_t type_bits)
	{
		VkPhysicalDevice physicalDevice = Cubed::GetVulkanInfo()->PhysicalDevice;
		VkPhysicalDeviceMemoryProperties prop;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &prop);
		for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
			if ((prop.memoryTypes[i].propertyFlags & properties) == properties && type_bits & (1 << i))
				return i;
		return 0xFFFFFFFF; // Unable to find memoryType
	}


	void Renderer::Init()
	{
		InitBuffers();
		InitPipeline();
	}

	void Renderer::Shutdown()
	{
		WL_INFO("Shutting down!");

		VkDevice device = GetVulkanInfo()->Device;

		vkDestroyPipeline(device, m_GraphicsPipeline, nullptr);

		vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);

		vkDestroyBuffer(device, m_VertexBuffer.Handle, nullptr);
		vkFreeMemory(device, m_VertexBuffer.Memory, nullptr);
		vkDestroyBuffer(device, m_IndexBuffer.Handle, nullptr);
		vkFreeMemory(device, m_IndexBuffer.Memory, nullptr);

	}

	void Renderer::Render()
	{
		VkCommandBuffer commandBuffer = Walnut::Application::GetActiveCommandBuffer();
		ImGui_ImplVulkanH_Window* windowData = Walnut::Application::GetMainWindowData();

		float vpWidth = (float)windowData->Width;
		float vpHeight = (float)windowData->Height;

		// Bind the graphics pipeline.
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

	

		glm::mat4 cameraTransform = glm::translate(glm::mat4(1.0f), m_CameraPosition)
			* glm::eulerAngleXYZ(glm::radians(m_CameraRotation.x), glm::radians(m_CameraRotation.y), glm::radians(m_CameraRotation.z));;

		m_PushConstants.ViewProjection = glm::perspectiveFov(glm::radians(70.0f), vpWidth, vpHeight, 0.01f, 1000.0f)
			* glm::inverse(cameraTransform);

		m_PushConstants.Transform = glm::translate(glm::mat4(1.0f), m_CubePosition)
			* glm::eulerAngleXYZ(glm::radians(m_CubeRotation.x), glm::radians(m_CubeRotation.y), glm::radians(m_CubeRotation.z));

		vkCmdPushConstants(commandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &m_PushConstants);

		VkDeviceSize offset{ 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_VertexBuffer.Handle, &offset);

		vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer.Handle, offset, VK_INDEX_TYPE_UINT32);

		VkViewport vp{
			.y = vpHeight,
			.width = vpWidth,
			.height = -vpHeight,
			.minDepth = 0.0f,
			.maxDepth = 1.0f };
		// Set viewport dynamically
		vkCmdSetViewport(commandBuffer, 0, 1, &vp);

		VkRect2D scissor{
			.extent = {.width = (uint32_t)windowData->Width, .height = (uint32_t)windowData->Height} };
		// Set scissor dynamically
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		//// Bind the vertex buffer to source the draw calls from.
		//VkDeviceSize offset = { 0 };
		//vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertex_buffer, &offset);

		// Draw three vertices with one instance from the currently bound vertex bound.
		vkCmdDrawIndexed(commandBuffer, m_IndexBuffer.BufferSize / sizeof(uint32_t), 1, 0, 0, 0);
	}

	void Renderer::RenderCube(const glm::vec3& position)
	{
		glm::vec3 translation = position * m_CubeScale;

		VkCommandBuffer commandBuffer = Walnut::Application::GetActiveCommandBuffer();
		ImGui_ImplVulkanH_Window* windowData = Walnut::Application::GetMainWindowData();

		float vpWidth = (float)windowData->Width;
		float vpHeight = (float)windowData->Height;

		// Bind the graphics pipeline.
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

		glm::mat4 cameraTransform = glm::translate(glm::mat4(1.0f), m_CameraPosition)
			* glm::eulerAngleXYZ(glm::radians(m_CameraRotation.x), glm::radians(m_CameraRotation.y), glm::radians(m_CameraRotation.z));;

		m_PushConstants.ViewProjection = glm::perspectiveFov(glm::radians(70.0f), vpWidth, vpHeight, 0.01f, 1000.0f)
			* glm::inverse(cameraTransform);

		m_PushConstants.Transform = glm::translate(glm::mat4(1.0f), translation)
			* glm::eulerAngleXYZ(glm::radians(m_CubeRotation.x), glm::radians(m_CubeRotation.y), glm::radians(m_CubeRotation.z));

		vkCmdPushConstants(commandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &m_PushConstants);

		VkDeviceSize offset{ 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_VertexBuffer.Handle, &offset);

		vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer.Handle, offset, VK_INDEX_TYPE_UINT32);

		VkViewport vp{
			.y = vpHeight,
			.width = vpWidth,
			.height = -vpHeight,
			.minDepth = 0.0f,
			.maxDepth = 1.0f };
		// Set viewport dynamically
		vkCmdSetViewport(commandBuffer, 0, 1, &vp);

		VkRect2D scissor{
			.extent = {.width = (uint32_t)windowData->Width, .height = (uint32_t)windowData->Height} };
		// Set scissor dynamically
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		// Draw three vertices with one instance from the currently bound vertex bound.
		vkCmdDrawIndexed(commandBuffer, m_IndexBuffer.BufferSize / sizeof(uint32_t), 1, 0, 0, 0);
	}

	void Renderer::RenderUI()
	{
		ImGui::Begin("Controls");

		ImGui::DragFloat3("Position", glm::value_ptr(m_CubePosition), 0.05f);
		ImGui::DragFloat3("Rotation", glm::value_ptr(m_CubeRotation), 0.15f);

		ImGui::DragFloat3("Camera Position", glm::value_ptr(m_CameraPosition), 0.05f);
		ImGui::DragFloat3("Camera Rotation", glm::value_ptr(m_CameraRotation), 0.05f);
		ImGui::DragFloat("m_CubeScale", &m_CubeScale, 0.005f);

		ImGui::End();

	}

	void Renderer::InitPipeline()
	{
		VkDevice device = GetVulkanInfo()->Device;
		VkRenderPass renderPass = Walnut::Application::GetMainWindowData()->RenderPass;


		std::array<VkPushConstantRange, 1> pushConstantRanges{};
		pushConstantRanges[0].offset = 0;
		pushConstantRanges[0].size = sizeof(glm::mat4) * 2;
		pushConstantRanges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		// Create a blank pipeline layout.
		// We are not binding any resources to the pipeline in this first sample.
		VkPipelineLayoutCreateInfo layout_info{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		layout_info.pPushConstantRanges = pushConstantRanges.data();
		layout_info.pushConstantRangeCount = (uint32_t)pushConstantRanges.size();
		VK_CHECK(vkCreatePipelineLayout(device, &layout_info, nullptr, &m_PipelineLayout));

		// The Vertex input properties define the interface between the vertex buffer and the vertex shader.

		// Specify we will use triangle lists to draw geometry.
		VkPipelineInputAssemblyStateCreateInfo input_assembly{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST 
		};

		// Define the vertex input binding.
		VkVertexInputBindingDescription binding_description{
			.binding = 0,
			.stride = sizeof(Vertex),
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX 
		};

		// Define the vertex input attribute.
		std::array<VkVertexInputAttributeDescription, 2> attribute_descriptions{
			{
				{
					.location = 0, 
					.binding = 0, 
					.format = VK_FORMAT_R32G32B32_SFLOAT, 
					.offset = offsetof(Vertex, position)
				}, 
				{
					.location = 1, 
					.binding = 0, 
					.format = VK_FORMAT_R32G32B32_SFLOAT, 
					.offset = offsetof(Vertex, color)
				}
			}
		};

		// Define the pipeline vertex input.
		VkPipelineVertexInputStateCreateInfo vertex_input{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
		vertex_input.vertexBindingDescriptionCount = 1;
		vertex_input.pVertexBindingDescriptions = &binding_description;
		vertex_input.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
		vertex_input.pVertexAttributeDescriptions = attribute_descriptions.data();
		
		// Specify rasterization state.
		VkPipelineRasterizationStateCreateInfo raster{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		raster.cullMode = VK_CULL_MODE_BACK_BIT;
		raster.frontFace = VK_FRONT_FACE_CLOCKWISE;
		raster.lineWidth = 1.0f;

		// Our attachment will write to all color channels, but no blending is enabled.
		VkPipelineColorBlendAttachmentState blend_attachment{
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT };

		VkPipelineColorBlendStateCreateInfo blend{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.attachmentCount = 1,
			.pAttachments = &blend_attachment };

		// We will have one viewport and scissor box.
		VkPipelineViewportStateCreateInfo viewport{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount = 1,
			.scissorCount = 1 };

		// Disable all depth testing.
		VkPipelineDepthStencilStateCreateInfo depth_stencil{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };

		// No multisampling.
		VkPipelineMultisampleStateCreateInfo multisample{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT };

		// Specify that these states will be dynamic, i.e. not part of pipeline state object.
		std::array<VkDynamicState, 2> dynamics{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

		VkPipelineDynamicStateCreateInfo dynamic{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.dynamicStateCount = static_cast<uint32_t>(dynamics.size()),
			.pDynamicStates = dynamics.data() };

		// Load our SPIR-V shaders.

		// Samples support different shading languages, all of which are offline compiled to SPIR-V, the shader format that Vulkan uses.
		// The shading language to load for can be selected via command line
	/*	std::string shader_folder{ "" };
		switch (get_shading_language())
		{
		case vkb::ShadingLanguage::HLSL:
			shader_folder = "hlsl";
			break;
		case vkb::ShadingLanguage::SLANG:
			shader_folder = "slang";
			break;
		default:
			shader_folder = "glsl";
		}*/

		std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages{};

		// Vertex stage of the pipeline
		shader_stages[0] = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			.module = LoadShader("Assets/Shaders/bin/basic.vert.spirv"),
			.pName = "main" };

		// Fragment stage of the pipeline
		shader_stages[1] = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = LoadShader("Assets/Shaders/bin/basic.frag.spirv"),
			.pName = "main" };

		VkGraphicsPipelineCreateInfo pipe{
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.stageCount = static_cast<uint32_t>(shader_stages.size()),
			.pStages = shader_stages.data(),
			.pVertexInputState = &vertex_input,
			.pInputAssemblyState = &input_assembly,
			.pViewportState = &viewport,
			.pRasterizationState = &raster,
			.pMultisampleState = &multisample,
			.pDepthStencilState = &depth_stencil,
			.pColorBlendState = &blend,
			.pDynamicState = &dynamic,
			.layout = m_PipelineLayout,        // We need to specify the pipeline layout up front
			.renderPass = renderPass             // We need to specify the render pass up front
		};
		
		VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipe, nullptr, &m_GraphicsPipeline));

		// Pipeline is baked, we can delete the shader modules now.
		vkDestroyShaderModule(device, shader_stages[0].module, nullptr);
		vkDestroyShaderModule(device, shader_stages[1].module, nullptr);
	}

	void Renderer::InitBuffers()
	{
		VkDevice device = GetVulkanInfo()->Device;

		glm::vec3 vertexData[8] = { 
			// front
			glm::vec3(-0.5f, -0.5f, 0.5f), // bottom left
			glm::vec3(-0.5f,  0.5f, 0.5f), // top left
			glm::vec3( 0.5f,  0.5f, 0.5f), // top right
			glm::vec3( 0.5f, -0.5f, 0.5f), //  bottom right

			// back
			glm::vec3( 0.5f, -0.5f, -0.5f), // bottom left
			glm::vec3( 0.5f,  0.5f, -0.5f), // top left
			glm::vec3(-0.5f,  0.5f, -0.5f), // top right
			glm::vec3(-0.5f, -0.5f, -0.5f)  //  bottom right
		};

		uint32_t indices[36] = { 
			0, 1, 2, 2, 3, 0, // Front
			3, 2, 5, 5, 4, 3, // Right
			4, 5, 6, 6, 7, 4, // Back
			7, 6, 1, 1, 0, 7, // Left
			1, 6, 5, 5, 2, 1, // Top
			7, 0, 3, 3, 4, 7  // Bottom
		};
		
		Vertex vertices[8] = {
			{vertexData[0], glm::vec3{1, 0, 0}}, // pos 1, color red
			{vertexData[1], glm::vec3{0, 1, 0}}, // pos 2, color green
			{vertexData[2], glm::vec3{0, 0, 1}}, // pos 3, color blue
			{vertexData[3], glm::vec3{1, 1, 1}}, // pos 4, color blue

			{vertexData[4], glm::vec3{1, 0, 0}}, 
			{vertexData[5], glm::vec3{0, 1, 0}}, 
			{vertexData[6], glm::vec3{0, 0, 1}}, 
			{vertexData[7], glm::vec3{1, 1, 1}}  
		};

		// Create vertex and index buffer
		m_VertexBuffer.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		CreateOrResizeBuffer(m_VertexBuffer, sizeof(vertices));

		m_IndexBuffer.Usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		CreateOrResizeBuffer(m_IndexBuffer, sizeof(indices));

		// Map memory from the cpu to the gpu
		Vertex* vbMemory{};
		VK_CHECK(vkMapMemory(device, m_VertexBuffer.Memory, 0, sizeof(vertices), 0, (void**)&vbMemory));
		memcpy_s(vbMemory, sizeof(vertices), vertices, sizeof(vertices));

		uint32_t* ibMemory{};
		VK_CHECK(vkMapMemory(device, m_IndexBuffer.Memory, 0, sizeof(indices), 0, (void**)&ibMemory));
		memcpy_s(ibMemory, sizeof(indices), indices, sizeof(indices));

		VkMappedMemoryRange range[2] = {};
		range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		range[0].memory = m_VertexBuffer.Memory;
		range[0].size = VK_WHOLE_SIZE;
		range[1].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		range[1].memory = m_IndexBuffer.Memory;
		range[1].size = VK_WHOLE_SIZE;

		VK_CHECK(vkFlushMappedMemoryRanges(device, 2, range));
		vkUnmapMemory(device, m_VertexBuffer.Memory);
		vkUnmapMemory(device, m_IndexBuffer.Memory);

	}

	VkShaderModule Renderer::LoadShader(const std::filesystem::path& path)
	{
		std::ifstream stream(path, std::ios::binary);

		if (!stream)
		{
			WL_ERROR("Could not open file! {}", path.string());
			return nullptr;
		}

		stream.seekg(0, std::ios_base::end);
		std::streampos size = stream.tellg();
		stream.seekg(0, std::ios_base::beg);

		std::vector <char> buffer(size);
		if (!stream.read(buffer.data(), size))
		{
			WL_ERROR("Could not open file! {}", path.string());
			return nullptr;
		}

		stream.close();

		VkShaderModuleCreateInfo shaderModuleCI{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
		shaderModuleCI.pCode = (uint32_t*)buffer.data();
		shaderModuleCI.codeSize = buffer.size();

		VkDevice device = GetVulkanInfo()->Device;
		VkShaderModule result = nullptr;
		vkCreateShaderModule(device, &shaderModuleCI, nullptr, &result);
		return result;

	}
	
	void Renderer::CreateOrResizeBuffer(Buffer& buffer, uint64_t newSize)
	{
		VkDevice device = GetVulkanInfo()->Device;

		if (buffer.Handle != VK_NULL_HANDLE)
			vkDestroyBuffer(device, buffer.Handle, nullptr);
		if (buffer.Memory != VK_NULL_HANDLE)
			vkFreeMemory(device, buffer.Memory, nullptr);

		VkBufferCreateInfo buffer_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		buffer_info.size = newSize;
		buffer_info.usage = buffer.Usage;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VK_CHECK(vkCreateBuffer(device, &buffer_info, nullptr, &buffer.Handle));

		VkMemoryRequirements req;
		vkGetBufferMemoryRequirements(device, buffer.Handle, &req);
		// bd->BufferMemoryAlignment = (bd->BufferMemoryAlignment > req.alignment) ? bd->BufferMemoryAlignment : req.alignment;

		VkMemoryAllocateInfo alloc_info = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		alloc_info.allocationSize = req.size;
		alloc_info.memoryTypeIndex = ImGui_ImplVulkan_MemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits);
		VK_CHECK(vkAllocateMemory(device, &alloc_info, nullptr, &buffer.Memory));

		VK_CHECK(vkBindBufferMemory(device, buffer.Handle, buffer.Memory, 0));
		buffer.BufferSize = req.size;
	}


}

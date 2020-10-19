#include "VulkanBuffer.h"

#include "platform/vulkan/VulkanDevice.h"

vkpc::VulkanBuffer::VulkanBuffer(VulkanDevice* device)
	: m_OwningDevice(device), m_Buffer(VK_NULL_HANDLE), m_Flags(VK_FLAGS_NONE), m_Size(0), m_DeviceMemory(VK_NULL_HANDLE)
{
}

vkpc::VulkanBuffer::~VulkanBuffer()
{
	DestroyBuffer(true);
}

VkBuffer vkpc::VulkanBuffer::GetVkBuffer()
{
	return m_Buffer;
}

VkDescriptorBufferInfo vkpc::VulkanBuffer::GetDescriptorInfo()
{
	return m_DescriptorInfo;
}

void vkpc::VulkanBuffer::SetFlags(VkBufferCreateFlags flags)
{
	m_Flags = flags;
}

void vkpc::VulkanBuffer::SetSize(VkDeviceSize size)
{
	m_Size = size;
}

void vkpc::VulkanBuffer::SetUsage(VkBufferUsageFlags usage)
{
	m_Usage = usage;
}

void vkpc::VulkanBuffer::SetSharingMode(VkSharingMode sharingMode)
{
	m_SharingMode = sharingMode;
}

bool vkpc::VulkanBuffer::Construct()
{
	return CreateBuffer();
}

void vkpc::VulkanBuffer::Demolish(bool freeMemory)
{
	DestroyBuffer(freeMemory);
}

void vkpc::VulkanBuffer::Allocate(VkMemoryPropertyFlags properties)
{
	AllocateMemory(properties);
}

bool vkpc::VulkanBuffer::FillBuffer(void* block)
{
	VKPC_ASSERT(m_Size > 0);
	VKPC_ASSERT(IsValid()); 

	if (vkBindBufferMemory(m_OwningDevice->GetLogicalDevice(), m_Buffer, m_DeviceMemory, 0) != VK_SUCCESS)
	{
		VKPC_LOG_ERROR("Failed to bind allocated memory to buffer!");
		return false;
	}

	void* data;
	vkMapMemory(m_OwningDevice->GetLogicalDevice(), m_DeviceMemory, 0, m_Size, 0, &data);
	memcpy(data, block, m_Size);
	vkUnmapMemory(m_OwningDevice->GetLogicalDevice(), m_DeviceMemory);

	//Mark the resource as currently active (so that the device can clean up the memory when its cleaned up).
	m_IsActive = true;

	return true;
}

bool vkpc::VulkanBuffer::CreateBuffer()
{
	VKPC_ASSERT(m_Size > 0);

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.flags = m_Flags;
	bufferInfo.size = m_Size;
	bufferInfo.usage = m_Usage;
	bufferInfo.sharingMode = m_SharingMode;

	if (vkCreateBuffer(m_OwningDevice->GetLogicalDevice(), &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS) {
		VKPC_LOG_ERROR("failed to create buffer!");
		m_IsValid = false;
		return false;
	}

	return true;
}

void vkpc::VulkanBuffer::DestroyBuffer(bool freeMemory)
{
	if (m_Buffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(m_OwningDevice->GetLogicalDevice(), m_Buffer, nullptr);
	}

	if (freeMemory)
	{
		FreeMemory();
	}
}

bool vkpc::VulkanBuffer::AllocateMemory(VkMemoryPropertyFlags properties)
{
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(m_OwningDevice->GetLogicalDevice(), m_Buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(m_OwningDevice->GetLogicalDevice(), &allocInfo, nullptr, &m_DeviceMemory) != VK_SUCCESS) {
		VKPC_LOG_ERROR("Failed to allocate buffer memory!");
		m_IsValid = false;
		return false;
	}

	return true;
}

void vkpc::VulkanBuffer::FreeMemory()
{
	if (IsActive())
	{
		if (m_DeviceMemory != VK_NULL_HANDLE)
		{
			vkFreeMemory(m_OwningDevice->GetLogicalDevice(), m_DeviceMemory, nullptr);
			m_DeviceMemory = VK_NULL_HANDLE;
		}

		//Make sure it only ever gets cleaned up once.
		m_IsActive = false;
	}
}

void vkpc::VulkanBuffer::SetupDescriptorInfo()
{
	m_DescriptorInfo = {};
	m_DescriptorInfo.buffer = m_Buffer;
	m_DescriptorInfo.offset = 0;
	m_DescriptorInfo.range = VK_WHOLE_SIZE;
}

uint32 vkpc::VulkanBuffer::FindMemoryType(uint32 filter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(m_OwningDevice->GetPhysicalDevice(), &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((filter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	return -1;
}

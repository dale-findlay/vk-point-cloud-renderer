#include "VulkanBuffer.h"

#include "platform/vulkan/VulkanDevice.h"

vkpc::VulkanBuffer::VulkanBuffer(VulkanDevice* device, size_t size, VkBufferUsageFlags usageFlags, VkSharingMode sharingMode, VkBufferCreateFlags createFlags)
	: m_OwningDevice(device), m_Size(size), m_UsageFlags(usageFlags), m_SharingMode(sharingMode), m_CreateFlags(createFlags), m_DeviceMemory(VK_NULL_HANDLE)
{
	if (!CreateBuffer())
	{
		VKPC_LOG_ERROR("Failed to create buffer!");
		m_IsValid = false;
	}
}

vkpc::VulkanBuffer::~VulkanBuffer()
{	
	DestroyBuffer();
	FreeMemory();
}

VkBuffer vkpc::VulkanBuffer::GetVkBuffer()
{
	return m_Buffer;
}

size_t vkpc::VulkanBuffer::GetSize() const
{
	return m_Size;
}

VkDescriptorBufferInfo vkpc::VulkanBuffer::GetDescriptorInfo()
{
	return m_DescriptorInfo;
}

VkSharingMode vkpc::VulkanBuffer::GetSharingMode() const
{
	return m_SharingMode;
}

bool vkpc::VulkanBuffer::FillBuffer(void* block, size_t size, VkMemoryPropertyFlags properties)
{
	VKPC_ASSERT(size == m_Size);
	VKPC_ASSERT(IsValid()); // definitely want this one!

	if (!AllocateMemory(properties))
	{
		VKPC_LOG_ERROR("Failed to allocate memory to buffer!");
		return false;
	}
	
	if (vkBindBufferMemory(m_OwningDevice->GetLogicalDevice(), m_Buffer, m_DeviceMemory, 0) != VK_SUCCESS)
	{
		VKPC_LOG_ERROR("Failed to bind allocated memory to buffer!");
		return false;
	}

	void* data;
	vkMapMemory(m_OwningDevice->GetLogicalDevice(), m_DeviceMemory, 0, m_Size, 0, &data);
	memcpy(data, block, size);	
	vkUnmapMemory(m_OwningDevice->GetLogicalDevice(), m_DeviceMemory);

	return true;
}

bool vkpc::VulkanBuffer::CreateBuffer()
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = m_Size;
	bufferInfo.usage = m_UsageFlags;
	bufferInfo.sharingMode = m_SharingMode;

	//Only create flags are technically optional.
	if (m_CreateFlags != -1)
	{
		bufferInfo.flags = m_SharingMode;
	}

	if (vkCreateBuffer(m_OwningDevice->GetLogicalDevice(), &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS) {
		VKPC_LOG_ERROR("failed to create vertex buffer!");
		return false;
	}

	//Mark the resource as currently active (so that the device can clean up the memory when its cleaned up).
	m_IsActive = true;

	return true;
}

void vkpc::VulkanBuffer::DestroyBuffer()
{
	if (IsActive())
	{
		if (m_Buffer)
		{
			vkDestroyBuffer(m_OwningDevice->GetLogicalDevice(), m_Buffer, nullptr);
		}

		//Make sure it only ever gets cleaned up once.
		m_IsActive = false;
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
		return false;
	}

	return true;
}

void vkpc::VulkanBuffer::FreeMemory()
{
	if (m_DeviceMemory)
	{
		vkFreeMemory(m_OwningDevice->GetLogicalDevice(), m_DeviceMemory, nullptr);
		m_DeviceMemory = VK_NULL_HANDLE;
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

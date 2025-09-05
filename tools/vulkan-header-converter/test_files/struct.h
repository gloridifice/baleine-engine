typedef struct VkRenderPassCreateInfo {
  VkStructureType sType;
  const void *pNext;
  VkRenderPassCreateFlags flags;
  uint32_t attachmentCount;
  const VkAttachmentDescription *pAttachments;
  uint32_t subpassCount;
  const VkSubpassDescription *pSubpasses;
  uint32_t dependencyCount;
  const VkSubpassDependency *pDependencies;
} VkRenderPassCreateInfo;

typedef struct VkCommandPoolCreateInfo {
  VkStructureType sType;
  const void *pNext;
  VkCommandPoolCreateFlags flags;
  uint32_t queueFamilyIndex;
} VkCommandPoolCreateInfo;

typedef struct VkSamplerCreateInfo {
  VkStructureType sType;
  const void *pNext;
  VkSamplerCreateFlags flags;
  VkFilter magFilter;
  VkFilter minFilter;
  VkSamplerMipmapMode mipmapMode;
  VkSamplerAddressMode addressModeU;
  VkSamplerAddressMode addressModeV;
  VkSamplerAddressMode addressModeW;
  float mipLodBias;
  VkBool32 anisotropyEnable;
  float maxAnisotropy;
  VkBool32 compareEnable;
  VkCompareOp compareOp;
  float minLod;
  float maxLod;
  VkBorderColor borderColor;
  VkBool32 unnormalizedCoordinates;
} VkSamplerCreateInfo;

typedef struct VkExtent2D {
  uint32_t width;
  uint32_t height;
} VkExtent2D;

typedef struct VkExtent3D {
  uint32_t width;
  uint32_t height;
  uint32_t depth;
} VkExtent3D;

typedef struct VkOffset2D {
  int32_t x;
  int32_t y;
} VkOffset2D;

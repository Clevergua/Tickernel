#pragma once
#include "gfxResources.h"

typedef struct PipelineStruct
{
    VkPipelineLayout vkPipelineLayout;                // for recording command buffers
    VkDescriptorSetLayout vkDescriptorSetLayout;      // for creating descriptor set
    TknDynamicArray vkDescriptorPoolSizeDynamicArray; // for creating descriptor pool
    VkPipeline vkPipeline;                            // vkPipeline
    TknDynamicArray materialPtrDynamicArray;          // materials
} Pipeline;

typedef struct
{
    TknDynamicArray pipelinePtrDynamicArray;          // pipelines
    TknDynamicArray vkWriteDescriptorSetDynamicArray; // for rewriting descriptor sets
    TknDynamicArray inputAttachmentIndexDynamicArray; // for rewriting descriptor sets
    VkDescriptorSetLayout vkDescriptorSetLayout;      // for creating descriptor set & pipelines
    VkDescriptorPool vkDescriptorPool;                // for creating descriptor set
    VkDescriptorSet vkDescriptorSet;                  // subpass descriptor set
} Subpass;

typedef struct
{
    VkRenderPass vkRenderPass;
    uint32_t attachmentCount;
    Attachment **attachmentPtrs;
    bool useSwapchain;
    uint32_t vkFramebufferCount;
    VkFramebuffer *vkFramebuffers;
    uint32_t subpassCount;
    Subpass *subpasses;
} RenderPass;

void createFramebuffers(GfxContext *pGfxContext, RenderPass *pRenderPass);
void destroyFramebuffers(GfxContext *pGfxContext, RenderPass *pRenderPass);

void createRenderPassPtr(GfxContext *pGfxContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment **inputAttachmentPtrs, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, TknDynamicArray *spvPathDynamicArrays, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, uint32_t renderPassIndex, RenderPass **ppRenderPass);
void destroyRenderPassPtr(GfxContext *pGfxContext, RenderPass *pRenderPass);

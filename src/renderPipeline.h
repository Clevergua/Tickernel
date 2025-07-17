#pragma once
#include "renderResources.h"

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

void createFramebuffers(GraphicsContext *pGraphicsContext, RenderPass *pRenderPass);
void destroyFramebuffers(GraphicsContext *pGraphicsContext, RenderPass *pRenderPass);

void createRenderPassPtr(GraphicsContext *pGraphicsContext, uint32_t attachmentCount, VkAttachmentDescription *vkAttachmentDescriptions, Attachment **inputAttachmentPtrs, uint32_t subpassCount, VkSubpassDescription *vkSubpassDescriptions, TknDynamicArray *spvPathDynamicArrays, uint32_t vkSubpassDependencyCount, VkSubpassDependency *vkSubpassDependencies, uint32_t renderPassIndex, RenderPass **ppRenderPass);
void destroyRenderPassPtr(GraphicsContext *pGraphicsContext, RenderPass *pRenderPass);

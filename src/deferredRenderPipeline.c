#include <deferredRenderPipeline.h>

void CreateDeferredRenderPipeline(GraphicEngine *pGraphicEngine)
{
    VkAttachmentDescription colorAttachmentDescription = {
        .flags = 0,
        .format = pGraphicEngine->surfaceFormat.format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };
    VkAttachmentDescription depthAttachmentDescription = {
        .flags = 0,
        .format = pGraphicEngine->depthFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };
    VkAttachmentDescription albedoAttachmentDescription = {
        .flags = 0,
        .format = pGraphicEngine->albedoFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };

    uint32_t attachmentCount = 3;
    VkAttachmentDescription vkAttachmentDescriptions[] = {
        colorAttachmentDescription,
        depthAttachmentDescription,
        albedoAttachmentDescription,
    };

    VkAttachmentReference geometryDepthAttachmentReference = {
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };
    VkAttachmentReference geometryAlbedoAttachmentReference = {
        .attachment = 2,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
    VkSubpassDescription geometrySubpassDescription = {
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = NULL,
        .colorAttachmentCount = 1,
        .pColorAttachments = &geometryAlbedoAttachmentReference,
        .pResolveAttachments = NULL,
        .pDepthStencilAttachment = &geometryDepthAttachmentReference,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = NULL,
    };

    VkAttachmentReference lightColorAttachmentReference = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
    VkAttachmentReference lightDepthAttachmentReference = {
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
    };
    VkAttachmentReference lightAlbedoAttachmentReference = {
        .attachment = 2,
        .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };
    VkSubpassDescription ligthSubpassDescription = {
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 2,
        .pInputAttachments = (const VkAttachmentReference[]){lightDepthAttachmentReference, lightAlbedoAttachmentReference},
        .colorAttachmentCount = 1,
        .pColorAttachments = &lightColorAttachmentReference,
        .pResolveAttachments = NULL,
        .pDepthStencilAttachment = &geometryDepthAttachmentReference,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = NULL,
    };
    uint32_t subpassCount = 2;
    VkSubpassDescription vkSubpassDescriptions[] = {
        geometrySubpassDescription,
        ligthSubpassDescription,
    };
    uint32_t dependencyCount = subpassCount - 1;
    VkSubpassDependency subpassDependencies[dependencyCount];
    for (uint32_t i = 0; i < dependencyCount; i++)
    {
        subpassDependencies[i].srcSubpass = i;
        subpassDependencies[i].dstSubpass = i + 1;
        subpassDependencies[i].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependencies[i].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        subpassDependencies[i].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        subpassDependencies[i].dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
        subpassDependencies[i].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    }

    VkRenderPassCreateInfo vkRenderPassCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .attachmentCount = attachmentCount,
        .pAttachments = vkAttachmentDescriptions,
        .subpassCount = subpassCount,
        .pSubpasses = vkSubpassDescriptions,
        .dependencyCount = dependencyCount,
        .pDependencies = subpassDependencies,
    };
    TickernelRenderPass tickernelRenderPass = {
        vkRenderPassCreateInfo = vkRenderPassCreateInfo,
    };

}
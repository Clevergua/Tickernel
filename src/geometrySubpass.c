#include "geometrySubpass.h"

void createGeometrySubpass(Subpass *pGeometrySubpass, PipelineConfig *pipelineConfigs, uint32_t pipelineConfigCount, VkDevice vkDevice)
{
    createPipelines(pGeometrySubpass, pipelineConfigs, pipelineConfigCount, vkDevice);
}

void destroyGeometrySubpass(Subpass *pGeometrySubpass, VkDevice vkDevice)
{
    destroyPipelines(pGeometrySubpass, vkDevice);
}

void recordGeometrySubpass(Subpass *pGeometrySubpass, VkCommandBuffer vkCommandBuffer)
{
    for (uint32_t i = 0; i < pGeometrySubpass->pipelineCount; i++)
    {
        Pipeline *pPipeline = &pGeometrySubpass->pipelines[i];
        vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->vkPipeline);
        for (uint32_t modelIndex = 0; modelIndex < pPipeline->materialDynamicArray.length; modelIndex++)
        {
            Material *pMaterial = pPipeline->materialDynamicArray.array[modelIndex];
            vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->vkPipelineLayout, 0, 1, &pMaterial->vkDescriptorSet, 0, NULL);
            for (uint32_t meshIndex = 0; meshIndex < pMaterial->meshDynamicArray.length; meshIndex++)
            {
                Mesh *pMesh = pMaterial->meshDynamicArray.array[meshIndex];
                if (pMesh->vertexCount > 0)
                {
                    vkCmdBindVertexBuffers(vkCommandBuffer, 0, 1, &pMesh->vertexBuffer.vkBuffer, (VkDeviceSize[]){0});
                    if (pMesh->indexCount > 0)
                    {
                        vkCmdBindIndexBuffer(vkCommandBuffer, pMesh->indexBuffer.vkBuffer, 0, VK_INDEX_TYPE_UINT32);
                        vkCmdDrawIndexed(vkCommandBuffer, pMesh->indexCount, pMesh->instanceCount, 0, 0, 0);
                    }
                    else
                    {
                    }
                }
                else
                {
                    vkCmdDraw(vkCommandBuffer, pMesh->vertexCount, pMesh->instanceCount, 0, 0);
                }
            }
        }
    }
}

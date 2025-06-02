#pragma once
#include "graphicCore.h"



void createGeometrySubpass(Subpass *pGeometrySubpass, PipelineConfig *pipelineConfigs, uint32_t pipelineConfigCount, VkDevice vkDevice);
void destroyGeometrySubpass(Subpass *pGeometrySubpass, VkDevice vkDevice);
void recordGeometrySubpass(Subpass *pGeometrySubpass, VkCommandBuffer vkCommandBuffer);
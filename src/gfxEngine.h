#pragma once
// #include <standardLibrary.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>
#include <tickernelCore.h>

typedef enum
{
    TKNColorAttachmentType,
    TKNDepthAttachmentType,
    TKNStencilAttachmentType,
    TKNPositionAttachmentType,
    TKNNormalAttachmentType,
    TKNAlbedoAttachmentType,
    TNKCustomAttachmentType,
} TKNAttachmentType;

typedef struct VkRenderPassCreateInfoNodePtrStruct
{
    VkRenderPassCreateInfo *pVkRenderPassCreateInfo;
    struct VkRenderPassCreateInfoNodePtrStruct *pNext;
} VkRenderPassCreateInfoPtrNode;

typedef struct VkGraphicsPipelineCreateConfigStruct
{
    VkGraphicsPipelineCreateInfo vkGraphicsPipelineCreateInfo;
    uint32_t vkPipelineShaderStageCreateInfosCount;
    VkPipelineShaderStageCreateInfo *vkPipelineShaderStageCreateInfos;
    VkPipelineLayoutCreateInfo vkPipelineLayoutCreateInfo;
    uint32_t vkDescriptorSetLayoutCreateInfoCount;
    VkDescriptorSetLayoutCreateInfo *vkDescriptorSetLayoutCreateInfos;
    VkRenderPassCreateInfo vkRenderPassCreateInfo;
    VkFramebufferCreateInfo vkFramebufferCreateInfo;
    TKNAttachmentType *tknAttachmentTypes;
} VkGraphicsPipelineCreateConfig;


// struct nlist { /* table entry: */
//     struct nlist *next; /* next entry in chain */
//     char *name; /* defined name */
//     char *defn; /* replacement text */
// };

// #define HASHSIZE 101
// static struct nlist *hashtab[HASHSIZE]; /* pointer table */

// /* hash: form hash value for string s */
// unsigned hash(char *s)
// {
//     unsigned hashval;
//     for (hashval = 0; *s != '\0'; s++)
//       hashval = *s + 31 * hashval;
//     return hashval % HASHSIZE;
// }

// /* lookup: look for s in hashtab */
// struct nlist *lookup(char *s)
// {
//     struct nlist *np;
//     for (np = hashtab[hash(s)]; np != NULL; np = np->next)
//         if (strcmp(s, np->name) == 0)
//           return np; /* found */
//     return NULL; /* not found */
// }

// char *strdup(char *);
// /* install: put (name, defn) in hashtab */
// struct nlist *install(char *name, char *defn)
// {
//     struct nlist *np;
//     unsigned hashval;
//     if ((np = lookup(name)) == NULL) { /* not found */
//         np = (struct nlist *) malloc(sizeof(*np));
//         if (np == NULL || (np->name = strdup(name)) == NULL)
//           return NULL;
//         hashval = hash(name);
//         np->next = hashtab[hashval];
//         hashtab[hashval] = np;
//     } else /* already there */
//         free((void *) np->defn); /*free previous defn */
//     if ((np->defn = strdup(defn)) == NULL)
//        return NULL;
//     return np;
// }

// char *strdup(char *s) /* make a duplicate of s */
// {
//     char *p;
//     p = (char *) malloc(strlen(s)+1); /* +1 for ’\0’ */
//     if (p != NULL)
//        strcpy(p, s);
//     return p;
// }

typedef struct TKNGraphicPipelineStruct
{
    VkRenderPass vkRenderPass;
    VkFramebuffer *vkFramebuffers;
    VkPipelineLayout vkPipelineLayout;
    VkPipeline vkPipeline;
    VkCommandBuffer *vkCommandBuffers;
} TKNGraphicPipeline;

typedef struct GraphicPipelineNodeStruct
{
    struct GraphicPipelineNodeStruct *pNext;
    VkGraphicsPipelineCreateConfig *pVkGraphicsPipelineCreateConfig;
    TKNGraphicPipeline *pTKNGraphicPipeline;
} GraphicPipelineNode;

typedef struct GFXEngineStruct
{
    // Config
    bool enableValidationLayers;
    char *name;
    int height;
    int width;
    int targetSwapchainImageCount;
    VkPresentModeKHR targetPresentMode;
    uint32_t maxCommandBufferListCount;
    uint32_t maxVkRenderPassCount;
    uint32_t tknGraphicPipelineHashSize;

    // Runtime
    GLFWwindow *pGLFWWindow;
    VkInstance vkInstance;
    VkSurfaceKHR vkSurface;
    VkPhysicalDevice vkPhysicalDevice;
    uint32_t queueFamilyPropertyCount;
    uint32_t graphicQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex;
    VkDevice vkDevice;
    VkQueue vkGraphicQueue;
    VkQueue vkPresentQueue;
    VkSwapchainKHR vkSwapchain;
    VkSurfaceFormatKHR surfaceFormat;
    VkExtent2D swapchainExtent;
    uint32_t swapchainImageCount;
    VkImage *swapchainImages;
    VkImageView *swapchainImageViews;
    VkImage depthImage;
    VkFormat depthFormat;
    VkImageView depthImageView;
    VkDeviceMemory depthImageMemory;
    VkCommandPool *vkCommandPools;
    uint32_t submitVkCommandBufferCount;
    VkCommandBuffer *submitVkCommandBuffers;
    uint32_t acquiredImageIndex;
    VkSemaphore *imageAvailableSemaphores;
    VkSemaphore *renderFinishedSemaphores;
    VkFence *renderFinishedFences;
    uint32_t frameCount;
    uint32_t frameIndex;
    bool hasRecreateSwapchain;
    uint32_t vkRenderPassCount;
    VkRenderPass *vkRenderPasses;
    VkRenderPassCreateInfo *vkRenderPassCreateInfos;
    VkRenderPassCreateInfoPtrNode *vkRenderPassCreateInfoPtrNodes;

    GraphicPipelineNode* graphicPipelineNodes;
} GFXEngine;

void StartGFXEngine(GFXEngine *pGFXEngine);
void UpdateGFXEngine(GFXEngine *pGFXEngine);
void EndGFXEngine(GFXEngine *pGFXEngine);

void AddTKNGraphicPipeline(GFXEngine *pGFXEngine, VkGraphicsPipelineCreateConfig tknGraphicPipelineConfig);
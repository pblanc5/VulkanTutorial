#include "first_app.hpp"

// STD
#include <stdexcept>
#include <memory>
#include <array>
#include <iostream>

namespace lve {

    FirstApp::FirstApp() {
        std::cout << "Starting App...\n";
        loadModels();
        createPipelineLayout();
        createPipeline();
        createCommandBuffers();
    }

    FirstApp::~FirstApp() {
        vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
    }

    void FirstApp::run() {
        while (!lveWindow.shouldClose()) {
            glfwPollEvents();
            drawFrame();
        }

        vkDeviceWaitIdle(lveDevice.device());
    }

    void FirstApp::loadModels() {
        
        std::vector<LveModel::Vertex> vertices = {
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        };

        // Optional Challenge
        //seirpinskiSieve(-1.0, 1, 2.0f, 1, vertices);
        lveModel = std::make_unique<LveModel>(lveDevice, vertices);
    }

    void FirstApp::createPipelineLayout() {
        std::cout << "Creating Pipeline Layout...\n";
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout");
        } 
    }

    void FirstApp::createPipeline() {
        std::cout << "Creating Pipeline...\n";
        auto pipelineConfig = 
            LvePipeline::defaultPipelineConfigInfo(lveSwapChain.width(), lveSwapChain.height());
        pipelineConfig.renderPass = lveSwapChain.getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        lvePipeline = std::make_unique<LvePipeline>(
            lveDevice,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig
        );

        std::cout << "End Create Pipeline...\n";
    }

    void FirstApp::createCommandBuffers() {
        std::cout << "Creating Command Buffer...\n";
        commandBuffers.resize(lveSwapChain.imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = lveDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(lveDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers");
        }

        for (int i = 0; i < commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("Failed to begin recording command buffer: " + i);
            }

            // Render Pass Init
            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = lveSwapChain.getRenderPass();
            renderPassInfo.framebuffer = lveSwapChain.getFrameBuffer(i);

            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = lveSwapChain.getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
            clearValues[1].depthStencil = {1.0f, 0};
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            lvePipeline->bind(commandBuffers[i]);
            lveModel->bind(commandBuffers[i]);
            lveModel->draw(commandBuffers[i]);

            vkCmdEndRenderPass(commandBuffers[i]);
            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to record command buffer");
            }
        }
    }

    void FirstApp::drawFrame() {
        uint32_t imageIndex;
        auto result = lveSwapChain.acquireNextImage(&imageIndex);

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to aquire swap chain image");
        }

        result = lveSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
    }

    void FirstApp::seirpinskiSieve(float x, float y, float length, uint32_t iter, std::vector<LveModel::Vertex> &vertices) {
        // Write triangle vertices when max level of recurision is reached;
        if (iter == 5) {
            vertices.push_back({{x, y}, {1.0f, 0.0f, 0.0f}});
            vertices.push_back({{x + length/2, y - glm::sin(M_PI/3) * length}, {0.0f, 1.0f, 0.0f}});
            vertices.push_back({{x + length, y}, {0.0f, 0.0f, 1.0f}});
        } else {
            iter = iter + 1;
            seirpinskiSieve(x, y, length/2, iter, vertices);
            seirpinskiSieve(x + length/2, y, length/2, iter, vertices);
            seirpinskiSieve(x + length/4, y - glm::sin(M_PI/3) * length/2, length/2, iter, vertices);
        }
    }
}
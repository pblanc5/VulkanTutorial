#include "first_app.hpp"

// Libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// Std
#include <stdexcept>
#include <memory>
#include <array>
#include <iostream>

namespace lve {

    struct SimplePushConstantData {
        glm::mat2 transform{1.0f};
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    FirstApp::FirstApp() {
        std::cout << "Starting App...\n";
        loadGameObjects();
        createPipelineLayout();
        createPipeline();
    }

    FirstApp::~FirstApp() {
        vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
    }

    void FirstApp::run() {
        while (!lveWindow.shouldClose()) {
            glfwPollEvents();

            if (auto commandBuffer = lveRenderer.beginFrame()) {
                lveRenderer.beginSwapChainRenderPass(commandBuffer);
                renderGameObjects(commandBuffer);
                lveRenderer.endSwapChainRenderPass(commandBuffer);
                lveRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(lveDevice.device());
    }

    void FirstApp::loadGameObjects() {
        
        std::vector<LveModel::Vertex> vertices = {
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        };

        // Optional Challenge
        //seirpinskiSieve(-1.0, 1, 2.0f, 1, vertices);
        auto lveModel = std::make_shared<LveModel>(lveDevice, vertices);
        auto triangle = LveGameObject::createGameObject();
        triangle.model = lveModel;
        triangle.color = {.1f, .8f, .1f};
        triangle.transform2d.translation.x = .2f;
        triangle.transform2d.scale = {2.f, .5f};
        triangle.transform2d.rotation = .25f * glm::two_pi<float>();

        gameObjects.push_back(std::move(triangle));
    }

    void FirstApp::createPipelineLayout() {
        std::cout << "Creating Pipeline Layout...\n";

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout");
        } 
    }

    void FirstApp::createPipeline() {
        std::cout << "Creating Pipeline...\n";
        assert(pipelineLayout != nullptr && "Cannot crate pipeline before pipeline layout!");

        PipelineConfigInfo pipelineConfig{};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = lveRenderer.getSwapChainRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        lvePipeline = std::make_unique<LvePipeline>(
            lveDevice,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig
        );

        std::cout << "End Create Pipeline...\n";
    }

    void FirstApp::renderGameObjects(VkCommandBuffer commandBuffer) {
        lvePipeline->bind(commandBuffer);

        for (auto& object: gameObjects) {

            object.transform2d.rotation = glm::mod(object.transform2d.rotation + 0.001f, glm::two_pi<float>());

            SimplePushConstantData push{};
            push.offset = object.transform2d.translation;
            push.color = object.color;
            push.transform = object.transform2d.mat2();

            vkCmdPushConstants(
                commandBuffer,
                pipelineLayout, 
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push);

            object.model->bind(commandBuffer);
            object.model->draw(commandBuffer);
        }
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
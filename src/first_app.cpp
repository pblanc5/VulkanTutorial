#include "first_app.hpp"
#include "simple_render_system.hpp"

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

    FirstApp::FirstApp() {
        std::cout << "Starting App...\n";
        loadGameObjects();
    }

    FirstApp::~FirstApp() {}

    void FirstApp::run() {
        SimpleRenderSystem simpleRenderSystem{lveDevice, lveRenderer.getSwapChainRenderPass()};

        while (!lveWindow.shouldClose()) {
            glfwPollEvents();

            if (auto commandBuffer = lveRenderer.beginFrame()) {
                lveRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
                lveRenderer.endSwapChainRenderPass(commandBuffer);
                lveRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(lveDevice.device());
    }

    void FirstApp::loadGameObjects() {
        
        std::vector<LveModel::Vertex> vertices = {
            //{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            //{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            //{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        };

        // Optional Challenge
        seirpinskiSieve(-1.0, 1, 2.0f, 1, vertices);
        auto lveModel = std::make_shared<LveModel>(lveDevice, vertices);
        auto triangle = LveGameObject::createGameObject();
        triangle.model = lveModel;
        triangle.color = {.1f, .8f, .1f};
        triangle.transform2d.translation.x = .2f;
        triangle.transform2d.scale = {1.f, 1.f};
        triangle.transform2d.rotation = .25f * glm::two_pi<float>();

        gameObjects.push_back(std::move(triangle));
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
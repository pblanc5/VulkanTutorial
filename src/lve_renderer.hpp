#pragma once

#include "lve_window.hpp"
#include "lve_device.hpp"
#include "lve_swap_chain.hpp"

// STD
#include <cassert>
#include <memory>
#include <vector>

namespace lve {
    class LveRenderer {
        public:
            LveRenderer(LveWindow &window, LveDevice &device);
            ~LveRenderer();

            LveRenderer(const LveRenderer &) = delete;
            LveRenderer &operator=(const LveRenderer &) = delete;

            VkRenderPass getSwapChainRenderPass() const {return lveSwapChain->getRenderPass();}
            bool isFrameInProgress() const { return isFrameStarted; }

            VkCommandBuffer getCurrentCommandBuffer() const {
                assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
                return commandBuffers[currentImageIndex];
            }

            VkCommandBuffer beginFrame();
            void endFrame();
            void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
            void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

        private:
            void createCommandBuffers();
            void freeCommandBuffers();
            void drawFrame();
            void recreateSwapChain();

            LveWindow& lveWindow;
            LveDevice& lveDevice;
            std::unique_ptr<LveSwapChain> lveSwapChain;
            std::vector<VkCommandBuffer> commandBuffers;

            uint32_t currentImageIndex;
            bool isFrameStarted{false};
    };
}
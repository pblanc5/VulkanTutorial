#pragma once

#include "lve_model.hpp"

// std
#include <memory>

namespace lve {

    struct Transform2dComponent {
        glm::vec2 translation;
        glm::mat2 mat2() {return glm::mat2{1.f};}
    };

    class LveGameObject {
        public:
            using id_t = unsigned int;
            static LveGameObject createGameObject() {
                static id_t currentId = 0;
                return LveGameObject(currentId++);
            }

            LveGameObject(const LveGameObject &) = delete;
            LveGameObject &operator=(const LveGameObject &) = delete;
            LveGameObject(LveGameObject &&) = default;
            LveGameObject &operator=(LveGameObject &&) = default;

            id_t getId() {
                return id;
            }

            // Properties
            std::shared_ptr<LveModel> model;
            glm::vec3 color;
            Transform2dComponent transform2d{};

        private:
            LveGameObject(id_t objectId) : id{objectId} {}
            id_t id;
    };
}
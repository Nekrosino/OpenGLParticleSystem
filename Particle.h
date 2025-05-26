#pragma once
#include <glm/glm.hpp>

class Particle {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    float lifetime;

    Particle(glm::vec3 pos, glm::vec3 vel, float life)
        : position(pos), velocity(vel), lifetime(life) {
    }

    void update(float dt) {
        position += velocity * dt;
        lifetime -= dt;
    }

    bool isDead() const {
        return lifetime <= 0.0f;
    }
};

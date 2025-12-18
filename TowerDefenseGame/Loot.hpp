#pragma once
#include "Accessory.hpp"
#include "Config.hpp"
#include "Entity.hpp"
#include "Utils.hpp"

class Loot : public Entity {
   public:
    Loot(sf::Vector2f startPos, AccessoryType type) : m_type(type) {
        m_shape.setRadius(10.f);
        m_shape.setOrigin({10.f, 10.f});
        m_shape.setPosition(startPos);

        auto data = AccessoryData::get(type);
        m_shape.setFillColor(data.color);
        m_shape.setOutlineColor(sf::Color::White);
        m_shape.setOutlineThickness(2.f);

        m_targetPos = {static_cast<float>(Config::WINDOW_WIDTH - 40), 40.f};
    }

    void update(sf::Time dt) override {
        sf::Vector2f dir = m_targetPos - m_shape.getPosition();
        float dist = Utils::length(dir);

        float speed = 600.f;
        if (dist < 10.f) {
            m_arrived = true;
            destroy();
        } else {
            m_shape.move(Utils::normalize(dir) * speed * dt.asSeconds());
        }
    }

    void draw(sf::RenderWindow& window) override {
        window.draw(m_shape);
    }

    sf::Vector2f getPosition() const override {
        return m_shape.getPosition();
    }

    bool hasArrived() const { return m_arrived; }
    AccessoryType getType() const { return m_type; }

   private:
    sf::CircleShape m_shape;
    sf::Vector2f m_targetPos;
    AccessoryType m_type;
    bool m_arrived = false;
};
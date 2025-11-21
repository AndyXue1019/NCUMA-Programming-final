#pragma once
#include <SFML/Graphics.hpp>

class Entity {
   public:
    virtual ~Entity() = default;

    // 純虛擬函式，強制子類別實作
    virtual void update(sf::Time dt) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual sf::Vector2f getPosition() const = 0;

    // 標記是否該被移除 (例如敵人死亡或到達終點)
    bool isActive() const { return m_active; }
    void destroy() { m_active = false; }

   protected:
    bool m_active = true;
};
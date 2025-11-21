#pragma once
#include <vector>

#include "Entity.hpp"

// 定義敵人類型
enum class EnemyType {
    Normal,
    Fast,
    Tank,
    MiniBoss,  // Wave 5, 10, 15
    FinalBoss  // Wave 20
};

class Enemy : public Entity {
   public:
    Enemy(const std::vector<sf::Vector2f>& path, EnemyType type);

    void update(sf::Time dt) override;
    void draw(sf::RenderWindow& window) override;
    sf::Vector2f getPosition() const override;
    void takeDamage(int damage);

    // Getters
    bool isDead() const { return m_hp <= 0; }
    int getBounty() const { return m_bounty; }  // 掉落金幣
    int getExp() const { return m_expReward; }  // 掉落經驗

    void applySlow(float factor, float duration);
    void teleportBack(int nodes);

   private:
    sf::CircleShape m_shape;
    std::vector<sf::Vector2f> m_path;
    std::size_t m_currentWaypointIndex = 0;

    float m_speed = 100.f;
    int m_maxHp = 50;
    int m_hp = 50;
    int m_bounty = 10;    // 金幣獎勵
    int m_expReward = 5;  // 經驗獎勵
    float m_baseSpeed;    // 紀錄原始速度
    float m_currentSpeed;
    float m_slowTimer = 0.f;
    bool m_isSlowed = false;
};
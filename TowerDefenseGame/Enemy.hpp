#pragma once
#include <vector>

#include "Entity.hpp"

enum class EnemyType {
    Normal,
    Fast,
    Tank,
    MiniBoss,
    FinalBoss,
    Triangle,
    Square
};

class Enemy : public Entity {
   public:
    Enemy(const std::vector<sf::Vector2f>& path, EnemyType type, bool isGamblerMode = false);

    void update(sf::Time dt) override;
    void draw(sf::RenderWindow& window) override;
    sf::Vector2f getPosition() const override;
    void takeDamage(int damage);

    std::size_t getWaypointIndex() const { return m_currentWaypointIndex; }
    sf::Vector2f getNextWaypointPos() const {
        if (m_currentWaypointIndex < m_path.size()) {
            return m_path[m_currentWaypointIndex];
        }
        return getPosition();  // 防呆：如果已經走完，回傳自身位置
    }

    void applyBurn(float duration);
    void applyStun(float duration);
    void applySlow(float factor, float duration);
    void teleportBack(int nodes);

    bool isDead() const { return m_hp <= 0; }
    int getBounty() const { return m_bounty; }
    int getExp() const { return m_expReward; }

   private:
    sf::Color m_color = sf::Color::Red;
    sf::CircleShape m_shape;
    std::vector<sf::Vector2f> m_path;
    std::size_t m_currentWaypointIndex = 0;

    EnemyType m_type;

    float m_speed = 100.f;
    int m_maxHp = 50;
    int m_hp = 50;
    int m_bounty = 10;
    int m_expReward = 5;
    float m_baseSpeed;
    float m_currentSpeed;

    // 狀態計時器
    float m_slowTimer = 0.f;
    bool m_isSlowed = false;

    bool m_isBurning = false;
    float m_burnTimer = 0.f;
    float m_burnTickTimer = 0.f;

    bool m_isStunned = false;
    float m_stunTimer = 0.f;

    bool m_isDashing = false;         // 是否正在衝刺
    float m_dashDurationTimer = 0.f;  // 衝刺持續時間 (0.5s)
    float m_dashCheckTimer = 0.f;     // 每隔多久檢查一次是否要衝刺
};
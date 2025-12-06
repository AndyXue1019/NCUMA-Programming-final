#include "Enemy.hpp"
#include "Utils.hpp"

Enemy::Enemy(const std::vector<sf::Vector2f>& path, EnemyType type)
    : m_path(path) {
    // 根據類型設定數值與外觀
    switch (type) {
    case EnemyType::Normal:
        m_speed = 100.f;
        m_maxHp = 50;
        m_bounty = 10;
        m_expReward = 5;
        m_shape.setRadius(15.f);
        m_shape.setFillColor(sf::Color::Red);
        break;
    case EnemyType::Fast:
        m_speed = 150.f;
        m_maxHp = 30;
        m_bounty = 15;
        m_expReward = 8;
        m_shape.setRadius(12.f);
        m_shape.setFillColor(sf::Color::Yellow);
        break;
    case EnemyType::Tank:
        m_speed = 70.f;
        m_maxHp = 150;
        m_bounty = 20;
        m_expReward = 15;
        m_shape.setRadius(20.f);
        m_shape.setFillColor(sf::Color(150, 0, 0));  // 深紅
        break;
    case EnemyType::MiniBoss:  // Wave 5, 10, 15
        m_speed = 60.f;
        m_maxHp = 500;
        m_bounty = 100;
        m_expReward = 100;
        m_shape.setRadius(30.f);
        m_shape.setFillColor(sf::Color::Magenta);
        break;
    case EnemyType::FinalBoss:  // Wave 20
        m_speed = 40.f;
        m_maxHp = 5000;
        m_bounty = 1000;
        m_expReward = 1000;
        m_shape.setRadius(50.f);
        m_shape.setFillColor(sf::Color::Cyan);
        break;
    }
    m_baseSpeed = m_speed;
    m_currentSpeed = m_speed;
    m_hp = m_maxHp;
    m_shape.setOrigin({ m_shape.getRadius(), m_shape.getRadius() }); // SFML 3 寫法

    if (!m_path.empty()) m_shape.setPosition(m_path[0]);
}

void Enemy::update(sf::Time dt) {
    // 檢查是否到達終點
    if (m_path.empty() || m_currentWaypointIndex >= m_path.size()) {
        destroy();  // 到達終點
        return;
    }

    // --- 1. 更新狀態計時器 (Status Timers) ---

    // (A) 暈眩計時
    if (m_isStunned) {
        m_stunTimer -= dt.asSeconds();
        if (m_stunTimer <= 0.f) {
            m_isStunned = false;
        }
    }

    // (B) 緩速計時
    if (m_isSlowed) {
        m_slowTimer -= dt.asSeconds();
        if (m_slowTimer <= 0.f) {
            m_isSlowed = false;
            m_currentSpeed = m_baseSpeed; // 恢復速度
        }
    }

    // (C) 灼燒計時與傷害
    if (m_isBurning) {
        m_burnTimer -= dt.asSeconds();
        m_burnTickTimer += dt.asSeconds();

        // 每 1 秒扣 3% 最大血量
        if (m_burnTickTimer >= 1.0f) {
            m_burnTickTimer = 0.f;
            int damage = static_cast<int>(m_maxHp * 0.03f);
            if (damage < 1) damage = 1; // 至少扣 1 滴
            takeDamage(damage);
        }

        if (m_burnTimer <= 0.f) {
            m_isBurning = false;
        }
    }

    // --- 2. 移動邏輯 ---
    // 只有在「沒有暈眩」的時候才移動
    if (!m_isStunned) {
        sf::Vector2f target = m_path[m_currentWaypointIndex];
        sf::Vector2f current = m_shape.getPosition();
        sf::Vector2f dir = target - current;

        if (Utils::length(dir) < 5.f) {
            m_currentWaypointIndex++;
        }
        else {
            m_shape.move(Utils::normalize(dir) * m_currentSpeed * dt.asSeconds());
        }
    }

    // --- 3. 視覺顏色更新 (優先級：暈眩 > 灼燒 > 緩速 > 正常) ---
    if (m_isStunned) {
        m_shape.setFillColor(sf::Color::Blue);
    }
    else if (m_isBurning) {
        m_shape.setFillColor(sf::Color(255, 69, 0)); // 橘紅
    }
    else if (m_isSlowed) {
        m_shape.setFillColor(sf::Color(0, 255, 255)); // 青色
    }
    else {
        // 恢復原本顏色 (注意：這裡暫時設為紅色，如果你有不同顏色的敵人，
        // 最好在 Enemy.hpp 加一個 m_originalColor 來記錄並在這裡恢復)
        m_shape.setFillColor(sf::Color::Red);
    }

    // --- 4. 死亡檢查 ---
    if (m_hp <= 0) destroy();
}

void Enemy::applySlow(float factor, float duration) {
    m_isSlowed = true;
    m_currentSpeed = m_baseSpeed * factor;  // 例如 0.5 倍速
    m_slowTimer = duration;
}

void Enemy::applyBurn(float duration) {
    m_isBurning = true;
    m_burnTimer = duration;
    // 不重置 m_burnTickTimer，讓傷害節奏保持連續，或是你可以選擇 m_burnTickTimer = 0.9f 讓它快點觸發
}

// [新增] 暈眩實作
void Enemy::applyStun(float duration) {
    m_isStunned = true;
    m_stunTimer = duration;
}

void Enemy::teleportBack(int nodes) {
    // 往回退 nodes 個節點，但不能小於 0
    if (m_currentWaypointIndex >= static_cast<size_t>(nodes)) {
        m_currentWaypointIndex -= nodes;
    }
    else {
        m_currentWaypointIndex = 0;
    }
    // 瞬間移動到該節點位置
    if (m_currentWaypointIndex < m_path.size()) {
        m_shape.setPosition(m_path[m_currentWaypointIndex]);
    }
}

void Enemy::draw(sf::RenderWindow& window) {
    window.draw(m_shape);
}

sf::Vector2f Enemy::getPosition() const {
    return m_shape.getPosition();
}

void Enemy::takeDamage(int damage) {
    m_hp -= damage;
}
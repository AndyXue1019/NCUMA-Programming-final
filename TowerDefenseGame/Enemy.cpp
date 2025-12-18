#include "Enemy.hpp"

#include "Utils.hpp"

Enemy::Enemy(const std::vector<sf::Vector2f>& path, EnemyType type, bool isGamblerMode)
    : m_path(path), m_type(type) {
    m_shape.setPointCount(30);

    switch (type) {
        case EnemyType::Normal:
            m_speed = 100.f;
            m_maxHp = 50;
            m_bounty = 10;
            m_expReward = 5;
            m_shape.setRadius(15.f);
            m_shape.setFillColor(m_color);
            break;
        case EnemyType::Fast:
            m_speed = 150.f;
            m_maxHp = 30;
            m_bounty = 15;
            m_expReward = 8;
            m_color = sf::Color::Yellow;
            m_shape.setRadius(12.f);
            m_shape.setFillColor(m_color);
            break;
        case EnemyType::Tank:
            m_speed = 70.f;
            m_maxHp = 150;
            m_bounty = 20;
            m_expReward = 15;
            m_color = sf::Color(150, 0, 0);  // 深紅
            m_shape.setRadius(20.f);
            m_shape.setFillColor(m_color);
            break;
        case EnemyType::Triangle:
            m_speed = 100.f * 1.5f;                  // 普通敵人的 150% 速度
            m_maxHp = static_cast<int>(50 * 0.75f);  // 普通敵人的 75% 血量 (~37)
            m_bounty = 12;
            m_expReward = 6;
            m_color = sf::Color(0, 255, 100);  // 亮綠色
            m_shape.setRadius(15.f);
            m_shape.setPointCount(3);  // 設定為三角形
            m_shape.setFillColor(m_color);
            break;
        case EnemyType::Square:
            m_speed = 100.f * 0.5f;  // 普通敵人的 50% 速度
            m_maxHp = 50;
            m_bounty = 15;
            m_expReward = 8;
            m_color = sf::Color(100, 100, 255);  // 藍紫色
            m_shape.setRadius(18.f);
            m_shape.setPointCount(4);  // 設定為正方形
            m_shape.setFillColor(m_color);
            break;
        case EnemyType::MiniBoss:  // Wave 5, 10, 15
            m_speed = 60.f;
            m_maxHp = 500;
            m_bounty = 100;
            m_expReward = 100;
            m_color = sf::Color::Magenta;
            m_shape.setRadius(30.f);
            m_shape.setFillColor(m_color);
            break;
        case EnemyType::FinalBoss:  // Wave 20
            m_speed = 40.f;
            m_maxHp = 5000;
            m_bounty = 1000;
            m_expReward = 1000;
            m_color = sf::Color::Cyan;
            m_shape.setRadius(50.f);
            m_shape.setFillColor(m_color);
            break;
    }

    if (isGamblerMode) {
        m_speed *= 5.0f;
        m_maxHp *= 10;
        m_bounty /= 2;
        m_expReward /= 2;
    }

    m_baseSpeed = m_speed;
    m_currentSpeed = m_speed;
    m_hp = m_maxHp;
    m_shape.setOrigin({m_shape.getRadius(), m_shape.getRadius()});

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
            m_currentSpeed = m_baseSpeed;  // 恢復速度
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
            if (damage < 1) damage = 1;  // 至少扣 1 滴
            takeDamage(damage);
        }

        if (m_burnTimer <= 0.f) {
            m_isBurning = false;
        }
    }

    if (m_type == EnemyType::Square && !m_isStunned) {  // 暈眩時不能衝刺
        if (m_isDashing) {
            // 正在衝刺中 倒數計時
            m_dashDurationTimer -= dt.asSeconds();
            if (m_dashDurationTimer <= 0.f) {
                // 衝刺結束，恢復原本速度 (考慮緩速狀態)
                m_isDashing = false;
                if (m_isSlowed) {
                    // 如果原本被緩速，恢復成緩速後的速度
                    m_currentSpeed = m_baseSpeed * 0.5f;  // 假設緩速是 0.5倍
                } else {
                    m_currentSpeed = m_baseSpeed;
                }
            }
        } else {
            // 沒在衝刺：冷卻並檢查機率
            m_dashCheckTimer -= dt.asSeconds();
            if (m_dashCheckTimer <= 0.f) {
                // 每 2 秒檢查一次是否要衝刺
                m_dashCheckTimer = 2.0f;

                // 50% 機率
                if (Utils::m_rand() < 50) {
                    m_isDashing = true;
                    m_dashDurationTimer = 0.5f;  // 衝刺持續 0.5 秒
                    m_currentSpeed = 500.f;      // 速度變為 500
                }
            }
        }
    }

    // --- 2. 移動邏輯 ---
    // 只有在"沒有暈眩"的時候才移動
    if (!m_isStunned) {
        sf::Vector2f target = m_path[m_currentWaypointIndex];
        sf::Vector2f current = m_shape.getPosition();
        sf::Vector2f dir = target - current;

        if (Utils::length(dir) < 5.f) {
            m_currentWaypointIndex++;
        } else {
            m_shape.move(Utils::normalize(dir) * m_currentSpeed * dt.asSeconds());
        }
    }

    // --- 3. 視覺顏色更新 (優先級：暈眩 > 灼燒 > 緩速 > 正常) ---
    if (m_isStunned) {
        m_shape.setFillColor(sf::Color::Blue);
    } else if (m_isBurning) {
        m_shape.setFillColor(sf::Color(255, 69, 0));  // 橘紅
    } else if (m_isSlowed) {
        m_shape.setFillColor(sf::Color(0, 255, 255));  // 青色
    } else {
        // 恢復原本顏色
        m_shape.setFillColor(m_color);
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
    // 不重置 m_burnTickTimer，讓傷害節奏保持連續
    // 可以選擇 m_burnTickTimer = 0.9f 讓它快點觸發
}

void Enemy::applyStun(float duration) {
    m_isStunned = true;
    m_stunTimer = duration;
}

void Enemy::teleportBack(int nodes) {
    // 往回退 nodes 個節點，但不能小於 0
    if (m_currentWaypointIndex >= static_cast<size_t>(nodes)) {
        m_currentWaypointIndex -= nodes;
    } else {
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
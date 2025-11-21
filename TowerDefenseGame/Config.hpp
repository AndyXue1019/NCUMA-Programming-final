#pragma once
#include <SFML/System/Vector2.hpp>

namespace Config {
// 視窗設定
constexpr unsigned int WINDOW_WIDTH = 1280;
constexpr unsigned int WINDOW_HEIGHT = 720;
constexpr unsigned int FRAME_RATE_LIMIT = 60;

// 地圖網格設定 (Slot-based map system
constexpr float GRID_SIZE = 64.0f;  // 每個格子 64x64 pixel

// 計算網格數量
constexpr int COLUMNS = WINDOW_WIDTH / static_cast<int>(GRID_SIZE);
constexpr int ROWS = WINDOW_HEIGHT / static_cast<int>(GRID_SIZE);
}  // namespace Config
#pragma once
#include "KamataEngine.h"

// ゲームシーン
class GameScene {
private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

public:
	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();
};

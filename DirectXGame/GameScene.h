#pragma once
#include "KamataEngine.h"
#include "Player.h"

// KamataEngineの名前空間を使用
using namespace KamataEngine;

// ゲームシーン
class GameScene {
private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
	// 3Dモデル
	Model* model_ = nullptr;
	// カメラ
	Camera* camera_ = nullptr;
	// 自キャラ
	Player* player_ = nullptr;

public:
	// デストラクタ
	~GameScene();
	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();
}

#pragma once
#include"KamataEngine.h"

//ゲームシーン
class GameScene {
private:
	// 3Dモデル
	KamataEngine::Model* modelBlock_;

public:
	//初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();
};

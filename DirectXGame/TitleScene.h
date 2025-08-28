#pragma once
#include "TitleScene.h"
#include "KamataEngine.h"
#include "Fade.h"
#include "Skydome.h"

// KamataEngineの名前空間を使用
using namespace KamataEngine;

class TitleScene {
private:
	// シーンのフェーズ
	enum class Phase {
		kFadeIn,  // フェードイン
		kMain,    // メイン部
		kFadeOut, // フェードアウト
	};
	// 現在のフェーズ
	Phase phase_ = Phase::kFadeIn;

	// 終了フラグ
	bool finished_ = false;
	// フェード
	Fade* fade_ = nullptr;
	// フェード時間
	static inline const int fadeTime = 1;
	// テクスチャハンドル
	uint32_t textureHandleTitle_ = 0;
	Sprite* title_ = nullptr;
	// カメラ
	KamataEngine::Camera camera_;
	// 天球
	Skydome* skydome_ = nullptr;
	KamataEngine::Model* modelSkydome_ = nullptr;

public:
	~TitleScene();
	void Initialize();
	void Update();
	void Draw();
	// 終了フラグのgetter
	bool GetFinished() const { return finished_; };
};

#pragma once
#include "TitleScene.h"
#include "KamataEngine.h"
#include "Fade.h"

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

public:
	~TitleScene();
	void Initialize();
	void Update();
	void Draw();
	// 終了フラグのgetter
	bool GetFinished() const { return finished_; };
};

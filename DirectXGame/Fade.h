#pragma once
#include "KamataEngine.h"

// KamataEngineの名前空間を使用
using namespace KamataEngine;

class Fade {
public:
	// フェードの状態
	enum class Status {
		None,    // フェードなし
		FadeIn,  // フェードイン
		FadeOut, // フェードアウト
	};
	
	void Initialize();
	void Update();
	void Draw();
	// フェード開始
	void Start(Status status, float duration);
	// フェード終了
	void Stop();
	// フェード終了判定
	bool IsFinished() const;

private:
	Sprite* sprite_ = nullptr;
	// 現在のフェードの状態
	Status status_ = Status::None;
	// フェードの持続時間
	float duration_ = 0.0f;
	// 経過時間カウンター
	float counter_ = 0.0f;
};

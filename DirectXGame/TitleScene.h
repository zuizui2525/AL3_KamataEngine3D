#pragma once
#include "TitleScene.h"
#include "KamataEngine.h"

// KamataEngineの名前空間を使用
using namespace KamataEngine;

class TitleScene {
private:
	// 終了フラグ
	bool finished_ = false;
public:
	void Initialize();
	void Update();
	void Draw();
	// 終了フラグのgetter
	bool GetFinished() const { return finished_; };
};

#pragma once
#include <memory>
#include <iostream>
#include "KamataEngine.h"

/// <summary>
/// 天球
/// </summary>
class Skydome {
private:
	// ワールド変換データ
	std::shared_ptr<KamataEngine::WorldTransform> worldTransform_;
	// モデル
	std::shared_ptr<KamataEngine::Model> model_ = nullptr;
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
};

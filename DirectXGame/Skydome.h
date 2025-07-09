#pragma once
#include <memory>
#include <iostream>
#include "KamataEngine.h"

/// <summary>
/// 天球
/// </summary>
class Skydome {
private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
	// モデル
	KamataEngine::Model* model_ = nullptr;
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
};

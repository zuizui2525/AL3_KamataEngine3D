#pragma once
#include <array>
#define _USE_MATH_DEFINES
#include <Math.h>
#include "KamataEngine.h"

// KamataEngineの名前空間を使用
using namespace KamataEngine;

class DeathParticle {
private:
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// カメラ
	Camera* camera_ = nullptr;
	// パーティクルの個数
	static inline const uint32_t kNumParticles = 8;
	std::array<WorldTransform, kNumParticles> worldTransforms_;
	// 存在時間(消滅までの時間)<秒>
	static inline const float kDuration = 2.0f;
	// 移動の速さ
	static inline const float kSpeed = 0.05f;
	// 分割した1個分の角度
	static inline const float kAngleUnit = 2.0f * static_cast<float>(M_PI) / kNumParticles;
	// 終了フラグ
	bool isFinished_ = false;
	// 経過時間カウント
	float counter_ = 0.0f;
	// 色変更オブジェクト
	ObjectColor objectColor_;
	// 色の数値
	Vector4 color_;

public:
	void Initialize(Model* model, Camera* camera, const Vector3& position);
	void Update();
	void Draw();
	bool GetIsFinished() const { return isFinished_; };
};

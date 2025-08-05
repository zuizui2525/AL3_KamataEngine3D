#pragma once
#include "KamataEngine.h"
#include "struct.h"

// KamataEngineの名前空間を使用
using namespace KamataEngine;

class Player;

class Enemy {
private:
	enum class LRDirection {
		kRight,
		kLeft,
	};
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// カメラ
	Camera* camera_ = nullptr;
	//足の速さ
	static inline const float kWalkSpeed = 0.05f;
	// 速度
	Vector3 velocity_ = {};
	// 最初の角度
	static inline const float kWalkMotionAngleStart = -15.0f;
	// 最後の角度
	static inline const float kWalkMotionAngleEnd = 30.0f;
	// アニメーションの周期となる時間[秒]
	static inline const float kWalkMotionTime = 0.5f;
	// 経過時間
	float walkTimer_ = 0.0f;
	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 1.8f;
	static inline const float kHeight = 1.8f;

public:
	void Initialize(Model* model, Camera* camera, const Vector3& position);
	void Update();
	void Draw();
	Vector3 GetWorldPosition();
	AABB GetAABB();
	// 衝突判定
	void OnCollision(const Player* player);
};

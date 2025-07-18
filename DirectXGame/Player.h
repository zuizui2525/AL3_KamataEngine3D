#pragma once  
#include "KamataEngine.h"  

// KamataEngineの名前空間を使用
using namespace KamataEngine;

enum class LRDirection {
	kRight,
	kLeft,
};

/// <summary>  
/// 自キャラ  
/// </summary>  
class Player {
private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// カメラ
	Camera* camera_ = nullptr;
	// 速度
	Vector3 velocity_ = {};
	// 固定の加速度
	static inline const float kAcceleration = 0.01f;
	// 固定の減衰抵抗力
	static inline const float kAttenuation = 0.08f;
	// 最大速度制限
	static inline const float kLimitRunSpeed = 0.3f;
	// 向き
	LRDirection lrDirection_ = LRDirection::kRight;
	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;
	// 旋回時間<秒>
	static inline const float kTimeTurn = 0.3f;
	// 設置状態フラグ
	bool onGround_ = true;
	// 重力加速度(下方向)
	static inline const float kGravityAccleration = 9.8f / 60.0f;
	// 最大落下速度(下方向)
	static inline const float kLimitFallSpeed = 1.1f;
	// ジャンプ初速(上方向)
	static inline const float kJumpAcceleration = 1.7f;
	// 接地フラグ
	bool landing = false;
public :
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, Camera* camera, const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
};
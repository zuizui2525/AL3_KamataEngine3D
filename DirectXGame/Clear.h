#pragma once
#include "Collision.h"
#include "KamataEngine.h"
#include "struct.h"

// KamataEngineの名前空間を使用
using namespace KamataEngine;

// クリアクラス（ゴールオブジェクト）
class Clear {
public:
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

	/// <summary>
	/// AABBを取得
	/// </summary>
	AABB GetAABB() const;

	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	Vector3 GetWorldPosition() const;

private:
	// ゴールの当たり判定サイズ
	static inline const float kWidth_ = 1.8f;
	static inline const float kHeight_ = 1.8f;

	// カメラ
	Camera* camera_ = nullptr;
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0u;
};
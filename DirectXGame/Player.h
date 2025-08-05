#pragma once  
#include "KamataEngine.h"  
#include "struct.h"

// KamataEngineの名前空間を使用
using namespace KamataEngine;

class MapChipField;
class Enemy;

/// <summary>  
/// 自キャラ  
/// </summary>  
class Player {
private:

	//// 構造体 ////
	
	// マップとの当たり判定情報
	struct CollisionMapInfo {
		bool ceilingCollisionFlag = false; // 天井衝突フラグ
		bool landingFlag = false;          // 着地フラグ
		bool wallContactFlag = false;      // 壁接触フラグ
		Vector3 velocity;                  // 移動量
	};

	//// 列挙型 ////
	enum class LRDirection {
		kRight,
		kLeft,
	};

	enum Corner {
		kRightBottom, // 右下
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上
		kNumCorner    // 要素数
	};

	//// 変数 ////
	
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
	static inline const float kAcceleration = 0.2f;
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
	static inline const float kLimitFallSpeed = 0.8f;
	// ジャンプ初速(上方向)
	static inline const float kJumpAcceleration = 1.8f;
	// 接地フラグ
	bool landing = false;
	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;
	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 1.8f;
	static inline const float kHeight = 1.8f;
	// 着地時の速度減衰率
	static inline const float kAttenuationLanding = 0.8f;
	// 着地時の速度減衰率
	static inline const float kAttenuationWall = 0.8f;
	// デスフラグ
	bool isDead_ = false;
	//// 関数 ////

	// 移動関係
	void HandleMoveInput();
	// マップ衝突判定
	void MapCollisionDetection(CollisionMapInfo& info);
	void MapCollisionDetectionUp(CollisionMapInfo& info);
	void MapCollisionDetectionDown(CollisionMapInfo& info);
	void MapCollisionDetectionLeft(CollisionMapInfo& info);
	void MapCollisionDetectionRight(CollisionMapInfo& info);
	// 指定した角の座標を得る
	Vector3 CornerPosition(const Vector3& center, Corner corner);
	// 判定結果を反映して移動させる
	void ActAccordingToTheOutcomeOfTheDecision(const CollisionMapInfo& info);
	// 天井に接触しているときの処理
	void touchingTheCeiling(const CollisionMapInfo& info);
	// 設置状態の切り替え処理
	void SwitchingInstallationStatus(const CollisionMapInfo& info);
	// 壁に接触している場合の処理
	void contactWithAWall(const CollisionMapInfo& info);

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

	WorldTransform& GetWorldTransform() { return worldTransform_; };
	Vector3& GetVelocity() { return velocity_; };
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; };
	Vector3 GetWorldPosition();
	AABB GetAABB();
	// 衝突判定
	void OnCollision(const Enemy* enemy);
	// デスフラグのgetter
	bool IsDead() const { return isDead_; };
};
#pragma once
#include "KamataEngine.h"

class Player;

class CameraController {
public:
	struct Rect {
		float left = 0.0f;   // 左端
		float right = 1.0f;  // 右端
		float bottom = 0.0f; // 下端
		float top = 1.0f;    // 上端
	};
private:
	// カメラ
	KamataEngine::Camera* camera_;
	// ターゲット
	Player* target_ = nullptr;
	// 追従対象とカメラの座標の差（オフセット）
	KamataEngine::Vector3 targetOffset_ = {0, 0, -30};
	// カメラ移動範囲
	Rect movableArea_ = {0, 100, 0, 100};
	// カメラの目標座標
	KamataEngine::Vector3 targetCoordinates_;
	// 座標補間割合
	static inline const float kInterpolationRate = 0.7f;
	// 速度掛け率
	static inline const float kVelocityBias = 0.2f;
	// 追従対象の各方向へのカメラ移動範囲
	static inline const Rect margin = {-10.0f, 10.0f, -10.0f, 10.0f};
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
	/// ターゲットとなるプレイヤーを設定します。
	/// </summary>
	/// <param name="target">設定するターゲットのPlayerオブジェクトへのポインタ。</param>
	void SetTarget(Player* target) { target_ = target; };
	/// <summary>
	/// カメラを設定します。
	/// </summary>
	/// <param name="camera">設定する Camera オブジェクトへのポインタ。</param>
	void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; };
	/// <summary>
	/// 移動可能な領域を設定します。
	/// </summary>
	/// <param name="area">設定する移動可能領域を表す Rect 型の値。</param>
	void SetMovableArea(Rect area) { movableArea_ = area; };
	/// <summary>
	/// オブジェクトの状態をリセットします。
	/// </summary>
	void Reset();
};

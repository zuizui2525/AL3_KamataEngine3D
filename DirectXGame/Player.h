#pragma once  
#include "KamataEngine.h"  

// KamataEngineの名前空間を使用
using namespace KamataEngine;

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

public:
	/// <summary>
	/// 初期化
	/// <param name="model">モデル</param>
	/// <param name="textureHandle">テクスチャハンドル</param>
	/// <param name="camera">カメラ</param>
	/// </summary>
	void Initialize(Model* model, uint32_t textureHandle, Camera* camera);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
};
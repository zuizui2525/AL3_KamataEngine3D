#pragma once  
#include <vector>  
#include "KamataEngine.h"
#include "Skydome.h"
#include "MapChipField.h"

// ゲームシーン  
class GameScene {  
private:  
	// テクスチャハンドル  
	uint32_t textureHandle_ = 0;  
	// 3Dモデル  
	KamataEngine::Model* modelBlock_;
	// 天球
	Skydome* skydome_;
	KamataEngine::Model* modelSkydome_;
	// カメラ
	KamataEngine::Camera camera_;
	// デバックカメラ
	KamataEngine::DebugCamera* debugCamera_;
	// デバックカメラ有効
	bool isDebugCameraActive_ = false;
	// ブロック用のワールドトランスフォーム 
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;  
	// マップチップフィールド
	MapChipField* mapChipField_;
public:  
	// デストラクタ  
	~GameScene();  
	// 初期化  
	void Initialize();  
	// 更新  
	void Update();  
	// 描画  
	void Draw();  
	// 表示ブロックの生成
	void GenerateBlocks();
};

#pragma once  
#include <vector>  
#include "KamataEngine.h"
#include "Skydome.h"
#include "MapChipField.h"
#include "Player.h"
#include "CameraController.h"

// ゲームシーン  
class GameScene {  
private:  
	// テクスチャハンドル  
	uint32_t textureHandle_ = 0;  
	// 3Dモデル  
	KamataEngine::Model* modelBlock_;
	// 天球
	Skydome* skydome_ = nullptr;
	KamataEngine::Model* modelSkydome_ = nullptr;
	// カメラ
	KamataEngine::Camera camera_;
	// デバックカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	// デバックカメラ有効
	bool isDebugCameraActive_ = false;
	// コントロールカメラ
	CameraController* cameraController_ = nullptr;
	// ブロック用のワールドトランスフォーム 
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;  
	// マップチップフィールド
	MapChipField* mapChipField_ = nullptr;
	// プレイヤー
	Player* player_ = nullptr;
	KamataEngine::Model* modelPlayer_ = nullptr;

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

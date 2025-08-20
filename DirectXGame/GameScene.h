#pragma once  
#include <vector>  
#include "KamataEngine.h"
#include "Skydome.h"
#include "MapChipField.h"
#include "Player.h"
#include "CameraController.h"
#include "Enemy.h"
#include "DeathParticle.h"
#include "Fade.h"

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
	// 敵
	std::list<Enemy*> enemies_;
	KamataEngine::Model* modelEnemy_ = nullptr;
	// デスパーティクル
	DeathParticle* deathParticles_ = nullptr;
	KamataEngine::Model* modelDeathParticle_ = nullptr;
	// ゲームのフェーズ（型）
	enum class Phase {
		kFadeIn, // フェードイン
		kPlay, // ゲームプレイ
		kDeath, // デス演出
		kFadeOut, // フェードアウト 
	};
	// ゲームの現在フェーズ（変数）
	Phase phase_;
	// 終了フラグ
	bool finished_ = false;
	// フェード
	Fade* fade_ = nullptr;
	// フェード時間
	static inline const int fadeTime = 1;

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
	// 全ての当たり判定を行う
	void CheckAllCollisions();
	// フェーズの切り替え
	void ChargePhase();
	// 終了フラグのgetter
	bool GetFinished() const { return finished_; };
};

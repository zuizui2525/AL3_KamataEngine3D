#pragma once  
#include <vector>  
#include "KamataEngine.h"  

// ゲームシーン  
class GameScene {  
private:  
	// テクスチャハンドル  
	uint32_t textureHandle_ = 0;  
	// 3Dモデル  
	KamataEngine::Model* modelBlock_;  
	// ブロック用のワールドトランスフォーム  
	std::vector<KamataEngine::WorldTransform*> worldTransformBlocks_;  

public:  
	// デストラクタ  
	~GameScene();  
	// 初期化  
	void Initialize();  
	// 更新  
	void Update();  
	// 描画  
	void Draw();  
};

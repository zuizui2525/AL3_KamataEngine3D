#pragma once  
#include"KamataEngine.h"  

//ゲームシーン  
class GameScene {  
private:  
	// テクスチャハンドル  
	uint32_t textureHandle_ = 0;  
	// スプライト  
	KamataEngine::Sprite* sprite_ = nullptr;  
	// DirectX共通クラスへのポインタ  
	KamataEngine::DirectXCommon* dxCommon = nullptr;  

public:  
	//初期化  
	void Initialize();  
	// 更新  
	void Update();  
	// 描画  
	void Draw();  
	// コンストラクタ  
	GameScene();  
	// デストラクタ  
	~GameScene();  
};

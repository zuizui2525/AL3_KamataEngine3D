#include "Skydome.h"  
#include "Function.h"

// KamataEngineの名前空間を使用  
using namespace KamataEngine;  

void Skydome::Initialize(Model* model, Camera* camera) {  
    // ファイル名を指定してテクスチャハンドルを読み込む
	textureHandle_ = TextureManager::Load("./Resources/skydome/skydome.png");
    // モデルの生成  
    model_ = model; 
    // カメラの生成と初期化  
    camera_ = camera;
    // ワールドトランスフォームの初期化   
    worldTransform_.Initialize();  
}

void Skydome::Update() {
	
}  

void Skydome::Draw() {
	// モデルの描画
	model_->Draw(worldTransform_, *camera_, textureHandle_);
}

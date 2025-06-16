#include "Player.h"

void Player::Initialize(Model* model, uint32_t textureHandle, Camera* camera) {
	// NULLポインタチェック
	assert(model);
	// データをモデルに代入
	model_ = model;
	// NULLポインタチェック
	assert(textureHandle != 0);
	// データをテクスチャハンドルに代入
	textureHandle_ = textureHandle;
	// NULLポインタチェック
	assert(camera);
	// データをカメラに代入
	camera_ = camera;
	// ワールド変数の初期化
	worldTransform_.Initialize();
}

void Player::Update() {
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Player::Draw() {  
	// 3Dモデルの描画 
	model_->Draw(worldTransform_, camera_, textureHandle_);
}
#include "Player.h"

// DirectXCommonインスタンスの取得
DirectXCommon* dxCommon = DirectXCommon::GetInstance();

void Player::Initialize(Model* model, uint32_t textureHandle, Camera* camera) {
	// NULLポインタチェック
	assert(model);
	assert(textureHandle != 0);
	assert(camera);
	// データをモデルに代入
	model_ = model;
	// データをテクスチャハンドルに代入
	textureHandle_ = textureHandle;
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
	model_->PreDraw(dxCommon->GetCommandList());
	model_->Draw(worldTransform_, *camera_, textureHandle_);
	model_->PostDraw();
}
#include "Clear.h"
#include "Function.h"
#include <cassert>

void Clear::Initialize(Model* model, Camera* camera, const Vector3& position) {
	// モデルポインタがnullptrではないかアサートで確認
	assert(model);
	assert(camera);
	// データをカメラに代入
	camera_ = camera;
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	// ゴール用のテクスチャハンドルをロード
	textureHandle_ = TextureManager::Load("./Resources/goal/goal.png");
}

void Clear::Update() {
	worldTransform_.rotation_.y += 0.03f;
	// ワールド変換行列を更新
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Clear::Draw() {
	// モデルポインタがnullptrではないかアサートで確認
	assert(model_);
	// モデルを描画
	model_->Draw(worldTransform_, *camera_, textureHandle_);
}

AABB Clear::GetAABB() const {
	// ワールド座標とサイズに基づいてAABBを計算
	AABB aabb = {
	    worldTransform_.translation_.x - kWidth_ / 2.0f, worldTransform_.translation_.y - kHeight_ / 2.0f, worldTransform_.translation_.z - kWidth_ / 2.0f,
	    worldTransform_.translation_.x + kWidth_ / 2.0f, worldTransform_.translation_.y + kHeight_ / 2.0f, worldTransform_.translation_.z + kWidth_ / 2.0f,
	};
	return aabb;
}

Vector3 Clear::GetWorldPosition() const {
	// ワールド座標を返す
	return worldTransform_.translation_;
}
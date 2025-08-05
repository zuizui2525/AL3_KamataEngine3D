#include "Enemy.h"
#include "Function.h"
#include <numbers>

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
	// NULLポインタチェック
	assert(model);
	assert(camera);
	// ファイル名を指定してテクスチャハンドルを読み込む
	textureHandle_ = TextureManager::Load("./Resources/enemy/enemyColor.png");
	// データをモデルに代入
	model_ = model;
	// データをカメラに代入
	camera_ = camera;
	// ワールド変数の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	//worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	// 速度の設定
	velocity_ = {-kWalkSpeed, 0, 0};
	walkTimer_ = 0.0f;
}

void Enemy::Update() {
	// タイマー加算
	walkTimer_ += 1.0f / 60.0f;
	// 回転アニメーション
	float param = std::sin(std::numbers::pi_v<float> * walkTimer_ / kWalkMotionTime);
	float degree = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
	worldTransform_.rotation_.z = ConvertToRadian(degree);
	// 移動
	worldTransform_.translation_ += velocity_;

	// アフィン変換行列の作成
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Enemy::Draw() {
	// 3Dモデルの描画
	model_->Draw(worldTransform_, *camera_, textureHandle_);
}

Vector3 Enemy::GetWorldPosition() {
	Vector3 worldPos{};
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;
	return worldPos;
}

AABB Enemy::GetAABB() {
	Vector3 worldPos = GetWorldPosition();
	AABB aabb;
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};
	return aabb;
}

// 衝突判定
void Enemy::OnCollision(const Player* player) {
	(void)player;
}

#include "Enemy.h"
#include "Function.h"
#include "Player.h"
#include <numbers>
#include <algorithm>

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
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
	// 行動リクエストがあれば反映
	if (behaviorRequest_ != Behavior::kUnknown) {
		if (behavior_ != behaviorRequest_) {
			// 振る舞いが切り替わった瞬間だけ
			behavior_ = behaviorRequest_;

			if (behavior_ == Behavior::kDead) {
				deadTimer_ = 0.0f;
				isDead_ = false;
			}
		}
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
	case Behavior::kRoot: {
		walkTimer_ += 1.0f / 60.0f;
		float param = std::sin(std::numbers::pi_v<float> * walkTimer_ / kWalkMotionTime);
		float degree = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
		worldTransform_.rotation_.z = ConvertToRadian(degree);
		worldTransform_.translation_ += velocity_;
		break;
	}
	case Behavior::kDead: {
		isCollisionDisabled_ = true;
		deadTimer_ += 1.0f / 60.0f; // deltaTime = 前フレームからの経過秒
		float t = deadTimer_ / 1.0f;
		t = std::clamp(t, 0.0f, 1.0f);

		worldTransform_.scale_.x = EaseOut(0.3f, 1.3f, t);
		worldTransform_.scale_.y = EaseOut(1.6f, 0.7f, t);

		if (t >= 1.0f) {
			isDead_ = true; // フラグON
		}
		break;
	}
	}

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
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
	if (behavior_ == Behavior::kDead) {
		return;
	}
	// プレイヤーが攻撃中なら敵が死ぬ
	if (player->IsAttack()) {
		behaviorRequest_ = Behavior::kDead;
	}
}

#include "DeathParticle.h"
#include "Function.h"
#include <algorithm>

void DeathParticle::Initialize(Model* model, Camera* camera, const Vector3& position) {
	// NULLポインタチェック
	assert(model);
	assert(camera);
	// データをモデルに代入
	model_ = model;
	// データをカメラに代入
	camera_ = camera;
	// ワールド変換の初期化
	for (WorldTransform& worldTransform : worldTransforms_) {
		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}
	// 色
	objectColor_.Initialize();
	color_ = {1, 1, 1, 1};
}

void DeathParticle::Update() {
	// 終了なら何もしない
	if (isFinished_) {
		return;
	}

	for (uint32_t i = 0; i < kNumParticles; ++i) {
		// 基本となる速度ベクトル
		Vector3 velocity = {kSpeed, 0, 0};
		// 回転角を計算する
		float angle = kAngleUnit * i;
		//Z軸まわりの回転行列
		Matrix4x4 matrixRotation = MakeRotateZMatrix(angle);
		// 基本ベクトルを回転させて速度ベクトルを得る
		velocity = Transform(matrixRotation, velocity);
		// 移動処理
		worldTransforms_[i].translation_ += velocity;
	}

	// カウンターを1フレーム分の秒数進める
	counter_ += 1.0f / 60.0f;

	// 存在時間の上限に達したら
	if (counter_ >= kDuration) {
		counter_ = kDuration;
		// 終了扱いする
		isFinished_ = true;
	}

	// 色
	color_.w = std::clamp((1.0f - counter_ / kDuration), 0.0f, 1.0f);
	// 色変更オブジェクトに色の数値を設定する
	objectColor_.SetColor(color_);

	// ワールド変換の更新
	for (WorldTransform& worldTransform : worldTransforms_) {
		// アフィン変換行列の作成
		worldTransform.matWorld_ = MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);
		// 行列を定数バッファに転送
		worldTransform.TransferMatrix();
	}
}

void DeathParticle::Draw() {
	// 終了なら何もしない
	if (isFinished_) {
		return;
	}

	// 3Dモデルの描画
	for (WorldTransform& worldTransform : worldTransforms_) {
		model_->Draw(worldTransform, *camera_, textureHandle_, &objectColor_);
	}
}
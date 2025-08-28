#include "CameraController.h"
#include "Player.h"
#include "Function.h"

// KamataEngineの名前空間を使用
using namespace KamataEngine;

void CameraController::Initialize() {
	// カメラの初期化
	camera_->Initialize();
}

void CameraController::Update() {
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象の速度を参照
	const Vector3& targetVelocity = target_->GetVelocity();
	// 追従対象とオフセットからカメラの目標座標を計算
	targetCoordinates_.x = targetWorldTransform.translation_.x + targetOffset_.x + targetVelocity.x * kVelocityBias;
	targetCoordinates_.y = targetWorldTransform.translation_.y + targetOffset_.y + targetVelocity.y * kVelocityBias;
	targetCoordinates_.z = targetWorldTransform.translation_.z + targetOffset_.z + targetVelocity.z * kVelocityBias;
	// 座標補間によりゆっくり追従
	camera_->translation_ = Lerp(camera_->translation_, targetCoordinates_, kInterpolationRate);
	// 追従対象が画面外に出ないように補正
	camera_->translation_.x = (std::max)(camera_->translation_.x, targetWorldTransform.translation_.x + margin.left);
	camera_->translation_.x = (std::min)(camera_->translation_.x, targetWorldTransform.translation_.x + margin.right);
	camera_->translation_.y = (std::max)(camera_->translation_.y, targetWorldTransform.translation_.x + margin.bottom);
	camera_->translation_.y = (std::min)(camera_->translation_.y, targetWorldTransform.translation_.x + margin.top);
	// カメラの範囲制限
	camera_->translation_.x = (std::max)(camera_->translation_.x, movableArea_.left);
	camera_->translation_.x = (std::min)(camera_->translation_.x, movableArea_.right);
	camera_->translation_.y = (std::max)(camera_->translation_.y, movableArea_.bottom);
	camera_->translation_.y = (std::min)(camera_->translation_.y, movableArea_.top);
	// 行列の更新
	camera_->UpdateMatrix();
}

void CameraController::Reset() {
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象の速度を参照
	const Vector3& targetVelocity = target_->GetVelocity();
	// 追従対象とオフセットからカメラの座標を計算
	camera_->translation_.x = targetWorldTransform.translation_.x + targetOffset_.x + targetVelocity.x * kVelocityBias;
	camera_->translation_.y = targetWorldTransform.translation_.y + targetOffset_.y + targetVelocity.y * kVelocityBias;
	camera_->translation_.z = targetWorldTransform.translation_.z + targetOffset_.z + targetVelocity.z * kVelocityBias;
}
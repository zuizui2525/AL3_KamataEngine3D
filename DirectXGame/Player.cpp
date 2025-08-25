#include "Player.h"
#include "Function.h"
#include "MapChipField.h"
#include <numbers>
#include <algorithm>

// DirectXCommonインスタンスの取得
DirectXCommon* dxCommon = DirectXCommon::GetInstance();

void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {
	// NULLポインタチェック
	assert(model);
	assert(camera);
	// ファイル名を指定してテクスチャハンドルを読み込む
	textureHandle_ = TextureManager::Load("./Resources/PatiDigda/PatiDigda.png");
	// データをモデルに代入
	model_ = model;
	// データをカメラに代入
	camera_ = camera;
	// ワールド変数の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void Player::Update() {	
	if (behaviorRequest_ != Behavior::kUnknown) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_;
		// ふるまいごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
			BehaviorRootInitialize();
			break;
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		}
		// ふるまいリクエストをリセット
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	}

}

void Player::BehaviorRootUpdate() {
	// 設置状態
	// ①移動入力
	HandleMoveInput();
	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.velocity = velocity_;
	// マップ衝突チェック
	MapCollisionDetection(collisionMapInfo);
	// ④天井に接触している時の処理
	touchingTheCeiling(collisionMapInfo);
	// ⑤壁に接触している場合の処理
	contactWithAWall(collisionMapInfo);
	// ⑥設置状態の切り替え
	SwitchingInstallationStatus(collisionMapInfo);

	if (turnTimer_ > 0.0f) {
		// 旋回タイマーを1/60秒だけカウントダウンする
		turnTimer_ -= 1.0f / 60.0f;
		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		// 自キャラの角度を設定する
		float t = 1.0f - (turnTimer_ / kTimeTurn);
		worldTransform_.rotation_.y = Lerp(turnFirstRotationY_, destinationRotationY, t);
	}

	if (Input::GetInstance()->TriggerKey(DIK_E)) {
		// 攻撃ビヘイビアをリクエスト
		behaviorRequest_ = Behavior::kAttack;
	}

	// アフィン変換行列の作成
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Player::BehaviorAttackUpdate() {
	// 設置状態
	// 予備動作
	attackParameter_++;

	switch (attackPhase_) {
	case AttackPhase::reservoir:
	default:{
		float t = static_cast<float>(attackParameter_) / 3.0f;
		worldTransform_.scale_.z = EaseOut(1.0f, 0.3f, t);
		worldTransform_.scale_.y = EaseOut(1.0f, 1.6f, t);
		// 前進動作に移行
		if (attackParameter_ >= 3.0f) {
			attackPhase_ = AttackPhase::rush;
			attackParameter_ = 0; // カウンターリセット
		}
		break;
	}
	case AttackPhase::rush:{
		float t = static_cast<float>(attackParameter_) / 3.0f;
		worldTransform_.scale_.z = EaseOut(0.3f, 1.3f, t);
		worldTransform_.scale_.y = EaseOut(1.6f, 0.7f, t);
		// 余韻動作に移行
		if (attackParameter_ >= 3.0f) {
			attackPhase_ = AttackPhase::lingeringSound;
			attackParameter_ = 0; // カウンターリセット
		}
		// 前進
		if (worldTransform_.rotation_.y < std::numbers::pi_v<float>) {
			velocity_ = Vector3(0.5f, 0, 0);
		} else {
			velocity_ = Vector3(-0.5f, 0, 0);
		}
		break;
	}
	case AttackPhase::lingeringSound: {
		float t = static_cast<float>(attackParameter_) / 3.0f;
		worldTransform_.scale_.z = EaseOut(1.3f, 1.0f, t);
		worldTransform_.scale_.y = EaseOut(0.7f, 1.0f, t);
		break;
	}
	}

	// 規定の時間経過で攻撃終了して通常状態に戻す
	if (attackParameter_ >= attackTime_) {
		behaviorRequest_ = Behavior::kRoot;
	}

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.velocity = velocity_;
	// マップ衝突チェック
	MapCollisionDetection(collisionMapInfo);
	// ④天井に接触している時の処理
	touchingTheCeiling(collisionMapInfo);
	// ⑤壁に接触している場合の処理
	contactWithAWall(collisionMapInfo);
	// ⑥設置状態の切り替え
	SwitchingInstallationStatus(collisionMapInfo);

	if (turnTimer_ > 0.0f) {
		// 旋回タイマーを1/60秒だけカウントダウンする
		turnTimer_ -= 1.0f / 60.0f;
		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		// 自キャラの角度を設定する
		float t = 1.0f - (turnTimer_ / kTimeTurn);
		worldTransform_.rotation_.y = Lerp(turnFirstRotationY_, destinationRotationY, t);
	}

	// アフィン変換行列の作成
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Player::BehaviorRootInitialize() {}

void Player::BehaviorAttackInitialize() { 
	attackParameter_ = 0; 
	attackPhase_ = AttackPhase::reservoir;
}

bool Player::IsAttack() const {
	if (behavior_ == Behavior::kAttack) {
		return true;
	} else {
		return false;
	}
}

void Player::Draw() {
	// 3Dモデルの描画
	model_->Draw(worldTransform_, *camera_, textureHandle_);
}

Vector3 Player::GetWorldPosition() {
	Vector3 worldPos{};
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;
	return worldPos;
}

AABB Player::GetAABB() {
	Vector3 worldPos = GetWorldPosition();
	AABB aabb;
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};
	return aabb;
}

// 衝突判定
void Player::OnCollision(const Enemy* enemy) {
	if (IsAttack()) {
		return;
	}
	(void)enemy;
	// フラグを立てる
	isDead_ = true;
}

void Player::HandleMoveInput() {
	if (onGround_) {
		// 設置状態時の移動入力処理
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x += kAcceleration;
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x -= kAcceleration;
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			}
			velocity_ += acceleration;
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			velocity_.x *= (1.0f - kAttenuation);
		}

		if (Input::GetInstance()->PushKey(DIK_UP)) {
			velocity_ += Vector3(0, kJumpAcceleration, 0);
		}
	} else {
		// 落下速度
		velocity_ = Add(velocity_, Vector3(0.0f, -kGravityAccleration, 0.0f));
		// 落下速度制限
		velocity_.y = (std::max)(velocity_.y, -kLimitFallSpeed);
	}
}

void Player::MapCollisionDetection(CollisionMapInfo& info) {
	// 衝突情報を初期化
	info.ceilingCollisionFlag = false;
	info.landingFlag = false;
	info.wallContactFlag = false;

	// X軸の移動量のみを考慮して当たり判定を行う
	CollisionMapInfo horizontalInfo{};
	horizontalInfo.velocity = Vector3(info.velocity.x, 0.0f, 0.0f);

	MapCollisionDetectionLeft(horizontalInfo);
	MapCollisionDetectionRight(horizontalInfo);

	// X軸の判定結果を反映
	worldTransform_.translation_.x += horizontalInfo.velocity.x;
	info.wallContactFlag = horizontalInfo.wallContactFlag;

	// Y軸の移動量のみを考慮して当たり判定を行う
	CollisionMapInfo verticalInfo{};
	verticalInfo.velocity = Vector3(0.0f, info.velocity.y, 0.0f);

	MapCollisionDetectionUp(verticalInfo);
	MapCollisionDetectionDown(verticalInfo);

	// Y軸の判定結果を反映
	worldTransform_.translation_.y += verticalInfo.velocity.y;
	info.landingFlag = verticalInfo.landingFlag;
	info.ceilingCollisionFlag = verticalInfo.ceilingCollisionFlag;
}

void Player::MapCollisionDetectionUp(CollisionMapInfo& info) {
	const float kPenetrationBuffer = 0.01f; // ほんの少し余裕をもたせる
	// 上昇アリ？
	if (info.velocity.y <= 0) {
		return;
	}
	// 移動後の４つの角の座標
	std::array<Vector3, 4> positionsNew{};
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.velocity, static_cast<Corner>(i));
	}
	MapChipField::MapChipType mapChipType{};
	// 真上の当たり判定を行う
	bool hitLeft = false;
	bool hitRight = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet{};
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipField::MapChipType::kBlock) {
		hitLeft = true;
	}
	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipField::MapChipType::kBlock) {
		hitRight = true;
	}
	if (hitLeft) {
		// 天井のどのマスに当たったかを取得
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(Vector3(positionsNew[kLeftTop].x, worldTransform_.translation_.y + kHeight / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// 天井の矩形情報を取得
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// Y方向の移動量を調整（めり込みを防ぐ）
			info.velocity.y = (std::max)(0.0f, rect.bottom - worldTransform_.translation_.y - kPenetrationBuffer - kHeight / 2.0f);
			// フラグを立てる
			info.ceilingCollisionFlag = true;
		}
	} 
	if (hitRight) {
		// 天井のどのマスに当たったかを取得
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(Vector3(positionsNew[kRightTop].x, worldTransform_.translation_.y + kHeight / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// 天井の矩形情報を取得
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// Y方向の移動量を調整（めり込みを防ぐ）
			info.velocity.y = (std::max)(0.0f, rect.bottom - worldTransform_.translation_.y - kPenetrationBuffer - kHeight / 2.0f);
			// フラグを立てる
			info.ceilingCollisionFlag = true;
		}
	}
}

void Player::MapCollisionDetectionDown(CollisionMapInfo& info) {
	const float kPenetrationBuffer = 0.01f; // ほんの少し余裕をもたせる
	// 下降あり？
	if (info.velocity.y >= 0) {
		return;
	}
	// 移動後の４つの角の座標
	std::array<Vector3, 4> positionsNew{};
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.velocity, static_cast<Corner>(i));
	}
	MapChipField::MapChipType mapChipType{};
	// 真下の当たり判定を行う
	bool hitLeft = false;
	bool hitRight = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet{};
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipField::MapChipType::kBlock) {
		hitLeft = true;
	}
	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipField::MapChipType::kBlock) {
		hitRight = true;
	}
	if (hitLeft) {
		// 地面のどのマスに当たったかを取得
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(Vector3(positionsNew[kLeftBottom].x, worldTransform_.translation_.y - kHeight / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// 地面の矩形情報を取得
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// Y方向の移動量を調整（めり込みを防ぐ）
			info.velocity.y = (std::min)(0.0f, rect.top - worldTransform_.translation_.y + kPenetrationBuffer + kHeight / 2.0f);
			// フラグを立てる
			info.landingFlag = true;
		}
	}
	if (hitRight) {
		// 地面のどのマスに当たったかを取得
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(Vector3(positionsNew[kRightBottom].x, worldTransform_.translation_.y - kHeight / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// 地面の矩形情報を取得
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// Y方向の移動量を調整（めり込みを防ぐ）
			info.velocity.y = (std::min)(0.0f, rect.top - worldTransform_.translation_.y + kPenetrationBuffer + kHeight / 2.0f);
			// フラグを立てる
			info.landingFlag = true;
		}
	} 
}

void Player::MapCollisionDetectionLeft(CollisionMapInfo& info) {
	const float kPenetrationBuffer = 0.01f; // ほんの少し余裕をもたせる
	// 壁あり？
	if (info.velocity.x >= 0) {
		return;
	}
	// 移動後の４つの角の座標
	std::array<Vector3, 4> positionsNew{};
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.velocity, static_cast<Corner>(i));
	}
	MapChipField::MapChipType mapChipType{};
	// 真左の当たり判定を行う
	bool hitTop = false;
	bool hitBottom = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet{};
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipField::MapChipType::kBlock) {
		hitTop = true;
	}
	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipField::MapChipType::kBlock) {
		hitBottom = true;
	}
	if (hitTop) {
		// 壁のどのマスに当たったかを取得
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(Vector3(worldTransform_.translation_.x - kWidth / 2.0f, positionsNew[kLeftTop].y, 0));
		if (indexSetNow.xIndex != indexSet.xIndex) {
			// 壁の矩形情報を取得
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// X方向の移動量を調整（めり込みを防ぐ）
			info.velocity.x = (std::max)(0.0f, rect.right - worldTransform_.translation_.x + kPenetrationBuffer - kWidth / 2.0f);
			// フラグを立てる
			info.wallContactFlag = true;
		}
	}
	if (hitBottom) {
		// 壁のどのマスに当たったかを取得
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(Vector3(worldTransform_.translation_.x - kWidth / 2.0f, positionsNew[kLeftBottom].y, 0));
		if (indexSetNow.xIndex != indexSet.xIndex) {
			// 壁の矩形情報を取得
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// X方向の移動量を調整（めり込みを防ぐ）
			info.velocity.x = (std::max)(0.0f, rect.right - worldTransform_.translation_.x + kPenetrationBuffer - kWidth / 2.0f);
			// フラグを立てる
			info.wallContactFlag = true;
		}
	}
}

void Player::MapCollisionDetectionRight(CollisionMapInfo& info) {
	const float kPenetrationBuffer = 0.01f; // ほんの少し余裕をもたせる
	// 壁あり？
	if (info.velocity.x <= 0) {
		return;
	}
	// 移動後の４つの角の座標
	std::array<Vector3, 4> positionsNew{};
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.velocity, static_cast<Corner>(i));
	}
	MapChipField::MapChipType mapChipType{};
	// 真右の当たり判定を行う
	bool hitTop = false;
	bool hitBottom = false;
	// 右上点の判定
	MapChipField::IndexSet indexSet{};
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipField::MapChipType::kBlock) {
		hitTop = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipField::MapChipType::kBlock) {
		hitBottom = true;
	}
	if (hitTop) {
		// 壁のどのマスに当たったかを取得
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(Vector3(worldTransform_.translation_.x + kWidth / 2.0f, positionsNew[kRightTop].y, 0));
		if (indexSetNow.xIndex != indexSet.xIndex) {
			// 壁の矩形情報を取得
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// X方向の移動量を調整（めり込みを防ぐ）
			info.velocity.x = (std::min)(0.0f, rect.left - worldTransform_.translation_.x + kPenetrationBuffer + kWidth / 2.0f);
			// フラグを立てる
			info.wallContactFlag = true;
		}
	}
if (hitBottom) {
		// 壁のどのマスに当たったかを取得
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(Vector3(worldTransform_.translation_.x + kWidth / 2.0f, positionsNew[kRightBottom].y, 0));
		if (indexSetNow.xIndex != indexSet.xIndex) {
			// 壁の矩形情報を取得
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// X方向の移動量を調整（めり込みを防ぐ）
			info.velocity.x = (std::min)(0.0f, rect.left - worldTransform_.translation_.x + kPenetrationBuffer + kWidth / 2.0f);
			// フラグを立てる
			info.wallContactFlag = true;
		}
	}
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {  
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, // kRightBottom
        {-kWidth / 2.0f, -kHeight / 2.0f, 0}, // kLeftBottom
        {+kWidth / 2.0f, +kHeight / 2.0f, 0}, // kRightTop
        {-kWidth / 2.0f, +kHeight / 2.0f, 0}  // kLeftTop
    };
	return center + offsetTable[static_cast<uint32_t>(corner)];
}

void Player::touchingTheCeiling(const CollisionMapInfo& info) {
	if (info.ceilingCollisionFlag) {
		velocity_.y = 0;
	}
}

void Player::SwitchingInstallationStatus(const CollisionMapInfo& info) {
	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			MapChipField::MapChipType mapChipType;
			// 真下の当たり判定を行う
			bool hitLeft = false;
			bool hitRight = false;
			// 移動後の４つの角の座標
			std::array<Vector3, 4> positionsNew{};
			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.velocity, static_cast<Corner>(i));
			}
			// 左上点の判定
			MapChipField::IndexSet indexSet{};
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipField::MapChipType::kBlock) {
				hitLeft = true;
			}
			// 右上点の判定
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipField::MapChipType::kBlock) {
				hitRight = true;
			}
			if (!hitLeft && !hitRight) {
				// 空中状態に切り替える
				onGround_ = false;
			}
		}
	} else {
		// 自キャラが設置状態？
		// 接地判定
		if (info.landingFlag) {
			// 着地状態に切り替える（落下を止める）
			onGround_ = true;
			// 着地時にX速度を減衰
			velocity_.x *= (1.0f - kAttenuationLanding);
			// Y速度をゼロにする
			velocity_.y = 0.0f;
		} 
	}
}

void Player::contactWithAWall(const CollisionMapInfo& info) {
	// 壁接触による減速
	if (info.wallContactFlag) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
}
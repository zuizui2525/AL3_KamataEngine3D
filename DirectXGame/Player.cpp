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
	// 設置状態
	// ①移動入力
	HandleMoveInput();
	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.velocity = velocity_;
	// マップ衝突チェック
	MapCollisionDetection(collisionMapInfo);
	// ③判定結果を反映させて移動させる
	ActAccordingToTheOutcomeOfTheDecision(collisionMapInfo);
	// ④天井に接触している時の処理
	touchingTheCeiling(collisionMapInfo);
	if (!collisionMapInfo.landingFlag) {
		// 落下速度
		velocity_ += Vector3(0, -kGravityAccleration, 0);
		// 落下速度制限
        velocity_.y = (std::max)(velocity_.y, -kLimitFallSpeed);
	}

	if (turnTimer_ > 0.0f) {
		// 旋回タイマーを1/60秒だけカウントダウンする
		turnTimer_ -= 1.0f / 60.0f;
		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {
			std::numbers::pi_v<float> / 2.0f, 
			std::numbers::pi_v<float> * 3.0f / 2.0f
		};
		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		// 自キャラの角度を設定する
		float t = 1.0f - (turnTimer_ / kTimeTurn);
		worldTransform_.rotation_.y = Lerp(turnFirstRotationY_, destinationRotationY, t);
	}

	// 接地判定
	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			// 空中状態に移行
			onGround_ = false;
		}
	} else {
		// 着地
		if (collisionMapInfo.landingFlag) {
			// 摩擦で横方向速度が減衰する
			velocity_.x *= (1.0f - kAttenuation);
			// 下方向速度をリセット
			velocity_.y = 0.0f;
			// 接地状態に移行
			onGround_ = true;
			collisionMapInfo.landingFlag = false;
		}
	}
	
	// アフィン変換行列の作成
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Player::Draw() {
	// 3Dモデルの描画
	model_->Draw(worldTransform_, *camera_, textureHandle_);
}

void Player::HandleMoveInput() {
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

	if (onGround_ && Input::GetInstance()->PushKey(DIK_UP)) {
		velocity_ += Vector3(0, kJumpAcceleration, 0);
	}
}

void Player::MapCollisionDetection(CollisionMapInfo& info) {
	MapCollisionDetectionUp(info);
	MapCollisionDetectionDown(info);
	//MapCollisionDetectionLeft(info);
	//MapCollisionDetectionRight(info);
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
		// 天井の矩形情報を取得
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		// Y方向の移動量を調整（めり込みを防ぐ）
		info.velocity.y = (std::max)(0.0f, rect.bottom - positionsNew[kLeftTop].y - kPenetrationBuffer);
		// フラグを立てる
		info.ceilingCollisionFlag = true;
	} else if (hitRight) {
		// 天井のどのマスに当たったかを取得
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
		// 天井の矩形情報を取得
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		// Y方向の移動量を調整（めり込みを防ぐ）
		info.velocity.y = (std::max)(0.0f, rect.bottom - positionsNew[kRightTop].y - kPenetrationBuffer);
		// フラグを立てる
		info.ceilingCollisionFlag = true;
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
		// 地面の矩形情報を取得
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		// Y方向の移動量を調整（めり込みを防ぐ）
		info.velocity.y = (std::max)(0.0f, rect.bottom - positionsNew[kLeftBottom].y + kPenetrationBuffer);
		// フラグを立てる
		info.landingFlag = true;
	} else if (hitRight) {
		// 地面のどのマスに当たったかを取得
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		// 地面の矩形情報を取得
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		// Y方向の移動量を調整（めり込みを防ぐ）
		info.velocity.y = (std::max)(0.0f, rect.bottom - positionsNew[kRightBottom].y + kPenetrationBuffer);
		// フラグを立てる
		info.landingFlag = true;
	}
}

//void Player::MapCollisionDetectionLeft(CollisionMapInfo& info) {}

//void Player::MapCollisionDetectionRight(CollisionMapInfo& info) {}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {  
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, // kRightBottom
        {-kWidth / 2.0f, +kHeight / 2.0f, 0}, // kLeftBottom
        {+kWidth / 2.0f, -kHeight / 2.0f, 0}, // kRightTop
        {-kWidth / 2.0f, -kHeight / 2.0f, 0}  // kLeftTop
    };
	return center + offsetTable[static_cast<uint32_t>(corner)];
}

void Player::ActAccordingToTheOutcomeOfTheDecision(const CollisionMapInfo& info) {
	// 移動
	worldTransform_.translation_ += info.velocity;
}

void Player::touchingTheCeiling(const CollisionMapInfo& info) {
	if (info.ceilingCollisionFlag) {
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0;
	}
}

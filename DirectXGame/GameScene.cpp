#include "GameScene.h"
#include "Function.h"
#include "Collision.h"

// KamataEngineの名前空間を使用
using namespace KamataEngine;

// デストラクタ
GameScene::~GameScene() {
	// 3Dモデルの解放
	delete modelBlock_;
	delete modelSkydome_;
	delete skydome_;
	// デバックカメラの解放
	delete debugCamera_;
	// コントロールカメラの解放
	delete cameraController_;
	// プレイヤーの解放
	delete player_;
	delete modelPlayer_;
	// 敵の解放
	delete modelEnemy_;
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	// フェードの解放
	delete fade_;
	// デスパーティクルの解放
	delete deathParticles_;
	delete modelDeathParticle_;
	// クリアの解放
	delete modelClear_;
	// マップチップフィールドの解放
	delete mapChipField_;
	// ブロック用のワールドトランスフォームの解放
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
}

// 初期化処理
void GameScene::Initialize() {
	// フェード
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, fadeTime);
	// ファイル名を指定してテクスチャハンドルを読み込む
	textureHandle_ = TextureManager::Load("./Resources/cube/cube.jpg");
	// カメラの初期化
	camera_.Initialize();
	// デバックカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
	// 3Dモデルの生成
	modelBlock_ = Model::Create();
	modelDeadBlock_ = Model::Create();
	// 天球の初期化
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);
	// マップチップフィールド
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/mapChipData/mapChip.csv");
	GenerateBlocks();
	// プレイヤー
	// 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 16);
	modelPlayer_ = Model::CreateFromOBJ("PatiDigda", true);
	player_ = new Player();
	player_->Initialize(modelPlayer_, &camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);
	// コントロールカメラ
	cameraController_ = new CameraController();
	cameraController_->SetMovableArea(
	    {mapChipField_->GetMapChipPositionByIndex(11, 0).x, mapChipField_->GetMapChipPositionByIndex(88, 0).x, 
		 mapChipField_->GetMapChipPositionByIndex(0, 13).y, mapChipField_->GetMapChipPositionByIndex(0, 13).y});
	cameraController_->SetTarget(player_);
	cameraController_->SetCamera(&camera_);
	cameraController_->Initialize();
	cameraController_->Reset();
	// 敵
	modelEnemy_ = Model::CreateFromOBJ("enemy", true);
	for (int32_t i = 0; i < 3; ++i) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(i*2 + 45, 14 + i *2);
		newEnemy->Initialize(modelEnemy_, &camera_, enemyPosition);

		enemies_.push_back(newEnemy);
	}
	// デスパーティクル
	modelDeathParticle_ = Model::CreateFromOBJ("deathParticle", true);
	// Clearクラスのモデルをロード
	Model* clearModel = Model::CreateFromOBJ("cube", true);
	// ゴールを初期化
	Vector3 goalPosition = mapChipField_->GetMapChipPositionByIndex(1, 13);
	clear_.Initialize(clearModel, &camera_, goalPosition);

	// ゲームプレイフェーズから開始
	phase_ = Phase::kFadeIn;
}

// ゲームの更新処理
void GameScene::Update() {
	// チェンジ
	ChargePhase();

	switch (phase_) {
	case Phase::kPlay:
#ifdef _DEBUG
		if (Input::GetInstance()->TriggerKey(DIK_R)) {
			if (isDebugCameraActive_ == true) {
				isDebugCameraActive_ = false;
			} else {
				isDebugCameraActive_ = true;
			}
		}
#endif
		// 天球
		skydome_->Update();
		// プレイヤー
		player_->Update();
		// 敵
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}
		enemies_.remove_if([](Enemy* enemy) {
			if (enemy->IsDead()) {
				delete enemy;
				return true;
			}
			return false;
		});
		// ゴールの更新
		clear_.Update();

		// カメラの処理
		if (isDebugCameraActive_) {
			// デバックカメラの更新
			debugCamera_->Update();
			camera_.matView = debugCamera_->GetCamera().matView;
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			// ビュー行列の転送
			camera_.TransferMatrix();
		} else {
			// コントロールカメラ
			cameraController_->Update();
			// ビュープロジェクション行列の更新と転送
			camera_.UpdateMatrix();
			// ビュー行列の転送
			camera_.TransferMatrix();
		}
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock) {
					continue;
				}
				// アフィン変換行列の作成
				worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
				// 定数バッファに転送する
				worldTransformBlock->TransferMatrix();
			}
		}
		// 当たり判定
		CheckAllCollisions();
		break;
	case Phase::kDeath:
		// 天球
		skydome_->Update();
		// 敵
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}
		// デスパーティクル
		if (deathParticles_) {
			deathParticles_->Update();
		}
		// カメラの処理
		if (isDebugCameraActive_) {
			// デバックカメラの更新
			debugCamera_->Update();
			camera_.matView = debugCamera_->GetCamera().matView;
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			// ビュー行列の転送
			camera_.TransferMatrix();
		} else {
			// コントロールカメラ
			cameraController_->Update();
			// ビュープロジェクション行列の更新と転送
			camera_.UpdateMatrix();
			// ビュー行列の転送
			camera_.TransferMatrix();
		}
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock) {
					continue;
				}
				// アフィン変換行列の作成
				worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
				// 定数バッファに転送する
				worldTransformBlock->TransferMatrix();
			}
		}
		break;
	case Phase::kClear:
		// クリア演出
		clear_.Update();
		// 天球
		skydome_->Update();
		// 敵
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}
		// カメラの処理
		if (isDebugCameraActive_) {
			// デバックカメラの更新
			debugCamera_->Update();
			camera_.matView = debugCamera_->GetCamera().matView;
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			// ビュー行列の転送
			camera_.TransferMatrix();
		} else {
			// コントロールカメラ
			cameraController_->Update();
			// ビュープロジェクション行列の更新と転送
			camera_.UpdateMatrix();
			// ビュー行列の転送
			camera_.TransferMatrix();
		}
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock) {
					continue;
				}
				// アフィン変換行列の作成
				worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
				// 定数バッファに転送する
				worldTransformBlock->TransferMatrix();
			}
		}
		break;
	}
}

// 描画処理
void GameScene::Draw() {
	// 天球の描画
	skydome_->Draw();

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}
			// マップチップタイプに応じてモデルを選択
			MapChipField::IndexSet index = mapChipField_->GetMapChipIndexSetByPosition(worldTransformBlock->translation_);
			if (mapChipField_->GetMapChipTypeByIndex(index.xIndex, index.yIndex) == MapChipField::MapChipType::kDeadBlock) {
				modelDeadBlock_->Draw(*worldTransformBlock, camera_, textureHandle_);
			} else {
				modelBlock_->Draw(*worldTransformBlock, camera_, textureHandle_);
			}
		}
	}

	// プレイヤーの描画
	player_->Draw();

	// 敵の描画
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	// デスパーティクル
	if (deathParticles_) {
		deathParticles_->Draw();
	}

	// ゴールの描画
	clear_.Draw();

	// フェード
	fade_->Draw();
}

void GameScene::GenerateBlocks() {
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 要素数を変更する
	// 列数を設定（縦方向のブロック数）
	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		// 1列の要素数を設定（横方向のブロック数）
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			MapChipField::MapChipType mapChipType = mapChipField_->GetMapChipTypeByIndex(j, i);
			if (mapChipType == MapChipField::MapChipType::kBlock || mapChipType == MapChipField::MapChipType::kDeadBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

// 全ての当たり判定を行う
void GameScene::CheckAllCollisions() {
#pragma region 自キャラと敵キャラ
	// 判定対象
	AABB aabb1, aabb2;
	// 自キャラの座標
	aabb1 = player_->GetAABB();

	// 自キャラと敵の当たり判定
	for (Enemy* enemy : enemies_) {
		// 敵キャラが死んだら
		if (enemy->IsCollisionDisabled()) {
			continue;
		}

		// 敵キャラの座標
		aabb2 = enemy->GetAABB();

		// AABB同士の交差判定
		if (IsCollision(aabb1, aabb2)) {
			// 自キャラの衝突判定
			player_->OnCollision(enemy);
			// 敵の衝突判定
			enemy->OnCollision(player_);
		}
	}
#pragma endregion

	// プレイヤーのAABBを取得
	AABB playerAABB = player_->GetAABB();
	// ゴールのAABBを取得
	AABB goalAABB = clear_.GetAABB();
	// AABB同士の当たり判定
	if (IsCollision(playerAABB, goalAABB)) {
		isGoalHit_ = true;
	}
#pragma region 自キャラとブロック
	// プレイヤーのインデックスを取得
	MapChipField::IndexSet playerIndex = mapChipField_->GetMapChipIndexSetByPosition(player_->GetWorldPosition());
	// マップチップのインデックスを取得
	MapChipField::MapChipType mapChipType = mapChipField_->GetMapChipTypeByIndex(playerIndex.xIndex, playerIndex.yIndex);

	// 即死ブロックの当たり判定
	if (mapChipType == MapChipField::MapChipType::kDeadBlock) {
		touchDeadBlock_ = true;
	}
#pragma endregion
}

void GameScene::ChargePhase() {
	switch (phase_) {
	case Phase::kFadeIn:
		// フェード
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kPlay;
		}

#ifdef _DEBUG
		if (Input::GetInstance()->TriggerKey(DIK_R)) {
			if (isDebugCameraActive_ == true) {
				isDebugCameraActive_ = false;
			} else {
				isDebugCameraActive_ = true;
			}
		}
#endif
		// 天球
		skydome_->Update();
		// プレイヤー
		player_->Update();
		// 敵
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		// カメラの処理
		if (isDebugCameraActive_) {
			// デバックカメラの更新
			debugCamera_->Update();
			camera_.matView = debugCamera_->GetCamera().matView;
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			// ビュー行列の転送
			camera_.TransferMatrix();
		} else {
			// コントロールカメラ
			cameraController_->Update();
			// ビュープロジェクション行列の更新と転送
			camera_.UpdateMatrix();
			// ビュー行列の転送
			camera_.TransferMatrix();
		}
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock) {
					continue;
				}
				// アフィン変換行列の作成
				worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
				// 定数バッファに転送する
				worldTransformBlock->TransferMatrix();
			}
		}
		break;
	case Phase::kPlay:
		if (player_->IsDead()) {
			// 死亡演出フェーズに切り替え
			phase_ = Phase::kDeath;
			// 自キャラの座標を取得
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();

			// デスパーティクル
			deathParticles_ = new DeathParticle();
			deathParticles_->Initialize(modelDeathParticle_, &camera_, deathParticlesPosition);
		}
		// ゴールに触れたらクリア演出へ移行
		if (isGoalHit_) {
			phase_ = Phase::kClear;
		}
		// デスブロックに触れたら
		if (touchDeadBlock_) {
			// 死亡演出フェーズに切り替え
			phase_ = Phase::kDeath;
			// 自キャラの座標を取得
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();

			// デスパーティクル
			deathParticles_ = new DeathParticle();
			deathParticles_->Initialize(modelDeathParticle_, &camera_, deathParticlesPosition);
		}
		break;
	case Phase::kDeath:
		if (deathParticles_ && deathParticles_->GetIsFinished()) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, fadeTime);
		}
		break;
	case Phase::kClear:
		phase_ = Phase::kFadeOut;
		fade_->Start(Fade::Status::FadeOut, fadeTime);
		break;
	case Phase::kFadeOut:
		// フェード
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}
}
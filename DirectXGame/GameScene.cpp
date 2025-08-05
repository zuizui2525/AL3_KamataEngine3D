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
	// ファイル名を指定してテクスチャハンドルを読み込む
	textureHandle_ = TextureManager::Load("./Resources/cube/cube.jpg");
	// カメラの初期化
	camera_.Initialize();
	// デバックカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
	// 3Dモデルの生成
	modelBlock_ = Model::Create();
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
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 18);
	modelPlayer_ = Model::CreateFromOBJ("PatiDigda", true);
	player_ = new Player();
	player_->Initialize(modelPlayer_, &camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);
	// コントロールカメラ
	cameraController_ = new CameraController();
	cameraController_->SetMovableArea(
	    {mapChipField_->GetMapChipPositionByIndex(5, 0).x, mapChipField_->GetMapChipPositionByIndex(94, 0).x, mapChipField_->GetMapChipPositionByIndex(0, 16).y,
	     mapChipField_->GetMapChipPositionByIndex(0, 16).y});
	cameraController_->SetTarget(player_);
	cameraController_->SetCamera(&camera_);
	cameraController_->Initialize();
	cameraController_->Reset();
	// 敵
	modelEnemy_ = Model::CreateFromOBJ("enemy", true);
	for (int32_t i = 0; i < 3; ++i) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(i*2 + 24, 18);
		newEnemy->Initialize(modelEnemy_, &camera_, enemyPosition);

		enemies_.push_back(newEnemy);
	}
}

// ゲームの更新処理
void GameScene::Update() {
#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_R)) {
		if (isDebugCameraActive_ == true) {
			isDebugCameraActive_ = false;
		} else {
			isDebugCameraActive_ = true;
		}
	}
#endif
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

	// 当たり判定
	CheckAllCollisions();
}

// 描画処理
void GameScene::Draw() {
	// モデル前処理
	Model::PreDraw(DirectXCommon::GetInstance()->GetCommandList());

	// 天球の描画
	skydome_->Draw();

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}

			// モデルの描画
			modelBlock_->Draw(*worldTransformBlock, camera_);
		}
	}

	// プレイヤーの描画
	player_->Draw();

	// 敵の描画
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	// モデル後処理
	Model::PostDraw();
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
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipField::MapChipType::kBlock) {
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
}

#include "GameScene.h"
#include "Function.h"

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
	// プレイヤーの解放
	delete player_;
	delete modelPlayer_;
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
	// カメラの処理
	if (isDebugCameraActive_) {
		// デバックカメラの更新
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		// ビュー行列の転送
		camera_.TransferMatrix();
	} else {
		// ビュープロジェクション行列の更新と転送
		camera_.UpdateMatrix();
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
	// 天球
	skydome_->Update();
	// プレイヤー
	player_->Update();
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
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

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
	// 要素数
	const uint32_t kNumBlockHorizontal = 20; // 横方向のブロック数
	const uint32_t kNumBlockVertical = 20;   // 縦方向のブロック数
	// ブロック1個分の横幅
	const float kBlockWidth = 2.0f;  // 1ブロックの幅
	const float kBlockHeight = 2.0f; // 1ブロックの高さ
	// 要素数を変更する
	worldTransformBlocks_.resize(kNumBlockVertical);
	for (uint32_t i = 0; i < kNumBlockVertical; ++i) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}
	// キューブの生成
	for (uint32_t i = 0; i < kNumBlockVertical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			// ワールドトランスフォームの生成
			if ((i + j) % 2 == 0) {
				worldTransformBlocks_[i][j] = new WorldTransform();
			} else {
				worldTransformBlocks_[i][j] = nullptr;
				continue;
			}
			// ワールドトランスフォームの初期化
			worldTransformBlocks_[i][j]->Initialize();
			// ワールドトランスフォームの位置を設定
			worldTransformBlocks_[i][j]->translation_.x = kBlockWidth * j;
			worldTransformBlocks_[i][j]->translation_.y = kBlockHeight * i;
		}
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

	// モデル後処理
	modelBlock_->PostDraw();
}
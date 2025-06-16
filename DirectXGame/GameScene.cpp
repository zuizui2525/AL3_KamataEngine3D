#include "GameScene.h"

// KamataEngineの名前空間を使用
using namespace KamataEngine;

// デストラクタ
GameScene::~GameScene() {
	// 3Dモデルの解放
	delete modelBlock_;
	// ブロック用のワールドトランスフォームの解放
	for (WorldTransform* worldTransformBlock : worldTransformBlocks_) {
		delete worldTransformBlock;
	}
	worldTransformBlocks_.clear();
}

// 初期化処理
void GameScene::Initialize() {
	// ファイル名を指定してテクスチャハンドルを読み込む
	textureHandle_ = TextureManager::Load("Resources/cube/cube.jpg");
	// 3Dモデルの生成
	modelBlock_ = Model::Create();
	// 要素数
	const uint32_t kNumBlockHorizontal = 20; // 横方向のブロック数
	// ブロック1個分の横幅
	const float kBlockWidth = 2.0f; // 1ブロックの幅
	// 要素数を変更する
	worldTransformBlocks_.resize(kNumBlockHorizontal);
	// キューブの生成
	for (uint32_t i = 0; i < kNumBlockHorizontal; ++i) {
		// ワールドトランスフォームの生成
		worldTransformBlocks_[i] = new WorldTransform();
		// ワールドトランスフォームの初期化
		worldTransformBlocks_[i]->Initialize();
		// ワールドトランスフォームの位置を設定
		worldTransformBlocks_[i]->translation_.x = kBlockWidth * i;
		worldTransformBlocks_[i]->translation_.y = 0.0f; 
	}
}

// ゲームの更新処理
void GameScene::Update() {
	
}

// 描画処理
void GameScene::Draw() {

}
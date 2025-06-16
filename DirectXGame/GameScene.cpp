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
}

// ゲームの更新処理
void GameScene::Update() {
	
}

// 描画処理
void GameScene::Draw() {

}
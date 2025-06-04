#include "GameScene.h"

// KamataEngineの名前空間を使用
using namespace KamataEngine;

// デストラクタ
GameScene::~GameScene() { 
	delete model_;
}

// 初期化処理
void GameScene::Initialize() {
	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("uvChecker.png");
	// 3Dモデルの生成
	model_ = Model::Create();
	// カメラの初期化
	camera_.Initialize();
}

// ゲームの更新処理
void GameScene::Update() {}

// 描画処理
void GameScene::Draw() {}
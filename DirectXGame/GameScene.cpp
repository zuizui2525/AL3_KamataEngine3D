#include "GameScene.h"

// KamataEngineの名前空間を使用
using namespace KamataEngine;

// デストラクタ
GameScene::~GameScene() { 
	// モデルの解放
	delete model_;
	// 自キャラの解放
	delete player_;
}

// 初期化処理
void GameScene::Initialize() {
	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("uvChecker.png");
	// 3Dモデルの生成
	model_ = Model::Create();
	// カメラの初期化
	camera_.Initialize();
	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(model_, textureHandle_, &camera_);
}

// ゲームの更新処理
void GameScene::Update() {
	// 自キャラの更新
	player_->Update();
}

// 描画処理
void GameScene::Draw() {
	// 自キャラの描画
	player_->Draw();
}
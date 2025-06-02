#include "GameScene.h"

// KamataEngineの名前空間を使用
using namespace KamataEngine;

// 初期化処理
void GameScene::Initialize() {
	// DirectX共通クラスのインスタンスを取得
	dxCommon = DirectXCommon::GetInstance();

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("uvChecker.png");
	// スプライトインスタンスの生成
	sprite_ = Sprite::Create(textureHandle_, {100, 50});
}

// ゲームの更新処理
void GameScene::Update() {
	// スプライトの今の座標を取得
	Vector2 position = sprite_->GetPosition();
	// 座標を{ 2, 1 }移動
	position.x += 2.0f;
	position.y += 1.0f;
	// 移動した座標をスプライトに反動
	sprite_->SetPosition(position);
}

// 描画処理
void GameScene::Draw() {
	// スプライト描画前描画
	Sprite::PreDraw(dxCommon->GetCommandList());

	sprite_->Draw();

	// スプライト描画後描画
	Sprite::PostDraw();
}

// コンストラクタ
GameScene::GameScene() {}

// デストラクタ
GameScene::~GameScene() { delete sprite_; }
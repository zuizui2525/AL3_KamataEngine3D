#include "GameScene.h"

// KamataEngineの名前空間を使用
using namespace KamataEngine;

// 初期化処理
void GameScene::Initialize() {
	//DirectXCommonインスタンスの取得
	dxCommon_ = DirectXCommon::GetInstance();
	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("uvChecker.png");
	// サウンドデータハンドル
	soundDataHandle_ = Audio::GetInstance()->LoadWave("fanfare.wav");
	// スプライトインスタンスの生成
	sprite_ = Sprite::Create(textureHandle_, {100, 50});
	// 3Dモデルの生成
	model_ = Model::Create();
	// 音声再生
	voiceHandle_ = Audio::GetInstance()->PlayWave(soundDataHandle_, true);
	// ワールドトランスフォーム
	worldTransform_.Initialize();
	// カメラの初期化
	camera_.Initialize();
	// ライン描画が参照するカメラを指定する(アドレス渡し)
	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);
	// デバックカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
	// 軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定する(アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());
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
	// スペースキーを押した瞬間
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		// 音声停止
		Audio::GetInstance()->StopWave(voiceHandle_);
	}
	// デバックカメラの更新
	debugCamera_->Update();
}

// 描画処理
void GameScene::Draw() {
	// スプライト描画前描画
	Sprite::PreDraw(dxCommon_->GetCommandList());
	// スプライト描画
	sprite_->Draw();
	// スプライト描画後描画
	Sprite::PostDraw();

	// 3Dモデル描画前描画
	Model::PreDraw(dxCommon_->GetCommandList());
	// 3Dモデル描画
	model_->Draw(worldTransform_, debugCamera_->GetCamera(), textureHandle_);
	// 3Dモデル描画後描画
	Model::PostDraw();

	// ラインを描画する
	//PrimitiveDrawer::GetInstance()->DrawLine3d({0, 0, 0}, {0, 10, 0}, {1.0f, 0.0f, 0.0f, 1.0f});
}

// コンストラクタ
GameScene::GameScene() {}

// デストラクタ
GameScene::~GameScene() {
	// スプライトの解放処理
	delete sprite_;
	// モデルの解放処理
	delete model_;
	// デバックカメラの解放処理
	delete debugCamera_;
}
#include "TitleScene.h"

TitleScene::~TitleScene() {
	delete fade_;
	delete modelSkydome_;
	delete skydome_;
	delete title_;
}

void TitleScene::Initialize() {
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, fadeTime);
	textureHandleTitle_ = TextureManager::Load("./Resources/title.png");
	// title
	title_ = Sprite::Create(textureHandleTitle_, {0,0});
	// カメラの初期化
	camera_.Initialize();
	// 天球の初期化
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);
}

void TitleScene::Update() {
	switch (phase_) {
	case Phase::kFadeIn:
		// フェード
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;
	case Phase::kMain:
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, fadeTime);
		}
		break;
	case Phase::kFadeOut:
		// フェード
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}
	// ビュープロジェクション行列の更新と転送
	camera_.UpdateMatrix();
	// ビュー行列の転送
	camera_.TransferMatrix();
	// 天球
	skydome_->Update();
}

void TitleScene::Draw() {
	// モデル前処理
	Model::PreDraw(DirectXCommon::GetInstance()->GetCommandList());
	// 天球
	skydome_->Draw();
	// フェード
	fade_->Draw();
	// モデル後処理
	Model::PostDraw();
	// スプライト前処理
	Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());
	// title
	title_->Draw();
	// スプライト後処理
	Sprite::PostDraw();
}
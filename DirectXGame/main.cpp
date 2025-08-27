#include <Windows.h>
#include "KamataEngine.h"
#include "GameScene.h"
#include "TitleScene.h"

// KamataEngineの名前空間を使用
using namespace KamataEngine;

TitleScene* titleScene = nullptr;
GameScene* gameScene = nullptr;

enum class Scene {
	kUnknown = 0,
	kTitle,
	kGame,
};

// 現在シーン（型）
Scene scene = Scene::kUnknown;

void ChangeScene();
void UpdateScene();
void DrawScene();

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//エンジンの初期化
	KamataEngine::Initialize(L"LE2B_03_イトウ_カズイ_AL3");
	
	scene = Scene::kTitle;
	titleScene = new TitleScene();
	titleScene->Initialize();

	//DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	// ImGuiManagerインスタンス
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	//メインループ!
	while (true) {
		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}

		///
		/// ↓↓↓　更新処理　↓↓↓
		///

		// ImGui受付開始
		imguiManager->Begin();
		
		// シーン切り替え
		ChangeScene();
		// 更新
		UpdateScene();

		// ImGui受付終了
		imguiManager->End();

		///
		/// ↑↑↑　更新処理　↑↑↑
		/// 

		//描画開始
		dxCommon->PreDraw();
		// モデル前処理
		Model::PreDraw(DirectXCommon::GetInstance()->GetCommandList());
		
		///
		/// ↓↓↓　描画処理　↓↓↓
		/// 

		// 描画
		DrawScene();

		// 軸表示の描画
		AxisIndicator::GetInstance()->Draw();
		// ImGui描画
		imguiManager->Draw();

		///
		/// ↑↑↑　描画処理　↑↑↑
		/// 
		
		// モデル後処理
		Model::PostDraw();
		// 描画終了
		dxCommon->PostDraw();
	}

	// 解放
	delete titleScene;
	delete gameScene;

	// ゲームシーンのポインタをnullptrにする
	gameScene = nullptr;

	// エンジンの終了処理
	KamataEngine::Finalize();

	// アプリケーションの終了
	return 0;
}

void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene->GetFinished()) {
			// シーン変更
			scene = Scene::kGame;
			// 旧シーンの解放
			delete titleScene;
			titleScene = nullptr;
			// 新シーンの生成と初期化
			gameScene = new GameScene();
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		if (gameScene->GetFinished()) {
			// シーン変更
			scene = Scene::kTitle;
			// 旧シーンの解放
			delete gameScene;
			gameScene = nullptr;
			// 新シーンの生成と初期化
			titleScene = new TitleScene();
			titleScene->Initialize();
		}
		break;
	}
}

void UpdateScene() {
	// 更新
	switch (scene) {
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	}
}

void DrawScene() {
	// 描画
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	}
}

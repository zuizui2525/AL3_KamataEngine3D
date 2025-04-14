#include <Windows.h>
#include "KamataEngine.h"
#include "GameScene.h"

// KamataEngineの名前空間を使用
using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//エンジンの初期化
	KamataEngine::Initialize(L"LE2B_03_イトウ_カズイ_AL3");
	
	//ゲームシーンのインスタンス生成
	GameScene* gameScene = new GameScene();
	// ゲームシーンの初期化
	gameScene->Initialize();
	
	//DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	//メインループ!
	while (true) {
		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}

		///
		/// ↓↓↓　更新処理　↓↓↓
		///

		// ゲームシーンの更新
		gameScene->Update();

		///
		/// ↑↑↑　更新処理　↑↑↑
		/// 

		//描画開始
		dxCommon->PreDraw();
		
		///
		/// ↓↓↓　描画処理　↓↓↓
		/// 

		// ゲームシーンの描画
		gameScene->Draw();

		///
		/// ↑↑↑　描画処理　↑↑↑
		/// 
		
		// 描画終了
		dxCommon->PostDraw();
	}

	// ゲームシーンの解放
	delete gameScene;
	// ゲームシーンのポインタをnullptrにする
	gameScene = nullptr;

	// エンジンの終了処理
	KamataEngine::Finalize();

	// アプリケーションの終了
	return 0;
}

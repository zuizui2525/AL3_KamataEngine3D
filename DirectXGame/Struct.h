#pragma once
#include "KamataEngine.h"

// KamataEngineの名前空間を使用
using namespace KamataEngine;

// 画面サイズ
const int kWindowWidth = 1280;
const int kWindowHeight = 720;

struct Segment{
	Vector3 origin; //!< 始点
	Vector3 diff; //!< 終点への差分ベクトル
	unsigned int color; //!< 色
};

struct Triangle {
	Vector3 vertex[3]; //!< 頂点
	unsigned int color; //!< 色
};

struct Plane {
	Vector3 normal; //!< 法線
	float distance; //!< 距離
	unsigned int color; //!< 色
};

struct Sphere {
	Vector3 center; //!< 中心点
	float radius; //!< 半径
	unsigned int color; //!< 色
};

struct AABB {
	Vector3 min; //!< 最小点
	Vector3 max; //!< 最大点
	unsigned int color; //!< 色
};

struct OBB {
	Vector3 center; //!< 中心点
	Vector3 orientations[3]; //!< 座標軸「。正規化・直交必須
	Vector3 size; //!< 座標軸方向の長さの半分。中心から面までの距離
	unsigned int color; //!< 色
};

struct Spring {
	Vector3 ancher;           // アンカー。固定された端の位置
	float naturalLength;      // 自然長
	float stiffness;          // 剛性。ばね定数k
	float dampingCoefficient; // 減衰係数
};

struct Ball {
	Vector3 position;     // ボールの位置
	Vector3 velocity;     // ボールの速度
	Vector3 acceleration; // ボールの加速度
	float mass;           // ボールの質量
	float radius;         // ボールの半径
	unsigned int color;   // ボールの色
};

struct Pendulum {
	Vector3 anchor;            // アンカーポイント。固定された端の位置
	float length;              // 紐の長さ
	float angle;               // 現在の角度
	float angularVelocity;     // 角速度ω
	float angularAcceleration; // 角加速度
};

struct ConicalPendulum {
	Vector3  anchor;       // アンカーポイント。固定された端の位置
	float length;          // 紐の長さ
	float halfApexAngle;   // 円錐の頂点の半分
	float angle;           // 現在の速度
	float angularVelocity; // 角速度ω
};

struct Capsule {
	Segment segment;
	float radius;
};
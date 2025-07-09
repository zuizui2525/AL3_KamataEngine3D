#pragma once
#include "KamataEngine.h"
#include <cstdint>
#include <vector>

enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

/// <summary>
/// マップチップフィールド
/// </summary>
class MapChipField {
private:
	// 1ブロックのサイズ
	static inline const float kBlockWidth_ = 2.0f;
	static inline const float kBlockHeight_ = 2.0f;
	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical_ = 20;
	static inline const uint32_t kNumBlockHorizontal_ = 100;
	MapChipData mapChipData_;
public:
	uint32_t GetNumBlockVirtical() { return kNumBlockVirtical_; };
	uint32_t GetNumBlockHorizontal() { return kNumBlockHorizontal_; };
	void ResetMapChipData();
	void LoadMapChipCsv(const std::string& filePath);
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);
};

#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../AssetUtil.h"
#include "Keyframe.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Any/Any.h>
#include <Lib/Logger/StreamLogger.h>

//* c++
#include <algorithm>
#include <vector>
#include <any>
#include <string>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// Animation structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Animation final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Curve structure
	////////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	struct Curve final {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// Segument structure
		////////////////////////////////////////////////////////////////////////////////////////////
		struct Segument {
		public:

			//=========================================================================================
			// public variables
			//=========================================================================================

			T start;  //!< 区間の始点キーの値
			T end;    //!< 区間の終点キーの値
			double t; //!< 補間係数 [0.0 ~ 1.0]

		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @brief アニメーションの区間時間を取得する
		TimePointd<TimeUnit::Second> GetDuration() const;

		//! @brief 指定した時間における補間値を取得する
		Segument GetSegument(TimePointd<TimeUnit::Second> time) const;

		//! @brief キーフレームを時間順にソートする
		static void Sort(std::vector<Keyframe<T>>& keyframes);

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::vector<Keyframe<T>> keyframes;
		//!< キーフレームの配列 (時間順でsortされていることを前提)

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	template <typename T>
	bool HasCurve(const std::string& name) const;

	template <typename T>
	void SetCurve(const std::string& name, const Curve<T>& curve);

	template <typename T>
	void SetCurve(const std::string& name, Curve<T>&& curve);

	template <typename T>
	const Curve<T>& GetCurve(const std::string& name) const;

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::unordered_map<std::string, Any> curves;
	//!< stringをキーにして, Curve<T>をAnyで保持するmap

};

////////////////////////////////////////////////////////////////////////////////////////////
// [Animation] Curve structure methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline TimePointd<TimeUnit::Second> Animation::Curve<T>::GetDuration() const {
	if (keyframes.empty()) {
		return { 0 }; //!< キーフレームが空の場合は0を返す
	}

	return keyframes.back().time; //!< 最後のキーフレームの時刻を返す
}

template <typename T>
inline Animation::Curve<T>::Segument Animation::Curve<T>::GetSegument(TimePointd<TimeUnit::Second> time) const {
	STREAM_ASSERT(!keyframes.empty(), "keyframes is empty.");

	Segument segument = {};

	if (time <= keyframes.front().time) {
		//!< timeが最初のキーフレームより前の場合は, 最初のキーフレームの値を返す
		segument.start = segument.end = keyframes.front().value;
		segument.t     = 0.0;

		return segument;
	}

	if (time >= keyframes.back().time) {
		//!< timeが最後のキーフレームより後の場合は, 最後のキーフレームの値を返す
		segument.start = segument.end = keyframes.back().value;
		segument.t     = 0.0;

		return segument;
	}

	//!< 二分木探索で, (time <= next)となるキーフレームを見つける
	auto next = std::upper_bound(
		keyframes.begin(), keyframes.end(),
		time,
		[](const TimePointd<TimeUnit::Second>& t, const Keyframe<T>& key) { return t < key.time; }
	);

	auto prev = std::prev(next);

	//!< 補間係数tを計算する
	TimePointd<TimeUnit::Second> duration = (next->time - prev->time);
	double t = (time - prev->time).time / duration.time;

	segument.start = prev->value;
	segument.end   = next->value;
	segument.t     = t;

	return segument;
}

template <typename T>
inline void Animation::Curve<T>::Sort(std::vector<Keyframe<T>>& keyframes) {
	std::sort(keyframes.begin(), keyframes.end(), [](const Keyframe<T>& a, const Keyframe<T>& b) { return a.time < b.time; });
}

////////////////////////////////////////////////////////////////////////////////////////////
// Animation structure template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline bool Animation::HasCurve(const std::string& name) const {
	return curves.contains(name);
}

template <typename T>
inline void Animation::SetCurve(const std::string& name, const Animation::Curve<T>& curve) {
	curves[name] = curve;
}

template <typename T>
inline void Animation::SetCurve(const std::string& name, Animation::Curve<T>&& curve) {
	curves[name] = std::move(curve);
}

template <typename T>
inline const Animation::Curve<T>& Animation::GetCurve(const std::string& name) const {
	STREAM_ASSERT(curves.contains(name), "Animation::GetCurve | curve not found. name: {}", name);
	return curves.at(name).Get<Animation::Curve<T>>();
}

SXAVENGER_ENGINE_NAMESPACE_END

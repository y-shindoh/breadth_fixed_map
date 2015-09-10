/* -*- coding: utf-8; tab-width: 4 -*- */
/**
 * @file	breadth_fixed_map.hpp の動作確認コマンド
 * @author	Yasutaka SHINDOH / 新堂 安孝
 */

#include <cstddef>
#include <cstdio>
#include <string>
#include "breadth_fixed_map.hpp"

#define	LENGTH	8
#define	KEY_FORMAT	"TEST[%d]"

int
main()
{
	ys::BreadthFixedMap<std::string, size_t> map((size_t)LENGTH);
	char key[16];

	//////////////////////////////
	// 要素を許容範囲の2倍挿入

	for (int i(0); i < LENGTH * 2; ++i) {
		std::sprintf(key, KEY_FORMAT, i);
		map.add(key, i);
	}

	std::printf("SIZE:\t%lu\n", map.size());

	for (int i(0); i < LENGTH * 2; ++i) {
		std::sprintf(key, KEY_FORMAT, i);
		if (!map.find(key)) continue;
		std::printf("%s:\t%lu\n", key, map.get(key));
	}

	//////////////////////////////
	// 要素の値を上書き

	for (int i(LENGTH); i < LENGTH * 2; ++i) {
		std::sprintf(key, KEY_FORMAT, i);
		map.add(key, i * 10);
	}

	std::printf("SIZE:\t%lu\n", map.size());

	for (int i(0); i < LENGTH * 2; ++i) {
		std::sprintf(key, KEY_FORMAT, i);
		if (!map.find(key)) continue;
		std::printf("%s:\t%lu\n", key, map.get(key));
	}

	//////////////////////////////
	// 要素を4つ削除

	for (int i(LENGTH + 1); LENGTH <= i; --i) {
		std::sprintf(key, KEY_FORMAT, i);
		map.remove(key);
	}

	for (int i(LENGTH * 2 - 2); i < LENGTH * 2; ++i) {
		std::sprintf(key, KEY_FORMAT, i);
		map.remove(key);
	}

	std::printf("SIZE:\t%lu\n", map.size());

	for (int i(0); i < LENGTH * 2; ++i) {
		std::sprintf(key, KEY_FORMAT, i);
		if (!map.find(key)) continue;
		std::printf("%s:\t%lu\n", key, map.get(key));
	}

	return 0;
}

/* -*- coding: utf-8; tab-width: 4 -*- */
/**
 * 一定数の要素のみを保持するハッシュ・マップ
 * @file	breadth_fixed_map.hpp
 * @author	Yasutaka SHINDOH / 新堂 安孝
 */

#ifndef	__BREADTH_FIXED_MAP_HPP__
#define	__BREADTH_FIXED_MAP_HPP__	"breadth_fixed_map.hpp"

#include <cstddef>
#include <cassert>
#include <unordered_map>

namespace ys
{
	/**
	 * 一定数の要素のみを保持するハッシュ・マップ
	 * @note	要素数が一定数を超えた場合、最終アクセスの古い順で要素を削除する。
	 * @note	テンプレートの型 @a KEY はマップのキーを示す。
	 * @note	テンプレートの型 @a VALUE はマップの値の実体を示す。
	 */
	template<typename KEY, typename VALUE>
	class BreadthFixedMap
	{
	private:

		/**
		 * ハッシュ・マップの値を持つ双方向リストのノード
		 */
		template<typename K, typename V>
		class Node
		{
		public:

			K key_;		///< キー
			V value_;	///< 値
			Node<K, V>* previous_;	///< 前のノード
			Node<K, V>* next_;		///< 次のノード

			/**
			 * コンストラクタ
			 * @param[in]	key	キー
			 * @param[in]	value	値
			 */
			Node(const K& key,
				 const V& value)
				: key_(key), value_(value),
				  previous_(0), next_(0)
				{
					;
				}
		};

		std::unordered_map<KEY, Node<KEY, VALUE>*> table_;	///< テーブル
		Node<KEY, VALUE>* head_;	///< キューの先頭
		Node<KEY, VALUE>* tail_;	///< キューの末尾
		Node<KEY, VALUE>* buffer_;	///< 未使用ノード
		size_t length_;				///< キューの要素数
		const size_t MAX_;			///< キューの最大要素数

		/**
		 * 未使用ノードを取得
		 * @param[in]	key	キー
		 * @param[in]	value	値
		 * @return	未使用ノード (取得失敗時は 0 が返る)
		 */
		Node<KEY, VALUE>*
		new_node(const KEY& key,
				 const VALUE& value)
			{
				Node<KEY, VALUE>* node;

				if (buffer_) {
					node = buffer_;
					buffer_ = node->next_;
					if (buffer_) buffer_->previous_ = 0;
					node->key_ = key;
					node->value_ = value;
				}
				else {
					try {
						node = new Node<KEY, VALUE>(key, value);
					}
					catch (...) {
						assert(false);
						return 0;
					}
				}

				++length_;
				node->previous_ = node->next_ = 0;

				return node;
			}

		/**
		 * 使用中ノードを切り離す
		 * @param[in,out]	node	処理対象のノード
		 */
		void
		pick_up_node(Node<KEY, VALUE>* node)
			{
				if (node->previous_) {
					node->previous_->next_ = node->next_;
				}
				else {
					head_ = node->next_;
					if (head_) head_->previous_ = 0;
				}

				if (node->next_) {
					node->next_->previous_ = node->previous_;
				}
				else {
					tail_ = node->previous_;
					if (tail_) tail_->next_ = 0;
				}

				node->previous_ = node->next_ = 0;
			}

		/**
		 * 使用中ノードを除去
		 * @param[in,out]	node	除去対象のノード
		 * @note	ノードを使用中状態から未使用状態に変更するだけ。
		 */
		void
		delete_node(Node<KEY, VALUE>* node)
			{
				assert(node);

				pick_up_node(node);

				if (buffer_) buffer_->previous_ = node;
				node->next_ = buffer_;
				buffer_ = node;
				--length_;
			}

		/**
		 * ノードをテーブルから除去
		 * @param[in,out]	node	除去対象のノード
		 */
		void
		remove(Node<KEY, VALUE>* node)
			{
				assert(node);

				delete_node(node);
				table_.erase(node->key_);
			}

	public:

		/**
		 * コンストラクタ
		 * @param[in]	max	ハッシュ・マップが保持する最大ノード数
		 */
		BreadthFixedMap(size_t max = 1024)
			: head_(0), tail_(0), buffer_(0),
			  length_(0), MAX_(max)
			{
				;
			}

		/**
		 * デストラクタ
		 */
		virtual
		~BreadthFixedMap()
			{
				Node<KEY, VALUE>* b[] = {head_, buffer_};
				Node<KEY, VALUE>* c;
				Node<KEY, VALUE>* n;

				for (size_t i(0); i < sizeof(b) / sizeof(b[0]); ++i) {
					c = b[i];
					while (c) {
						n = c->next_;
						delete c;
						c = n;
					}
				}
			}

		/**
		 * ハッシュ・マップの要素数を取得
		 * @return	ハッシュ・マップの要素数
		 */
		size_t
		size() const
			{
				return length_;
			}

		/**
		 * ハッシュ・マップを探索
		 * @param[in]	key	キー
		 * @return	true: キーが見つかった, false: キーが見つからなかった
		 */
		bool
		find(const KEY& key) const
			{
				return table_.find(key) != table_.end();
			}

		/**
		 * ハッシュ・マップから値を取得
		 * @param[in]	key	キー
		 * @return	キーに対応する値
		 * @note	事前に @a find を用いてキーがハッシュ・マップ中にあるか確認すること。
		 */
		VALUE
		get(const KEY& key)
			{
				typename std::unordered_map<KEY, Node<KEY, VALUE>*>::iterator it;
				it = table_.find(key);

				assert(it != table_.end());

				Node<KEY, VALUE>* node(it->second);

				pick_up_node(node);
				node->next_ = head_;
				if (head_) head_->previous_ = node;
				head_ = node;

				return node->value_;
			}

		/**
		 * ハッシュ・マップからキーと値を除去
		 * @param[in]	key	キー
		 * @note	キーがなければ何もしない。
		 * @note	必要であれば、事前に値の除去処理を行っておくこと。
		 */
		void
		remove(const KEY& key)
			{
				typename std::unordered_map<KEY, Node<KEY, VALUE>*>::iterator it;
				it = table_.find(key);

				if (it == table_.end()) return;

				remove(it->second);
			}

		/**
		 * ハッシュ・マップに要素を追加
		 * @param[in]	key	キー
		 * @param[in]	value	要素
		 * @note	すでに登録されているキーに対しては値を更新する。
		 */
		void
		add(const KEY& key,
			const VALUE& value)
			{
				Node<KEY, VALUE>* node;
				typename std::unordered_map<KEY, Node<KEY, VALUE>*>::iterator it;
				it = table_.find(key);

				if (it != table_.end()) {
					node = it->second;
					pick_up_node(node);
					node->value_ = value;
				}
				else {
					while (MAX_ <= length_ && tail_) remove(tail_);
					node = new_node(key, value);
					table_.insert(std::pair<KEY, Node<KEY, VALUE>*>(key, node));
				}

				node->next_ = head_;
				if (head_) head_->previous_ = node;
				head_ = node;
				if (!tail_) tail_ = node;
			}
	};
};

#endif	// __BREADTH_FIXED_MAP_HPP__

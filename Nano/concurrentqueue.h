#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>
#include <atomic>
#include <chrono>
#include <string>

#include "nocopyable.h"

namespace Nano {
	namespace Concurrency {
		template <typename T>
		class ConcurrentQueue :public Noncopyable
		{
		private:
			struct node
			{
				std::shared_ptr<T> data;
				std::unique_ptr<node> next;
				node* prev;
				
				node() : prev(nullptr) {}
			};

			std::mutex m_head_mutex;
			std::unique_ptr<node> m_head;
			std::mutex m_tail_mutex;
			node* m_tail;
			std::condition_variable m_data_cond;
			std::atomic_bool m_bstop;
			std::atomic_int m_count;

			node* get_tail()
			{
				std::lock_guard<std::mutex> tail_lock(m_tail_mutex);
				return m_tail;
			}

			std::unique_ptr<node> pop_head()
			{
				std::unique_ptr<node> old_head = std::move(m_head);
				m_head = std::move(old_head->next);
				m_count.fetch_sub(1);
				return old_head;
			}

			std::unique_lock<std::mutex> wait_for_data()
			{
				std::unique_lock<std::mutex> head_lock(m_head_mutex);
				m_data_cond.wait(head_lock, [&] {return m_head.get() != get_tail() || m_bstop.load() == true; });
				return std::move(head_lock);
			}

			std::unique_ptr<node> wait_pop_head()
			{
				std::unique_lock<std::mutex> head_lock(wait_for_data());
				if (m_bstop.load()) {
					return nullptr;
				}

				return pop_head();
			}

			std::unique_ptr<node> wait_pop_head(T& value)
			{
				std::unique_lock<std::mutex> head_lock(wait_for_data());
				if (m_bstop.load()) {
					return nullptr;
				}
				value = std::move(*m_head->data);
				return pop_head();
			}

			std::unique_ptr<node> try_pop_head()
			{
				std::lock_guard<std::mutex> head_lock(m_head_mutex);
				if (m_head.get() == get_tail())
				{
					return std::unique_ptr<node>();
				}
				return pop_head();
			}

			std::unique_ptr<node> try_pop_head(T& value)
			{
				std::lock_guard<std::mutex> head_lock(m_head_mutex);
				if (m_head.get() == get_tail())
				{
					return std::unique_ptr<node>();
				}
				value = std::move(*m_head->data);
				return pop_head();
			}
		public:
			ConcurrentQueue() :
				m_head(new node), m_tail(m_head.get()), m_count(0), m_bstop(false)
			{}

			~ConcurrentQueue() {
				m_bstop.store(true);
				m_data_cond.notify_all();
			}

			void Exit() {
				m_bstop.store(true);
				m_data_cond.notify_all();
			}

			bool wait_and_pop_timeout(T& value) {
				std::unique_lock<std::mutex> head_lock(m_head_mutex);
				auto res = m_data_cond.wait_for(head_lock, std::chrono::milliseconds(100),
					[&] {return m_head.get() != get_tail() || m_bstop.load() == true; });
				if (res == false) {
					return false;
				}

				if (m_bstop.load()) {
					return false;
				}
				value = std::move(*m_head->data);
				m_head = std::move(m_head->next);
				m_count.fetch_sub(1);
				return true;
			}

			std::shared_ptr<T> wait_and_pop()
			{
				std::unique_ptr<node> const old_head = wait_pop_head();
				if (old_head == nullptr) {
					return nullptr;
				}
				return old_head->data;
			}

			bool wait_and_pop(T& value)
			{
				std::unique_ptr<node> const old_head = wait_pop_head(value);
				if (old_head == nullptr) {
					return false;
				}
				return true;
			}

			std::shared_ptr<T> try_pop()
			{
				std::unique_ptr<node> old_head = try_pop_head();
				return old_head ? old_head->data : std::shared_ptr<T>();
			}

			bool try_pop(T& value)
			{
				std::unique_ptr<node> const old_head = try_pop_head(value);
				if (old_head) {
					return true;
				}
				return false;
			}

			bool empty()
			{
				std::lock_guard<std::mutex> head_lock(m_head_mutex);
				return (m_head.get() == get_tail());
			}

			void push(T new_value)
			{
				std::shared_ptr<T> new_data(
					std::make_shared<T>(std::move(new_value)));
				std::unique_ptr<node> p(new node);
				{
					std::lock_guard<std::mutex> tail_lock(m_tail_mutex);
					m_tail->data = new_data;
					node* const new_tail = p.get();
					new_tail->prev = m_tail;

					m_tail->next = std::move(p);

					m_tail = new_tail;
				}
				m_count.fetch_add(1);
				m_data_cond.notify_one();
			}

			bool try_steal(T& value) {
				std::unique_lock<std::mutex> tail_lock(m_tail_mutex, std::defer_lock);
				std::unique_lock<std::mutex> head_lock(m_head_mutex, std::defer_lock);
				std::lock(tail_lock, head_lock);
				if (m_head.get() == m_tail)
				{
					return false;
				}
				node* prev_node = m_tail->prev;
				value = std::move(*(prev_node->data));
				m_tail = prev_node;
				m_tail->next = nullptr;
				m_count.fetch_sub(1);
				return true;
			}
		};
	}
}
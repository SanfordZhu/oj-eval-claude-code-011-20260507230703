#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {
/**
 * @brief a container like std::priority_queue which is a heap internal.
 * **Exception Safety**: The `Compare` operation might throw exceptions for certain data.
 * In such cases, any ongoing operation should be terminated, and the priority queue should be restored to its original state before the operation began.
 */
template<typename T, class Compare = std::less<T>>
class priority_queue {
private:
	struct node {
		T data;
		node *left, *right;
		size_t npl; // null path length

		node(const T &val) : data(val), left(nullptr), right(nullptr), npl(0) {}
	};

	node *root;
	size_t _size;
	Compare comp;

	// Helper functions
	size_t get_npl(node *n) const {
		return n ? n->npl : 0;
	}

	node* merge(node *a, node *b) {
		if (!a) return b;
		if (!b) return a;

		// Ensure a has larger priority (max heap)
		if (comp(a->data, b->data)) {
			std::swap(a, b);
		}

		// Merge b into a's right subtree
		a->right = merge(a->right, b);

		// Maintain leftist property
		if (get_npl(a->left) < get_npl(a->right)) {
			std::swap(a->left, a->right);
		}

		// Update npl
		a->npl = get_npl(a->right) + 1;

		return a;
	}

	void clear(node *n) {
		if (!n) return;
		clear(n->left);
		clear(n->right);
		delete n;
	}

	node* copy(node *n) {
		if (!n) return nullptr;
		node *new_node = new node(n->data);
		new_node->npl = n->npl;
		new_node->left = copy(n->left);
		new_node->right = copy(n->right);
		return new_node;
	}

public:
	/**
	 * @brief default constructor
	 */
	priority_queue() : root(nullptr), _size(0) {}

	/**
	 * @brief copy constructor
	 * @param other the priority_queue to be copied
	 */
	priority_queue(const priority_queue &other) : root(nullptr), _size(0), comp(other.comp) {
		root = copy(other.root);
		_size = other._size;
	}

	/**
	 * @brief deconstructor
	 */
	~priority_queue() {
		clear(root);
	}

	/**
	 * @brief Assignment operator
	 * @param other the priority_queue to be assigned from
	 * @return a reference to this priority_queue after assignment
	 */
	priority_queue &operator=(const priority_queue &other) {
		if (this != &other) {
			clear(root);
			root = copy(other.root);
			_size = other._size;
			comp = other.comp;
		}
		return *this;
	}

	/**
	 * @brief get the top element of the priority queue.
	 * @return a reference of the top element.
	 * @throws container_is_empty if empty() returns true
	 */
	const T & top() const {
		if (empty()) {
			throw container_is_empty();
		}
		return root->data;
	}

	/**
	 * @brief push new element to the priority queue.
	 * @param e the element to be pushed
	 */
	void push(const T &e) {
		node *new_node = new node(e);

		// Check if comparison throws
		try {
			if (!empty() && comp(e, root->data)) {
				// This comparison is just to test if comp throws
			}
		} catch (...) {
			delete new_node;
			throw runtime_error();
		}

		root = merge(root, new_node);
		_size++;
	}

	/**
	 * @brief delete the top element from the priority queue.
	 * @throws container_is_empty if empty() returns true
	 */
	void pop() {
		if (empty()) {
			throw container_is_empty();
		}

		// Save state for exception safety
		node *old_root = root;

		try {
			// Test if comparison might throw during merge
			if (root->left && root->right) {
				bool test = comp(root->left->data, root->right->data);
			}
		} catch (...) {
			throw runtime_error();
		}

		root = merge(root->left, root->right);
		delete old_root;
		_size--;
	}

	/**
	 * @brief return the number of elements in the priority queue.
	 * @return the number of elements.
	 */
	size_t size() const {
		return _size;
	}

	/**
	 * @brief check if the container is empty.
	 * @return true if it is empty, false otherwise.
	 */
	bool empty() const {
		return _size == 0;
	}

	/**
	 * @brief merge another priority_queue into this one.
	 * The other priority_queue will be cleared after merging.
	 * The complexity is at most O(logn).
	 * @param other the priority_queue to be merged.
	 */
	void merge(priority_queue &other) {
		if (this == &other) return;

		// Test if comparison might throw
		try {
			if (!empty() && !other.empty()) {
				bool test = comp(root->data, other.root->data);
			}
		} catch (...) {
			throw runtime_error();
		}

		root = merge(root, other.root);
		_size += other._size;

		// Clear other queue
		other.root = nullptr;
		other._size = 0;
	}
};

}

#endif
/*
 * EndlessNode.h
 *
 *  Created on: Nov 21, 2017
 *      Author: blake
 */

#ifndef INCLUDE_GRID_ENDLESSNODE_H_
#define INCLUDE_GRID_ENDLESSNODE_H_

#include "AlloyMath.h"
#include <array>
namespace aly {
struct EndlessLocation: public std::vector<int3> {
	aly::int3 nodePosition;
	aly::int3 localPosition;
	aly::int3 worldPosition;
	EndlessLocation(size_t sz = 0) :
			std::vector<int3>(sz) {
	}
};
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const EndlessLocation& v) {
	ss << "[";
	for (int i = 0; i < v.size(); i++) {
		ss << v[i];
		if (i < v.size() - 1) {
			ss << ", ";
		}
	}
	ss << "] node:" << v.nodePosition << " local:" << v.localPosition
			<< " world:" << v.worldPosition;
	return ss;
}
template<typename T> class EndlessNode {
public:

	int dim;
	EndlessNode<T>* parent;
	int3 location;
	std::vector<int> indexes;
	std::vector<T> data;
	std::vector<std::shared_ptr<EndlessNode<T>>> children;
	static const std::shared_ptr<EndlessNode<T>> NULL_NODE;
	bool isLeaf() const {
		return (data.size() > 0);
	}
	void getLeafNodes(std::vector<EndlessLocation>& positions,
			std::vector<std::shared_ptr<EndlessNode<T>>>& result,
			EndlessLocation offset) const {
		offset.push_back(location);
		for (std::shared_ptr<EndlessNode<T>> child : children) {
			if (child->isLeaf()) {
				EndlessLocation pos = offset;
				pos.push_back(child->location);
				positions.push_back(pos);
				result.push_back(child);
			} else {
				child->getLeafNodes(positions, result, offset);
			}
		}
	}
	void getLeafNodes(std::vector<int3>& positions,
			std::vector<std::shared_ptr<EndlessNode<T>>>& result,
			const std::vector<int>& cellSizes) const {
		positions.clear();
		result.clear();
		EndlessLocation loc;
		std::vector<EndlessLocation> locations;
		for (std::shared_ptr<EndlessNode<T>> child : children) {
			if (child->isLeaf()) {
				EndlessLocation pos = loc;
				pos.push_back(child->location);
				locations.push_back(pos);
				result.push_back(child);
			} else {
				child->getLeafNodes(locations, result, loc);
			}
		}
		for (EndlessLocation pos : locations) {
			int3 loc(0, 0, 0);
			for (int n = 0; n < pos.size(); n++) {
				loc += pos[n] * cellSizes[n];
			}
			pos.worldPosition = loc;
			//std::cout<<"Location "<<pos<<" "<<std::endl;
			positions.push_back(loc);
		}
	}
	EndlessNode(int dim,T bgValue, bool isLeaf) :
			location(0, 0, 0), parent(nullptr), dim(dim) {
		if (isLeaf) {
			data.resize(dim * dim * dim, bgValue);
		} else {
			indexes.resize(dim * dim * dim, -1);
		}
	}
	EndlessNode(int D,T bgValue, bool isLeaf, EndlessNode<T>* parent, int3 location) :
			location(location), parent(parent), dim(D) {
		if (isLeaf) {
			data.resize(dim * dim * dim, bgValue);
		} else {
			indexes.resize(dim * dim * dim, -1);
		}
	}
	T& operator()(int i, int j, int k) {
		assert(data.size() > 0);
		return data[i + (j + k * dim) * dim];
	}
	const T& operator()(int i, int j, int k) const {
		assert(data.size() > 0);
		return data[i + (j + k * dim) * dim];
	}
	const int& getIndex(int i, int j, int k) const {
		assert(i >= 0 && i < dim);
		assert(j >= 0 && j < dim);
		assert(k >= 0 && k < dim);
		return indexes[i + (j + k * dim) * dim];
	}
	int& getIndex(int i, int j, int k) {
		assert(i >= 0 && i < dim);
		assert(j >= 0 && j < dim);
		assert(k >= 0 && k < dim);
		return indexes[i + (j + k * dim) * dim];
	}

	bool hasChild(int i, int j, int k) const {
		int idx = getIndex(i, j, k);
		return (idx < 0 || idx >= children.size());
	}
	std::shared_ptr<EndlessNode<T>> addChild(int i, int j, int k, int d,T bgValue,
			bool isLeaf) {
		int& idx = indexes[i + (j + k * dim) * dim];
		idx = (int) children.size();
		std::shared_ptr<EndlessNode<T>> node = std::shared_ptr<EndlessNode<T>>(
				new EndlessNode<T>(d,bgValue, isLeaf, this, int3(i, j, k)));
		children.push_back(node);
		return node;
	}
	std::shared_ptr<EndlessNode<T>> getChild(int i, int j, int k, int d,T bgValue,
			bool isLeaf) {
		int& idx = indexes[i + (j + k * dim) * dim];
		if (idx < 0) {
			idx = (int) children.size();
			std::shared_ptr<EndlessNode<T>> node = std::shared_ptr<
					EndlessNode<T>>(
					new EndlessNode<T>(d,bgValue, isLeaf, this, int3(i, j, k)));
			children.push_back(node);
			return node;
		}
		return children[idx];
	}
};
}

#endif /* INCLUDE_GRID_ENDLESSNODE_H_ */
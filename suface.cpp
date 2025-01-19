#include <iostream>
#include <vector>
#include <cmath>

// 比较两个向量是否满足条件
bool compareVectors1(const std::vector<int>& vec1, const std::vector<int>& vec2) {
	// 使用一标志来提前停止循环
	for (size_t i = 0; i < vec1.size(); ++i) {
		for (size_t j = 0; j < vec2.size(); ++j) {
			if (std::fabs(vec1[i] - vec2[j]) > 5) {
				return true;  // 如果满足条件，直接返回
			}
		}
	}
	return false;
}

int main() {
	// 初始化测试数据
	std::vector<std::vector<int>> ids = {
		{1, 2, 3},
		{7, 8, 9},
		{4, 5, 6},
		{1, 2, 3, 9}
	};

	std::vector<std::pair<int, int>> result;

	// 优化的双层循环遍历
	for (size_t i = 0; i < ids.size(); ++i) {
		for (size_t j = i + 1; j < ids.size(); ++j) {
			if (compareVectors1(ids[i], ids[j])) {
				result.push_back(std::make_pair(i, j));
			}
		}
	}

	// 输出满足条件的下标对
	std::cout << "满足条件的下标对：" << std::endl;
	for (const auto& pair : result) {
		std::cout << "ids[" << pair.first << "] 和 ids[" << pair.second << "]" << std::endl;
	}

	return 0;
}
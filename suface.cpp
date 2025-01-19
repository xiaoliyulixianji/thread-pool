#include <iostream>
#include <vector>
#include <cmath>

// �Ƚ����������Ƿ���������
bool compareVectors1(const std::vector<int>& vec1, const std::vector<int>& vec2) {
	// ʹ��һ��־����ǰֹͣѭ��
	for (size_t i = 0; i < vec1.size(); ++i) {
		for (size_t j = 0; j < vec2.size(); ++j) {
			if (std::fabs(vec1[i] - vec2[j]) > 5) {
				return true;  // �������������ֱ�ӷ���
			}
		}
	}
	return false;
}

int main() {
	// ��ʼ����������
	std::vector<std::vector<int>> ids = {
		{1, 2, 3},
		{7, 8, 9},
		{4, 5, 6},
		{1, 2, 3, 9}
	};

	std::vector<std::pair<int, int>> result;

	// �Ż���˫��ѭ������
	for (size_t i = 0; i < ids.size(); ++i) {
		for (size_t j = i + 1; j < ids.size(); ++j) {
			if (compareVectors1(ids[i], ids[j])) {
				result.push_back(std::make_pair(i, j));
			}
		}
	}

	// ��������������±��
	std::cout << "�����������±�ԣ�" << std::endl;
	for (const auto& pair : result) {
		std::cout << "ids[" << pair.first << "] �� ids[" << pair.second << "]" << std::endl;
	}

	return 0;
}
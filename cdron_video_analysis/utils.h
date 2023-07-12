#pragma once

enum Det {
    tl_x = 0,
    tl_y = 1,
    br_x = 2,
    br_y = 3,
    score = 4,
    class_idx = 5
};

struct CDetection {
    cv::Rect bbox;
    float score;
    int class_idx;


	void show()
	{
		printf("[id = %u][score = %f][x = %u, y = %u, width = %u, height = %u]\n", class_idx, score, bbox.x, bbox.y, bbox.width, bbox.height);
		//std::cout << "[id = ]"
	}
	void show() const
	{
		printf("[id = %u][score = %f][x = %u, y = %u, width = %u, height = %u]\n", class_idx, score, bbox.x, bbox.y, bbox.width, bbox.height);
		//std::cout << "[id = ]"
	}
};
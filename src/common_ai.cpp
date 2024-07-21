#include "tiny_ecs.hpp"
#include "tiny_ecs_registry.hpp"
#include "common_ai.hpp"

void CommonAI::processNode(AITreeNode* node) {
	if (node->isEnd) {
		node->Action();
	}
	else {
		if (node->Decision()) {
			processNode(node->left);
		}
		else {
			processNode(node->right);
		}
	}
}
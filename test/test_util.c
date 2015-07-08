#include "../util.h"


int main()
{
	pr_err("I am wrong\n");
	pr_warning("I am warned\n");
	pr_info("I am informed\n");

	die_perror("I am dying\n");
	return 0;
}
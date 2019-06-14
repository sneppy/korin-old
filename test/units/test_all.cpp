/**
 * This code runs all unit tests defined
 * in the test folder
 */
#include "test_all.gen.h"

int main(int argc, char ** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
/*!
\author Borja Portugal Martin
GitHub: https://github.com/borjaportugal

This file is subject to the license terms in the LICENSE file
found in the top-level directory of this distribution.
*/


#include "testing/testing.h"

#include <iostream>

int main()
{
	testing::TestingConfig config;
	config.m_abort_on_failure = false;
	config.m_verbose = true;

	if (testing::run_all_tests(config) == false)
	{
		std::cin.get();
	}

	return 0;
}


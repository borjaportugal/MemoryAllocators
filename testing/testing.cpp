/*!
\author Borja Portugal Martin
GitHub: https://github.com/borjaportugal

This file is subject to the license terms in the LICENSE file
found in the top-level directory of this distribution.
*/

#include "testing.h"

#include <iostream>

#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>	// console colors

namespace testing
{
	void TestingStats::dump(std::ostream & os) const
	{
		os << "Tests run: " << m_run_tests << '\n'
			<< "  Succeded: " << m_succeded_tests << '\n'
			<< "  Failed: " << m_failed_tests;
	}

#pragma region // Test
	namespace impl
	{
		TestResult Test::run() const
		{
			TestResult result{};

			try
			{
				run_test();
			}
			catch (const TestFailedException & ex)
			{
				result.m_succeded = false;
				result.m_fail_reason = ex.get_failed_condition();
				result.m_line = ex.get_line();
			}
			catch (const std::exception & ex)
			{
				result.m_succeded = false;
				result.m_fail_reason = ex.what();
			}
			catch (...)
			{
				result.m_succeded = false;
				result.m_fail_reason = "Unknown reason.";
			}

			return result;
		}


		void Test::run_test() const
		{
			m_test_fn();
		}
	}
#pragma endregion
	
#pragma region // TestRunner

	TestRunner & TestRunner::get_instance()
	{
		static TestRunner instance;
		return instance;
	}

	bool TestRunner::run_all_tests(const TestingConfig & config)
	{
		set_config(config);		// config we will be using this run
		m_stats = TestingStats{};	// reset to track new stats

		auto & os = get_ostream();

		bool all_succeded = true;
		for (const auto & test : m_tests)
		{
			if (verbose())
				os << "Running: " << test.get_name() << std::endl;

			const auto test_result = test.run();
			all_succeded &= test_result.m_succeded;

			track_test_result(test, test_result);

			if (!test_result.m_succeded && abort_on_failure())
			{
				os << "Aborting test execution.\n";
				break;
			}
		}

		m_stats.m_run_tests = m_stats.m_succeded_tests + m_stats.m_failed_tests;

		os << '\n';
		os << "Final stats:\n";
		m_stats.dump(os);
		os << '\n';
		return all_succeded;
	}

	std::ostream & TestRunner::get_ostream() const
	{
		return *m_config.m_ostream;
	}
	void TestRunner::set_config(const TestingConfig & config)
	{
		m_config = config;

		// Default ostream is std::cout
		if (!m_config.m_ostream)
			m_config.m_ostream = &std::cout;
	}

	void TestRunner::track_test_result(const Test & test, const TestResult & test_result)
	{
		if (test_result.m_succeded)
			track_succeded_test(test, test_result);
		else
			track_failed_test(test, test_result);
	}
	void TestRunner::track_succeded_test(const Test & test,
										 const TestResult & test_result)
	{
		m_stats.m_succeded_tests++;
	}
	void TestRunner::track_failed_test(const Test & test,
									   const TestResult & test_result)
	{
		m_stats.m_failed_tests++;
		report_failure(test, test_result);
	}
	void TestRunner::report_failure(const Test & test,
									const TestResult & test_result) const
	{
		set_console_color(ConsoleColors::RED);
		get_ostream() << "Test " << test.get_name() << " failed: "
			<< test_result.m_fail_reason
			<< '\n';
		set_console_color(ConsoleColors::DEFAULT);
	}

	void TestRunner::set_console_color(ConsoleColors color) const
	{
		WORD color_attr;
		switch (color)
		{
		case ConsoleColors::DEFAULT:
		{
			color_attr = 0x07;
		} break;
		case ConsoleColors::RED:
		{
			color_attr = 0x04;
		} break;
		}

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color_attr);
	}

	void TestRunner::register_test(const Test & test)
	{
		m_tests.emplace_back(test);
	}

#pragma endregion
}

namespace testing
{
	namespace impl
	{
		void register_test(const Test & test)
		{
			TestRunner::get_instance().register_test(test);
		}
	}

	bool run_all_tests(const TestingConfig & config)
	{
		return TestRunner::get_instance().run_all_tests(config);
	}
}


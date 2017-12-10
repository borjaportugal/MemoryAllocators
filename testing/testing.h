/*!
\author Borja Portugal Martin
GitHub: https://github.com/borjaportugal

This file is subject to the license terms in the LICENSE file
found in the top-level directory of this distribution.
*/

#pragma once

#include <exception>
#include <string>
#include <vector>
#include <ostream>
#include <algorithm>	// std::all_of

namespace testing
{
	struct TestingConfig
	{
		std::ostream * m_ostream{ nullptr };
		bool m_verbose{ true };
		bool m_abort_on_failure{ false };
	};
		
	///	\brief Stores statistics of the last run tests.
	struct TestingStats
	{
		void dump(std::ostream & os) const;

		unsigned m_succeded_tests{ 0 };
		unsigned m_failed_tests{ 0 };
		unsigned m_run_tests{ 0 };
	};

	namespace impl
	{
		struct TestResult
		{
			operator bool() const { return m_succeded; }

			bool m_succeded{ true };
			unsigned m_line{ 0 };
			std::string m_fail_reason;
		};

		/// \brief	Thrown when a test fails, contains information about failure.
		class TestFailedException : public std::exception
		{
		public:
			TestFailedException(const char * failed_condition, unsigned line)
				: m_failed_cond{ failed_condition }
				, m_line{ line }
			{}

			const char * what() const override { return get_failed_condition(); }

			const char * get_failed_condition() const { return m_failed_cond; }
			unsigned get_line() const { return m_line; }

		private:
			const char * m_failed_cond{ nullptr };
			unsigned m_line{ 0 };

		};

		/// \brief	Stores and runs a unit test.
		class Test
		{
			using test_fn = void(*)();

		public:
			explicit Test(test_fn test, const char * name)
				: m_test_fn{ test }
				, m_test_name{ name }
			{}

			TestResult run() const;

			const char * get_name() const { return m_test_name; }

		private:
			void run_test() const;

			test_fn m_test_fn{ nullptr };
			const char * m_test_name{ "" };
		};

		/// \brief	Helper for not including TestRunner here.
		void register_test(const Test & test);
	}

	/// \brief	Base class for user defined categories.
	class TestCategory
	{
	public:
		virtual ~TestCategory() = default;
		
		void run()
		{
			tear_up();
			run_impl();
			tear_down();
		}

	private:
		/// \brief	Called before the test is executed.
		virtual void tear_up() {}
		/// \brief	Called when the test has finished (if succeded).
		virtual void tear_down() {}

		/// \brief	For internal usage
		virtual void run_impl() = 0;
	};

	/// \brief	Main class in charge of running all tests and getting statistics.
	class TestRunner
	{
	public:
		static TestRunner & get_instance();

		bool run_all_tests(const TestingConfig & config);

		/// \brief	Returns the statistics of the last call to run tests.
		TestingStats get_stats() const { return m_stats; }

	private:
		using Test = impl::Test;
		using TestResult = impl::TestResult;

		// test registration
		void register_test(const Test & test);
		friend void ::testing::impl::register_test(const Test &);

	private:
		void set_config(const TestingConfig & config);
		void track_test_result(const Test & test, const TestResult & test_result);
		void track_succeded_test(const Test & test, const TestResult & test_result);
		void track_failed_test(const Test & test, const TestResult & test_result);
		void report_failure(const Test & test, const TestResult & test_result) const;

		// configuration related
		bool abort_on_failure() const { return m_config.m_abort_on_failure; }
		bool verbose() const { return m_config.m_verbose; }
		std::ostream & get_ostream() const;

		enum class ConsoleColors
		{
			DEFAULT,
			RED,
		};
		void set_console_color(ConsoleColors color) const;

		TestingConfig m_config;

		/// \brief	Stats about the last execution of tests.
		TestingStats m_stats;

		/// \brief	Stores all the tests that have been compiled in the program.
		std::vector<Test> m_tests;
	};

}

inline std::ostream & operator<<(std::ostream & os, const ::testing::TestingStats & stats)
{
	stats.dump(os);
	return os;
}

namespace testing
{
	/// \brief	Runs all the tests that have been compiled.
	bool run_all_tests(const TestingConfig & config = TestingConfig{});
}

/// \brief	Declares a variable name that won't be duplicated.
///			The user needs to specify some value to have some context in case the compiler complains.
#define _TESTING_UNNAMED_VARIABLE(x)	_TESTING_UNNAMED_VARIABLE_INNER(x, __LINE__, __COUNTER__)
#define _TESTING_UNNAMED_VARIABLE_INNER(x, line, counter)	_TESTING_UNNAMED_VARIABLE_INNER2(x, line, counter)
#define _TESTING_UNNAMED_VARIABLE_INNER2(x, line, counter)	x ## _unnamed_var_ ## line ## _ ## counter

/// \brief	Register the test before main is called.
#define _TESTING_REGISTER_TEST(category, name, func)										\
namespace testing { namespace impl {														\
	static const bool _TESTING_UNNAMED_VARIABLE(test_register_ ## func) = []()	\
	{																						\
		::testing::impl::register_test(::testing::impl::Test{ func, #category "::" #name });\
		return true;																		\
	}();																					\
} }

#define _TESTING_DECLARE_TEST_INNER(category, name, func)	\
	void func();											\
	_TESTING_REGISTER_TEST(category, name, func);			\
	void func()

#define _TESTING_DECLARE_TEST(category, name)	\
	_TESTING_DECLARE_TEST_INNER(category, name, category ##_## name)

#define _TESTING_STRINGIFY(x)		_TESTING_STRINGIFY_INNER(x)
#define _TESTING_STRINGIFY_INNER(x)	#x


// =============================================================
// Macros that the user can use are the ones from this point on.
// =============================================================


///	\brief	If the condition is not satisfied the test fails.
#define TEST_ASSERT(cond) do { if (!(cond))	throw std::exception{ "Condition ( " #cond " ) at line " _TESTING_STRINGIFY(__LINE__) " not satisfied." , __LINE__ }; } while (0)

///	\brief	Test fails inmediately.
#define TEST_FAILED() TEST_ASSERT(false)

#define TEST_ASSERT_ALL(b, e, comp) TEST_ASSERT(std::all_of(b, e, [](auto v){ return v comp; }))

///	\brief	Placeholder macro to mark paths that the execution should follow in order to make the test pass,
///	lets the test continue executing.
#define TEST_SUCCEDED()		

/// \brief	Declares a test
#define TEST_F(test_name)	_TESTING_DECLARE_TEST(global, test_name)

/// \brief	Declares a test within a category.
///			Categories need to inherit from ::testing::TestCategory and member variables created in them are 
///			accesible from the tests.
///			Each test has its own instance of the category and therefore member variables.
///			Variables are constructed just before running the test and destroyed after the test has finished (even if the test failed)
#define TEST(test_category, test_name)													\
	/* Wrap the test into a class that contains the needed data	*/						\
	class TestRunner_ ## test_name : public test_category								\
	{																					\
		void run_impl() override;														\
	};																					\
	/* What will be called by the TestRunner */											\
	_TESTING_DECLARE_TEST(test_category, test_name) { TestRunner_ ## test_name test; test.run(); }	\
	void TestRunner_ ## test_name::run_impl()
	




/** \file
 * \brief Example program that produces a nested error stack.
 *
 * The Error_Stack program has a function that throws when the stack is 
 * four layers deep. This exception then propagates up the call stack, 
 * and is rethrown at each level. As is propagates, more descriptive 
 * information is added. Finally, in main(), the exception is printed.
 *
 * This program's output should look similar to this:
 *
~~~~~
Exception: level: 0
        Reason: This is the third function in the throw hierarchy!
        Top-Level-Comment:      This is a test.
        source_filename:        /home/ryan/Documents/BetterThrow/BetterThrow/Examples/Error_Stack/Error_Stack.cpp
        source_function:        void badfunc_a(const string&)
        source_line:    39

Exception: level: 1
        Reason: This is the second function in the throw hierarchy!
        source_filename:        /home/ryan/Documents/BetterThrow/BetterThrow/Examples/Error_Stack/Error_Stack.cpp
        source_function:        void badfunc_b()
        source_line:    29

Exception: level: 2
        Odd number:     34989
        Reason: This should throw!
        source_filename:        /home/ryan/Documents/BetterThrow/BetterThrow/Examples/Error_Stack/Error_Stack.cpp
        source_function:        void badfunc_c()
        source_line:    18
~~~~~

 **/

#include <iostream>
#include <string>
#include "../../include/BetterThrow/Error.hpp"

void badfunc_c()
{
	BT_throw
		.add("Reason", "This should throw!")
		.add<int>("Odd number", 34989)
		();
}

void badfunc_b()
{
	try {
		badfunc_c();
	}
	catch (...) {
		std::throw_with_nested(BT_throw.add("Reason", "This is the second function in the throw hierarchy!"));
	}
}

void badfunc_a(const std::string &example_text)
{
	try {
		badfunc_b();
	}
	catch (...) {
		std::throw_with_nested(
				BT_throw
					.add("Reason", "This is the third function in the throw hierarchy!")
					.add("Top-Level-Comment", example_text));
	}
}


int main(int, char**)
{
	try {
		badfunc_a("This is a test.");
	}
	catch (std::exception & f) {
		BT::print_exception(f);
	}
	return 0;
}

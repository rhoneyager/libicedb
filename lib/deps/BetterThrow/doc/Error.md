The Error Classes (Error.hpp) {#Error}
----------------------

An example is at Examples/Error_Stack/Error_Stack.cpp.

The main error class is BT::xError. 

# Throwing an error

Use the BT_throw macro. This expands to:
```
throw BT::xError()
	.add("source_filename", "your-source-file")
	.add("source_function", "the signature of the lexically-enclosing function")
	.add("source_line", line#)
```

Additional parameters may be passed using the 
```
add<T>(const std::string& key, T value)
```
function. This function internally uses std::ostringstream to 
reformat all values as strings.

# Catching and rethrowing errors

Use std::throw_with_nested. This exception stack is easily unwindable.

```
void example_catching_and_rethrowing_function(int important_number)
{
	try {
		function_that_throws();
	}
	catch (...) {
		std::throw_with_nested(
			BT_throw
				.add("Reason", "We caught an exception and add information here.")
				.add("important_number", important_number)
			);
	}
}
```

# Unwinding the exception stack and printing error messages

A convenience function, 
```
BT::print_exception(const std::exception &, std::ostream &output_stream = std::cerr, int level=0)
```
exists to unwind the stack and print error messages.

```
int main(int, char**)
{
	try {
		BT_throw.add("Reason", "This is a test.");
	}
	catch (std::exception & f) {
		BT::print_exception(f);
	}
	return 0;
}
```

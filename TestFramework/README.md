# Test Framework Prototype
Test Framework Project. This source codes consists of the following components:

- Test Framework
    - Graphical User Inteface
    - Test Engine
- Test Dynamic Link Library (DLL)
    - Sample test libraries used by Test Framework

# Requirements
- Visual Studio 2019 
- Add "Desktop developlment with C++".
- Add "C++ MFC for latest v142 build tools (x86 & x64)".

# Build
- Open TestFramework.sln.
- Build the solution.

# Execution
An existing executable can be found in the Release directory. Alternatively, you may build it using the instructions in the previous section and run the resulting executable in the build directory.
The Test Framework runs as a GUI application which allows the user to browse for test binaries (DLLs, JARs, etc) and list the tests available. Several sample DLLs and JARs are built and copied to the same directory as the TestFramework.exe directory.

# Usage
- "Browse for Tests" - opens a file browser dialog to allow selection of one or more test files
- "Reset Test List" - removes all tests currently loaded into the test engine queue
- "Run Tests" - runs all tests currently loaded into the test engine queue
- "Number of Threads" - buttons controlling the number of threads the test engine should use (can be set to 5, 10, or 15. default: 10)

- "Reset Log" - erases all current data from the log dialog (note: this does NOT erase any log data saved to file)
- "Export Log" - opens a file browser to allow user to specify log file to save all log data to (note: if the file that is chosen already exists, the application will append the current log data to it)

* Please note that JAR files are actually DLL files renamed to JAR to demonstrate similar functionality for testing JAR files.

Test Framework

![TestFramework](https://github.com/KodeFu/cse682/blob/master/TestFramework/TestFramework/framework.png)

Test Engine Console

![TestEngine](https://github.com/KodeFu/cse682/blob/master/TestFramework/TestFramework/testengine.png)

# Software Credits (Reuse)
Test Harness Project
- Test DLL project and Test Engine loop written by Mudit Vats.


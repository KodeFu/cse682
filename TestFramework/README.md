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
Run TestFramework.exe from the Visual 2019 environment; i.e. "Start with Debugging". The executable can be run by navigating to the build directory as well.

The Test Framework runs as a GUI application which allows the user to browse for test binaries (DLLs, JARs, etc) and list the tests available. Several sample DLLs and JARs are built and copied to the same directory as the TestFramework.exe directory.

* Please note that JAR files are actually DLL files renamed to JAR to demonstrate similar functionality for testing JAR files.

Test Framework

![TestFramework](https://github.com/KodeFu/cse682/blob/master/TestFramework/TestFramework/framework.png)

Test Engine Console

![TestEngine](https://github.com/KodeFu/cse682/blob/master/TestFramework/TestFramework/testengine.png)

# Software Credits (Reuse)
Test Harness Project
- Test DLL project and Test Engine loop written by Mudit Vats.


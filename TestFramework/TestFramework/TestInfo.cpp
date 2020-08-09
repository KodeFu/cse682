#include "pch.h"
#include "TestInfo.h"

TestInfo::TestInfo()
	: _filePath(""), _fileName(""), _testName("")
{

}

TestInfo::~TestInfo()
{

}

void TestInfo::setFilePath(CString filePath)
{
	_filePath = filePath;
}

void TestInfo::setFileName(CString fileName)
{
	_fileName = fileName;
}

void TestInfo::setTestName(CString testName)
{
	_testName = testName;
}

CString TestInfo::getFilePath()
{
	return _filePath;
}

CString TestInfo::getFileName()
{
	return _fileName;
}

CString TestInfo::getTestName()
{
	return _testName;
}
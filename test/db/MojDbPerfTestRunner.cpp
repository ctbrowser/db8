/* @@@LICENSE
*
*      Copyright (c) 2009-2014 LG Electronics, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* LICENSE@@@ */


#include "MojDbPerfTestRunner.h"
#include "MojDbPerfCreateTest.h"
#include "MojDbPerfReadTest.h"
#include "MojDbPerfUpdateTest.h"
#include "MojDbPerfDeleteTest.h"
#include "MojDbPerfIndexTest.h"


MojString getTestDir()
{
	MojString dir;
	MojErr err = dir.format("/tmp/mojodb-test-dir-%d", getpid());
	assert( err == MojErrNone );
	return dir;
}
const MojString mojDbTestDirString = getTestDir();
const MojChar* const MojDbTestDir = mojDbTestDirString.data();
MojUInt64 allTestsTime = 0;

int main(int argc, char** argv)
{
	MojDbPerfTestRunner runner;
	return runner.main(argc, argv);
}

void MojDbPerfTestRunner::runTests()
{
#if MOJ_USE_LDB
    // TODO: GF-4906
    test(MojDbPerfIndexTest());
#endif
	test(MojDbPerfCreateTest());
	test(MojDbPerfReadTest());
	test(MojDbPerfUpdateTest());
	test(MojDbPerfDeleteTest());
	MojDouble res = allTestsTime / 1000000000.0f;
	(void) MojPrintF("\n\n ALL TESTS FINISHED. TIME ELAPSED: %10.3f seconds.\n\n", res);
}

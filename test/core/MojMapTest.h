/* @@@LICENSE
*
*      Copyright (c) 2009-2013 LG Electronics, Inc.
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

/**
****************************************************************************************************
* Filename              : MojMapTest.h
* Description           : Header file for MojMap test.
****************************************************************************************************
**/

#ifndef MOJMAPTEST_H_
#define MOJMAPTEST_H_

#include "MojCoreTestRunner.h"
#include "core/MojMap.h"

class MojMapTest : public MojTestCase
{
public:
	MojMapTest();

	virtual MojErr run();

private:
	MojSize count(MojMap<int, MojString>& set);
};

#endif /* MOJMAPTEST_H_ */

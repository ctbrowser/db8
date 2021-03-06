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
* Filename              : MojSchemaTest.h

* Description           : Header file for MojSchema test.
****************************************************************************************************
**/
#ifndef MOJSCHEMATEST_H_
#define MOJSCHEMATEST_H_

#include "MojCoreTestRunner.h"

class MojSchemaTest : public MojTestCase
{
public:
	MojSchemaTest();

	virtual MojErr run();

private:
	MojErr typeTest();
	MojErr disallowTest();
	MojErr propertiesTest();
	MojErr itemsTest();
	MojErr requiresTest();
	MojErr minmaxTest();
	MojErr arrayTest();
	MojErr stringTest();
	MojErr enumTest();
	MojErr divisibleTest();

	MojErr checkValid(const MojChar* schemaJson, const MojChar* instanceJson, bool expected);
};

#endif /* MOJSCHEMATEST_H_ */

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


#include "core/MojLogDb8.h"
#include "db-luna/MojDbBerkeleyFactory.h"
#include "db-luna/MojDbBerkeleyEngine.h"

namespace { MojDbStorageEngine::Registrator<MojDbBerkeleyFactory> factoryBerkeleyDB; }

MojErr MojDbBerkeleyFactory::create(MojRefCountedPtr<MojDbStorageEngine>& engineOut) const
{
    LOG_TRACE("Entering function %s", __FUNCTION__);

	engineOut.reset(new MojDbBerkeleyEngine());
	MojAllocCheck(engineOut.get());

	return MojErrNone;
}
MojErr MojDbBerkeleyFactory::createEnv(MojRefCountedPtr<MojDbEnv>& envOut) const
{
    LOG_TRACE("Entering function %s", __FUNCTION__);

	envOut.reset(new MojDbBerkeleyEnv());
	MojAllocCheck(envOut.get());

	return MojErrNone;
}

const MojChar* MojDbBerkeleyFactory::name() const {return _T("berkeleydb");};

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


#include "db/MojDbQueryFilter.h"
#include "db/MojDbTextUtils.h"
#include "unicode/ustring.h"
#include "core/MojLogDb8.h"

MojDbQueryFilter::MojDbQueryFilter()
{
}

MojDbQueryFilter::~MojDbQueryFilter()
{
}

MojErr MojDbQueryFilter::init(const MojDbQuery& query)
{
    LOG_TRACE("Entering function %s", __FUNCTION__);

	m_clauses = query.filter();

	return MojErrNone;
}

/***********************************************************************
 * test
 *
 * Test whether values of filter conditions exist in range or not
 * through the following sequence;
 *   1. If name of filter condition is object format as "file.name",
 *   Split with '.' delimiter and contain the result into string vector.
 *   2. Find values of delivered object through findValue function
 *   and contain the results into object array.
 *   3. Check whether retrieved value from delivered object exists in range.
 * overflows and to report any truncations.
 ***********************************************************************/
MojErr MojDbQueryFilter::test(const MojObject& obj, bool& isFound) const
{
    LOG_TRACE("Entering function %s", __FUNCTION__);

    for (MojDbQuery::WhereMap::ConstIterator filterIter = m_clauses.begin(); filterIter != m_clauses.end(); ++filterIter) {
        // if name of filter condition is object format as "file.name", split with '.' delimiter.
        MojString keyStr;
        keyStr.assign(filterIter.key());
        MojVector<MojString> keyVec;
        MojErr err = keyStr.split(_T('.'), keyVec);
        MojErrCheck(err);

        // find values by using key name and contain results into object array.
        MojObject objVals;
        err = findValue( obj, keyVec.begin(), keyVec.end(), objVals, isFound);
        MojErrCheck(err);

        if (!isFound) // object not found
            return MojErrNone;

        // check whether the value exists in range.
        bool testResult = false;
        for (MojObject::ConstArrayIterator valIter = objVals.arrayBegin(); valIter != objVals.arrayEnd(); ++valIter) {
            err = testLower(*filterIter, *valIter, testResult);
            MojErrCheck(err);
            if (!testResult)
                continue;

            err = testUpper(*filterIter, *valIter, testResult);
            MojErrCheck(err);

            if (testResult) {
                isFound = false;
                break;
            }
        }

        if (!testResult) {
            isFound = false;
            return MojErrNone;
        }
    }

    isFound = true;

    return MojErrNone;
}

/***********************************************************************
 * findValue
 *
 * Find values recursively from delivered data object
 * by using name vector from filter condition.
 * and contain the result into object array.
 ***********************************************************************/
MojErr MojDbQueryFilter::findValue(const MojObject obj, const MojString* begin, const MojString* end, MojObject& valOut, bool& ret) const
{
    LOG_TRACE("Entering function %s", __FUNCTION__);

    MojObject childObj = obj;
    for (const MojString* key = begin; key != end; ++key) {
        if(childObj.type() == MojObject::TypeArray) {
            // if array, find values recursively
            for (MojObject::ConstArrayIterator childObjIter = childObj.arrayBegin(); childObjIter != childObj.arrayEnd(); ++childObjIter) {
                findValue(*childObjIter, key, end, valOut, ret);
            }
            ret = !valOut.empty();
            return MojErrNone;
        } else {
            if(!childObj.get(key->data(), childObj)) {
                ret = false;
                return MojErrNone;
            }
        }
    }

    // if found, push result value into object array.
    if(childObj.type() == MojObject::TypeArray) {
        bool foundOut;
        MojErr err;
        MojObject::ArrayIterator iter;
        err = childObj.arrayBegin(iter);
        MojErrCheck(err);
        for (; iter != childObj.arrayEnd(); ++iter) {
            // if result set is array, we should remove "_id" for comparison.
            err = iter->del(_T("_id"), foundOut);
            MojErrCheck(err);
            valOut.push(*iter);
        }
    } else {
        valOut.push(childObj);
    }

    ret = true;
    return MojErrNone;
}

MojErr MojDbQueryFilter::testLower(const MojDbQuery::WhereClause& clause, const MojObject& val, bool & ret)
{
    LOG_TRACE("Entering function %s", __FUNCTION__);

	const MojObject& lowerVal = clause.lowerVal();
	switch (clause.lowerOp()) {
    case MojDbQuery::OpNone:
        ret = true;
        return MojErrNone;
	case MojDbQuery::OpEq:
        // if lower value type is array, lower operation can use equal operator only.
        if (lowerVal.type() == MojObject::TypeArray) {
            MojObject::ConstArrayIterator end = lowerVal.arrayEnd();
            for (MojObject::ConstArrayIterator i = lowerVal.arrayBegin(); i != end; ++i) {
                if (val == *i) {
                    ret = true;
                    return MojErrNone;
                }
            }
            ret = false;
            return MojErrNone;
        } else {
            ret = (val == lowerVal);
            return MojErrNone;
        }

	case MojDbQuery::OpNotEq:
        ret = (val != lowerVal);
        return MojErrNone;
	case MojDbQuery::OpGreaterThan:
        ret = (val > lowerVal);
        return MojErrNone;
	case MojDbQuery::OpGreaterThanEq:
        ret = (val >= lowerVal);
        return MojErrNone;
    case MojDbQuery::OpSubString: {
        FindSubStringResult res;
        MojErr err = findSubString(val, lowerVal, res);
        MojErrCheck(err);
        ret = res.isFound();
        return MojErrNone;
    }
	case MojDbQuery::OpPrefix: {
        FindSubStringResult res;
        MojErr err = findSubString(val, lowerVal, res);
        MojErrCheck(err);

        ret = ((res.isFound() && res.pos() == 0) || (res.srcIsEmpty() && res.subStringIsEmpty()));
        return MojErrNone;
	}

        default:
		MojAssertNotReached();
        ret = false;
        return MojErrNone;
	}
}

MojErr MojDbQueryFilter::testUpper(const MojDbQuery::WhereClause& clause, const MojObject& val, bool & ret)
{
    LOG_TRACE("Entering function %s", __FUNCTION__);

	const MojObject& upperVal = clause.upperVal();
	switch (clause.upperOp()) {
	case MojDbQuery::OpNone:
        ret = true;
        return MojErrNone;
	case MojDbQuery::OpLessThan:
        ret = (val < upperVal);
        return MojErrNone;
	case MojDbQuery::OpLessThanEq:
        ret = (val <= upperVal);
        return MojErrNone;
	default:
		MojAssertNotReached();
        ret = false;
        return MojErrNone;
	}
}

/***********************************************************************
 * findSubString
 *
 * Find whether "subString" exists in "src" or not recursively
 * through the following sequence.
 *   1. Check "subString" type is array or not.
 *   2. If type of "src" and "subString" is string,
 *      convert their to upper case for case insensitivity
 *   3. If "subString" is found in "src", return true.
 ***********************************************************************/
MojErr MojDbQueryFilter::findSubString(const MojObject& src, const MojObject& subString, FindSubStringResult& ret)
{
    LOG_TRACE("Entering function %s", __FUNCTION__);

    if (subString.type() == MojObject::TypeArray) {
        // If "subString" type is array, take out each items for recursive process
        MojObject::ConstArrayIterator end = subString.arrayEnd();
        for (MojObject::ConstArrayIterator i = subString.arrayBegin(); i != end; ++i) {
            MojErr err = findSubString(src, *i, ret);
            MojErrCheck( err );
            if(ret.isFound()) {
                return MojErrNone;
            }
        }
    } else {
        // Type of "src" and "subString" should be string.
        if(src.type() != MojObject::TypeString || subString.type() != MojObject::TypeString) {
            ret = FindSubStringResult();
            return MojErrNone;
        }
        MojString srcStr;
        MojErr err;
        err = src.stringValue(srcStr);
        MojErrCheck(err);
        MojString subStringStr;
        err = subString.stringValue(subStringStr);
        MojErrCheck(err);

        // Filtering out in case of "" string. Because ICU does not support.
        if (srcStr.empty() && subStringStr.empty()) {
            ret = FindSubStringResult(true, true);
            return MojErrNone;
        }
        else if (srcStr.empty() || subStringStr.empty()) {
            ret = FindSubStringResult(srcStr.empty(), subStringStr.empty());
            return MojErrNone;
        }
        // convert "src" to upper case.
        MojDbTextUtils::UnicodeVec srcOut;
        // locale info did not set for locale insesitivity.
        err = MojDbTextUtils::strToUpper(srcStr, _T(""), srcOut);
        MojErrCheck(err);

        // convert "subString" to upper case.
        MojDbTextUtils::UnicodeVec subStringOut;
        err = MojDbTextUtils::strToUpper(subStringStr, _T(""), subStringOut);
        MojErrCheck(err);

        // If "subString" is found in "src", return true.
        UChar * b = u_strstr(srcOut.begin(), subStringOut.begin());
        if( b ) {
            ret = FindSubStringResult( static_cast<int>(  b - srcOut.begin() ) );
            return MojErrNone;
        }
    }
    ret = FindSubStringResult();
    return MojErrNone;
}

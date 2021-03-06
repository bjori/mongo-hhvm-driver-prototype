/**
 *  Copyright 2014-2015 MongoDB, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "hphp/runtime/ext/extension.h"
#include "hphp/runtime/vm/native-data.h"
#if 0
#include "hphp/runtime/base/array-init.h"
#endif

#include "src/MongoDB/Driver/BulkWrite.h"
#include "src/MongoDB/Driver/Cursor.h"
#include "src/MongoDB/Driver/CursorId.h"
#include "src/MongoDB/Driver/Manager.h"
#include "src/MongoDB/Driver/Query.h"
#include "src/MongoDB/Driver/ReadPreference.h"
#include "src/MongoDB/Driver/Server.h"
#include "src/MongoDB/Driver/WriteConcern.h"
#include "src/MongoDB/Driver/WriteResult.h"

#include "src/MongoDB/BSON/functions.h"
#include "src/MongoDB/BSON/Binary.h"
#include "src/MongoDB/BSON/Javascript.h"
#include "src/MongoDB/BSON/ObjectID.h"
#include "src/MongoDB/BSON/UTCDateTime.h"

#include "mongodb.h"
#include "bson.h"
#include "utils.h"

extern "C" {
#include "libbson/src/bson/bson.h"
#include "libmongoc/src/mongoc/mongoc.h"
}

namespace HPHP {

void hippo_log_handler(mongoc_log_level_t log_level, const char *log_domain, const char *message, void *user_data)
{
	std::cerr << "log: " << log_domain << "; msg: " << message << "\n";
}

static class MongoDBExtension : public Extension {
	public:
		MongoDBExtension() : Extension("mongodb", "0.6.0") {}

		virtual void moduleInit() {
			/* MongoDB\BSON functions */
			HHVM_FALIAS(MongoDB\\BSON\\fromPHP, MongoDBBsonFromPHP);
			HHVM_FALIAS(MongoDB\\BSON\\fromJson, MongoDBBsonFromJson);
			HHVM_FALIAS(MongoDB\\BSON\\toPHP, MongoDBBsonToPHP);
			HHVM_FALIAS(MongoDB\\BSON\\toJson, MongoDBBsonToJson);

			/* MongoDB\BSON\Binary */
			Native::registerClassConstant<KindOfInt64>(s_MongoBsonBinary_className.get(), makeStaticString("TYPE_GENERIC"), (int64_t) BSON_SUBTYPE_BINARY);
			Native::registerClassConstant<KindOfInt64>(s_MongoBsonBinary_className.get(), makeStaticString("TYPE_FUNCTION"), (int64_t) BSON_SUBTYPE_FUNCTION);
			Native::registerClassConstant<KindOfInt64>(s_MongoBsonBinary_className.get(), makeStaticString("TYPE_OLD_BINARY"), (int64_t) BSON_SUBTYPE_BINARY_DEPRECATED);
			Native::registerClassConstant<KindOfInt64>(s_MongoBsonBinary_className.get(), makeStaticString("TYPE_OLD_UUID"), (int64_t) BSON_SUBTYPE_UUID_DEPRECATED);
			Native::registerClassConstant<KindOfInt64>(s_MongoBsonBinary_className.get(), makeStaticString("TYPE_UUID"), (int64_t) BSON_SUBTYPE_UUID);
			Native::registerClassConstant<KindOfInt64>(s_MongoBsonBinary_className.get(), makeStaticString("TYPE_MD5"), (int64_t) BSON_SUBTYPE_MD5);
			Native::registerClassConstant<KindOfInt64>(s_MongoBsonBinary_className.get(), makeStaticString("TYPE_USER_DEFINED"), (int64_t) BSON_SUBTYPE_USER);

			HHVM_MALIAS(MongoDB\\BSON\\Binary, __debugInfo, MongoDBBsonBinary, __debugInfo);

			/* MongoDB\BSON\ObjectID */
			HHVM_MALIAS(MongoDB\\BSON\\ObjectID, __construct, MongoDBBsonObjectID, __construct);
			HHVM_MALIAS(MongoDB\\BSON\\ObjectID, __debugInfo, MongoDBBsonObjectID, __debugInfo);
			HHVM_MALIAS(MongoDB\\BSON\\ObjectID, __toString, MongoDBBsonObjectID, __toString);

			Native::registerNativeDataInfo<MongoDBBsonObjectIDData>(MongoDBBsonObjectIDData::s_className.get());

			/* MongoDB\BSON\UTCDateTime */
			HHVM_MALIAS(MongoDB\\BSON\\UTCDateTime, __construct, MongoDBBsonUTCDateTime, __construct);
			HHVM_MALIAS(MongoDB\\BSON\\UTCDateTime, toDateTime, MongoDBBsonUTCDateTime, toDateTime);

			/* MongoDB\Driver\Manager */
			HHVM_MALIAS(MongoDB\\Driver\\Manager, __construct, MongoDBDriverManager, __construct);
			HHVM_MALIAS(MongoDB\\Driver\\Manager, __debugInfo, MongoDBDriverManager, __debugInfo);
			HHVM_MALIAS(MongoDB\\Driver\\Manager, __wakeup, MongoDBDriverManager, __wakeup);
			HHVM_MALIAS(MongoDB\\Driver\\Manager, executeBulkWrite, MongoDBDriverManager, executeBulkWrite);
			HHVM_MALIAS(MongoDB\\Driver\\Manager, executeCommand, MongoDBDriverManager, executeCommand);
			HHVM_MALIAS(MongoDB\\Driver\\Manager, executeDelete, MongoDBDriverManager, executeDelete);
			HHVM_MALIAS(MongoDB\\Driver\\Manager, executeInsert, MongoDBDriverManager, executeInsert);
			HHVM_MALIAS(MongoDB\\Driver\\Manager, executeQuery, MongoDBDriverManager, executeQuery);
			HHVM_MALIAS(MongoDB\\Driver\\Manager, executeUpdate, MongoDBDriverManager, executeUpdate);
			HHVM_MALIAS(MongoDB\\Driver\\Manager, getReadPreference, MongoDBDriverManager, getReadPreference);
			HHVM_MALIAS(MongoDB\\Driver\\Manager, getWriteConcern, MongoDBDriverManager, getWriteConcern);
			HHVM_MALIAS(MongoDB\\Driver\\Manager, selectServer, MongoDBDriverManager, selectServer);

			Native::registerNativeDataInfo<MongoDBDriverManagerData>(MongoDBDriverManagerData::s_className.get());

			/* MongoDB\Driver\BulkWrite */
			HHVM_MALIAS(MongoDB\\Driver\\BulkWrite, __construct, MongoDBDriverBulkWrite, __construct);
			HHVM_MALIAS(MongoDB\\Driver\\BulkWrite, insert, MongoDBDriverBulkWrite, insert);
			HHVM_MALIAS(MongoDB\\Driver\\BulkWrite, update, MongoDBDriverBulkWrite, update);
			HHVM_MALIAS(MongoDB\\Driver\\BulkWrite, delete, MongoDBDriverBulkWrite, delete);
			HHVM_MALIAS(MongoDB\\Driver\\BulkWrite, count, MongoDBDriverBulkWrite, count);
			HHVM_MALIAS(MongoDB\\Driver\\BulkWrite, __debugInfo, MongoDBDriverBulkWrite, __debugInfo);

			Native::registerNativeDataInfo<MongoDBDriverBulkWriteData>(MongoDBDriverBulkWriteData::s_className.get());

			/* MongoDb\Driver\CursorId */
			HHVM_MALIAS(MongoDB\\Driver\\CursorId, __construct, MongoDBDriverCursorId, __construct);
			HHVM_MALIAS(MongoDB\\Driver\\CursorId, __toString,  MongoDBDriverCursorId, __toString);

			Native::registerNativeDataInfo<MongoDBDriverCursorIdData>(MongoDBDriverCursorIdData::s_className.get());

			/* MongoDb\Driver\Cursor */
			HHVM_MALIAS(MongoDB\\Driver\\Cursor, __debugInfo, MongoDBDriverCursor, __debugInfo);
			HHVM_MALIAS(MongoDB\\Driver\\Cursor, getId, MongoDBDriverCursor, getId);
			HHVM_MALIAS(MongoDB\\Driver\\Cursor, getServer, MongoDBDriverCursor, getServer);
			HHVM_MALIAS(MongoDB\\Driver\\Cursor, setTypeMap, MongoDBDriverCursor, setTypeMap);
			HHVM_MALIAS(MongoDB\\Driver\\Cursor, current, MongoDBDriverCursor, current);
			HHVM_MALIAS(MongoDB\\Driver\\Cursor, key, MongoDBDriverCursor, key);
			HHVM_MALIAS(MongoDB\\Driver\\Cursor, next, MongoDBDriverCursor, next);
			HHVM_MALIAS(MongoDB\\Driver\\Cursor, rewind, MongoDBDriverCursor, rewind);
			HHVM_MALIAS(MongoDB\\Driver\\Cursor, valid, MongoDBDriverCursor, valid);
			HHVM_MALIAS(MongoDB\\Driver\\Cursor, toArray, MongoDBDriverCursor, toArray);

			Native::registerNativeDataInfo<MongoDBDriverCursorData>(MongoDBDriverCursorData::s_className.get());

			/* MongoDb\Driver\Query */
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverQuery_className.get(), makeStaticString("FLAG_NONE"), (int64_t) MONGOC_QUERY_NONE);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverQuery_className.get(), makeStaticString("FLAG_TAILABLE_CURSOR"), (int64_t) MONGOC_QUERY_TAILABLE_CURSOR);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverQuery_className.get(), makeStaticString("FLAG_SLAVE_OK"), (int64_t) MONGOC_QUERY_SLAVE_OK);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverQuery_className.get(), makeStaticString("FLAG_OPLOG_REPLAY"), (int64_t) MONGOC_QUERY_OPLOG_REPLAY);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverQuery_className.get(), makeStaticString("FLAG_NO_CURSOR_TIMEOUT"), (int64_t) MONGOC_QUERY_NO_CURSOR_TIMEOUT);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverQuery_className.get(), makeStaticString("FLAG_AWAIT_DATA"), (int64_t) MONGOC_QUERY_AWAIT_DATA);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverQuery_className.get(), makeStaticString("FLAG_EXHAUST"), (int64_t) MONGOC_QUERY_EXHAUST);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverQuery_className.get(), makeStaticString("FLAG_PARTIAL"), (int64_t) MONGOC_QUERY_PARTIAL);

			/* MongoDb\Driver\ReadPreference */
			HHVM_MALIAS(MongoDB\\Driver\\ReadPreference, _setReadPreference, MongoDBDriverReadPreference, _setReadPreference);
			HHVM_MALIAS(MongoDB\\Driver\\ReadPreference, _setReadPreferenceTags, MongoDBDriverReadPreference, _setReadPreferenceTags);
			HHVM_MALIAS(MongoDB\\Driver\\ReadPreference, __debugInfo, MongoDBDriverReadPreference, __debugInfo);

			Native::registerNativeDataInfo<MongoDBDriverReadPreferenceData>(MongoDBDriverReadPreferenceData::s_className.get());

			Native::registerClassConstant<KindOfInt64>(s_MongoDriverReadPreference_className.get(), makeStaticString("RP_PRIMARY"), (int64_t) MONGOC_READ_PRIMARY);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverReadPreference_className.get(), makeStaticString("RP_PRIMARY_PREFERRED"), (int64_t) MONGOC_READ_PRIMARY_PREFERRED);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverReadPreference_className.get(), makeStaticString("RP_SECONDARY"), (int64_t) MONGOC_READ_SECONDARY);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverReadPreference_className.get(), makeStaticString("RP_SECONDARY_PREFERRED"), (int64_t) MONGOC_READ_SECONDARY_PREFERRED);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverReadPreference_className.get(), makeStaticString("RP_NEAREST"), (int64_t) MONGOC_READ_NEAREST);

			/* MongoDb\Driver\Server */
			HHVM_MALIAS(MongoDB\\Driver\\Server, __debugInfo, MongoDBDriverServer, __debugInfo);
			HHVM_MALIAS(MongoDB\\Driver\\Server, executeBulkWrite, MongoDBDriverServer, executeBulkWrite);
			HHVM_MALIAS(MongoDB\\Driver\\Server, executeCommand, MongoDBDriverServer, executeCommand);
			HHVM_MALIAS(MongoDB\\Driver\\Server, executeQuery, MongoDBDriverServer, executeQuery);
			HHVM_MALIAS(MongoDB\\Driver\\Server, getHost, MongoDBDriverServer, getHost);
			HHVM_MALIAS(MongoDB\\Driver\\Server, getInfo, MongoDBDriverServer, getInfo);
			HHVM_MALIAS(MongoDB\\Driver\\Server, getLatency, MongoDBDriverServer, getLatency);
			HHVM_MALIAS(MongoDB\\Driver\\Server, getPort, MongoDBDriverServer, getPort);
			HHVM_MALIAS(MongoDB\\Driver\\Server, getTags, MongoDBDriverServer, getTags);
			HHVM_MALIAS(MongoDB\\Driver\\Server, getType, MongoDBDriverServer, getType);
			HHVM_MALIAS(MongoDB\\Driver\\Server, isPrimary, MongoDBDriverServer, isPrimary);
			HHVM_MALIAS(MongoDB\\Driver\\Server, isSecondary, MongoDBDriverServer, isSecondary);
			HHVM_MALIAS(MongoDB\\Driver\\Server, isArbiter, MongoDBDriverServer, isArbiter);
			HHVM_MALIAS(MongoDB\\Driver\\Server, isHidden, MongoDBDriverServer, isHidden);
			HHVM_MALIAS(MongoDB\\Driver\\Server, isPassive, MongoDBDriverServer, isPassive);

			Native::registerNativeDataInfo<MongoDBDriverServerData>(MongoDBDriverServerData::s_className.get());

			Native::registerClassConstant<KindOfInt64>(s_MongoDriverServer_className.get(), makeStaticString("TYPE_UNKNOWN"), (int64_t) MONGOC_SERVER_UNKNOWN);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverServer_className.get(), makeStaticString("TYPE_STANDALONE"), (int64_t) MONGOC_SERVER_STANDALONE);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverServer_className.get(), makeStaticString("TYPE_MONGOS"), (int64_t) MONGOC_SERVER_MONGOS);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverServer_className.get(), makeStaticString("TYPE_POSSIBLE_PRIMARY"), (int64_t) MONGOC_SERVER_POSSIBLE_PRIMARY);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverServer_className.get(), makeStaticString("TYPE_RS_PRIMARY"), (int64_t) MONGOC_SERVER_RS_PRIMARY);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverServer_className.get(), makeStaticString("TYPE_RS_SECONDARY"), (int64_t) MONGOC_SERVER_RS_SECONDARY);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverServer_className.get(), makeStaticString("TYPE_RS_ARBITER"), (int64_t) MONGOC_SERVER_RS_ARBITER);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverServer_className.get(), makeStaticString("TYPE_RS_OTHER"), (int64_t) MONGOC_SERVER_RS_OTHER);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverServer_className.get(), makeStaticString("TYPE_RS_GHOST"), (int64_t) MONGOC_SERVER_RS_GHOST);

			/* MongoDb\Driver\WriteConcern */
			HHVM_MALIAS(MongoDB\\Driver\\WriteConcern, __construct, MongoDBDriverWriteConcern, __construct);
			HHVM_MALIAS(MongoDB\\Driver\\WriteConcern, __debugInfo, MongoDBDriverWriteConcern, __debugInfo);

			Native::registerNativeDataInfo<MongoDBDriverWriteConcernData>(MongoDBDriverWriteConcernData::s_className.get());

			Native::registerClassConstant<KindOfString>(s_MongoDriverWriteConcern_className.get(), makeStaticString("MAJORITY"), s_MongoDriverWriteConcern_majority.get());

			/* MongoDb\Driver\WriteResult */
			HHVM_MALIAS(MongoDB\\Driver\\WriteResult, getServer, MongoDBDriverWriteResult, getServer);
			HHVM_MALIAS(MongoDB\\Driver\\WriteResult, isAcknowledged, MongoDBDriverWriteResult, isAcknowledged);

			Native::registerNativeDataInfo<MongoDBDriverWriteResultData>(MongoDBDriverWriteResultData::s_className.get());

			loadSystemlib("mongodb");
			mongoc_init();
			mongoc_log_set_handler(hippo_log_handler, NULL);
		}
} s_mongodb_extension;

HHVM_GET_MODULE(mongodb)

} // namespace HPHP

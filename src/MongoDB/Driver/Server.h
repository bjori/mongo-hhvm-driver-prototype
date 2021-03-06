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
#ifndef __MONGODB_DRIVER_SERVER_H__
#define __MONGODB_DRIVER_SERVER_H__

extern "C" {
#include "../../../libmongoc/src/mongoc/mongoc.h"

#define MONGOC_I_AM_A_DRIVER
#include "../../../libmongoc/src/mongoc/mongoc-client-private.h"
#include "../../../libmongoc/src/mongoc/mongoc-server-description-private.h"
#include "../../../libmongoc/src/mongoc/mongoc-topology-description-private.h"
#undef MONGOC_I_AM_A_DRIVER
}

namespace HPHP {

extern const StaticString s_MongoDriverServer_className;

class MongoDBDriverServerData
{
	public:
		static Class* s_class;
		static const StaticString s_className;

		static Class* getClass();

		mongoc_client_t    *m_client;
		uint32_t            m_server_id;

		void sweep() {
		}

		~MongoDBDriverServerData() {
			sweep();
		};
};

bool mongodb_driver_add_server_debug_wrapper(void *item, void *context);

Array HHVM_METHOD(MongoDBDriverServer, __debugInfo);
String HHVM_METHOD(MongoDBDriverServer, getHost);
Array HHVM_METHOD(MongoDBDriverServer, getInfo);
int64_t HHVM_METHOD(MongoDBDriverServer, getLatency);
int64_t HHVM_METHOD(MongoDBDriverServer, getPort);
Array HHVM_METHOD(MongoDBDriverServer, getTags);
int64_t HHVM_METHOD(MongoDBDriverServer, getType);
Object HHVM_METHOD(MongoDBDriverServer, executeCommand, const String &db, const Object &command, const Variant &readPreference);
Object HHVM_METHOD(MongoDBDriverServer, executeQuery, const String &ns, const Object &query, const Variant &readPreference);
Object HHVM_METHOD(MongoDBDriverServer, executeBulkWrite, const String &ns, const Object &bulk, const Variant &writeConcern);
bool HHVM_METHOD(MongoDBDriverServer, isPrimary);
bool HHVM_METHOD(MongoDBDriverServer, isSecondary);
bool HHVM_METHOD(MongoDBDriverServer, isArbiter);
bool HHVM_METHOD(MongoDBDriverServer, isHidden);
bool HHVM_METHOD(MongoDBDriverServer, isPassive);

}
#endif


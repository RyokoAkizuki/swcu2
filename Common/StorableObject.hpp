/*
 * Copyright 2015 Yukino Hayakawa<tennencoll@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "Common.hpp"

namespace swcu {

class StorableObject
{
protected:
    std::string     mCollection;
    mongo::OID      mId;
    bool            mValid;

public:
    /**
     * Constructor.
     * Accept a collection name and an assigned object id.
     * Used to load an object.
     */
    StorableObject(const std::string& collection, const mongo::OID& oid);
    /**
     * Constructor.
     * Accept a collection name.
     * Used to create an object,
     * or load an object using field other than _id.
     */
    StorableObject(const std::string& collection);
    virtual ~StorableObject() {}

    /**
     * Indicates whether a document is available in database.
     * @return If this object has a document in database.
     */
            bool        isValid() const         { return mValid; }
            mongo::OID  getId() const           { return mId; }
            time_t      getTimestamp()          { return mId.asTimeT(); }

            bool        reload()                { return _loadObject(); }

protected:
    /**
     * Store the document to database.
     * This will generate an object id.
     * If isValid(), this operation will not perform and will print an error
     * message. Use _updateObject() instead.
     * @param  data Document, doesn't including the _id field.
     * @return      True if saved, false is failed or isValid().
     */
            bool        _createObject(const mongo::BSONObj& data);

    /**
     * Perform update operation using provided data.
     * You may update data members after this operation is successfully
     * performed.
     * @param  data Including operator and data.
     * @return      True if succeeded. False if failed or !inValid().
     */
            bool        _updateObject(const mongo::BSONObj& data);

    /**
     * Fetch document from database using information (mCollection and
     * mId) provided in constructor and parse it.
     * Indeed a shortcut to _loadObject(fieldname, value).
     * @return      Whether the specified document is found.
     *              True if does. False if not.
     */
            bool        _loadObject()   { return _loadObject("_id", mId); }

    /**
     * Fetch document using provided data.
     * You'd better not find a document using a field dosen't have
     * a unique index.
     * @param  fieldname Fieldname.
     * @param  value     Value. You may convert it to UTF8 if it's a string.
     * @return           Whether a document is found and parsed.
     */
    template<typename T>
            bool        _loadObject(const std::string& fieldname, T value)
    {
        MONGO_WRAPPER({
            auto doc = getDBConn()->findOne(
                mCollection, QUERY(fieldname << value)
            );
            if(doc.isEmpty())
            {
                LOG(ERROR) << "Document not found.";
                mValid  = false;
                return false;
            }
            else
            {
                mId     = doc["_id"].OID();
                mValid  = _parseObject(doc);
                return mValid;
            }
        });
        // Exception arised.
        return false;
    }

    /**
     * Will be called inside fetchObject().
     * Parse a document passed from fetchObject().
     * May validate data format and update data members.
     * This is a pure virtual function. Overload this function in
     * derived classes.
     * @param  data Document passed from fetchObject().
     * @return      Whether the document is valid.
     *              True if document is parsed.
     *              False if errors occured while parsing the document.
     */
    virtual bool        _parseObject(const mongo::BSONObj& data) = 0;

    /**
     * Shortcut to _updateObject() when updating a single field.
     */
    template<typename T>
            bool        _updateField(const char* operation,
        const char* fieldname, T value)
    {
        return _updateObject(BSON(operation << BSON(fieldname << value)));
    }
};

class EventLog : public StorableObject
{
public:
    EventLog(const std::string& subsystem, const std::string& event,
        const mongo::BSONObj& data);
    virtual ~EventLog() {}
    virtual bool        _parseObject(const mongo::BSONObj& /* data */)
    { return true; }
};

}

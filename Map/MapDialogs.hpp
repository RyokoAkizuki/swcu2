/*
 * Copyright 2014 Yukino Hayakawa<tennencoll@gmail.com>
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

#include <functional>
#include <memory>

#include "Map.hpp"
#include "../Interface/Dialog.hpp"

namespace swcu {

class Map;

class MapManagerDialog : public MenuDialog
{
public:
                    MapManagerDialog(int playerid);
    virtual         ~MapManagerDialog() {}

    virtual bool    build();
};

class MapViewDialog : public ItemListDialog<std::shared_ptr<Map>>
{
public:
    typedef std::function<bool(const std::shared_ptr<Map>&)> Filter;
    typedef std::function<void(const std::shared_ptr<Map>&)> Callback;

protected:
    Callback        mCallback;
    Filter          mFilter;

public:
                    MapViewDialog(int playerid,
        const std::string& title,
        Callback callback = [](const std::shared_ptr<Map>&) {},
        Filter filter = [](const std::shared_ptr<Map>&) { return true; });
    virtual         ~MapViewDialog() {}

    virtual bool    build();
    virtual bool    process(std::shared_ptr<Map> key);
};

class MapEditDialog : public MenuDialog
{
protected:
    std::shared_ptr<Map>    mMap;

public:
                    MapEditDialog(int playerid,
        const std::shared_ptr<Map>& map);
    virtual         ~MapEditDialog() {}

    virtual bool    build();
};

class MapSetTypeDialog : public RadioListDialog<MapType>
{
protected:
    std::shared_ptr<Map>    mMap;

public:
                    MapSetTypeDialog(int playerid, std::shared_ptr<Map> m);
    virtual         ~MapSetTypeDialog() {}

    virtual bool    build();
    virtual bool    process(MapType type);
};

class MapAddObjectDialog : public ItemListDialog<int>
{
protected:
    std::shared_ptr<Map>    mMap;

public:
                    MapAddObjectDialog(int playerid,
        const std::shared_ptr<Map>& map);
    virtual         ~MapAddObjectDialog() {}

    virtual bool    build();
    virtual bool    process(int objectid);
};

class PropertyDialog : public MenuDialog
{
public:
                    PropertyDialog(int playerid);
    virtual         ~PropertyDialog() {}

    virtual bool    build();
};

class PropertyEditDialog : public MenuDialog
{
protected:
    std::shared_ptr<Map>    mMap;

public:
                    PropertyEditDialog(int playerid,
        const std::shared_ptr<Map>& map);
    virtual         ~PropertyEditDialog() {}

    virtual bool    build();
};

class PropertySetNameDialog : public InputDialog
{
protected:
    enum State
    {
        INIT, FAIL
    } mState;
    std::shared_ptr<Map>    mMap;

public:
                    PropertySetNameDialog(int playerid,
        const std::shared_ptr<Map>& map);
    virtual         ~PropertySetNameDialog() {}

    virtual bool    build();
    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

class PropertySetPasswordDialog : public InputDialog
{
protected:
    enum State
    {
        INIT, FAIL
    } mState;
    std::shared_ptr<Map>    mMap;

public:
                    PropertySetPasswordDialog(int playerid,
        const std::shared_ptr<Map>& map);
    virtual         ~PropertySetPasswordDialog() {}

    virtual bool    build();
    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

class PropertySetPriceDialog : public InputDialog
{
protected:
    enum State
    {
        INIT, FAIL
    } mState;
    std::shared_ptr<Map>    mMap;

public:
                    PropertySetPriceDialog(int playerid,
        const std::shared_ptr<Map>& map);
    virtual         ~PropertySetPriceDialog() {}

    virtual bool    build();
    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

class ObjectSetTextDialog : public InputDialog
{
protected:
    int             mObjectId;
    Object*         mObjectPtr;

public:
                    ObjectSetTextDialog(int playerid, int oid, Object* check);
    virtual         ~ObjectSetTextDialog() {}

    virtual bool    build();
    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

}

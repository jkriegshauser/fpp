#pragma once
/*
 * This file is part of the Falcon Player (FPP) and is Copyright (C)
 * 2013-2022 by the Falcon Player Developers.
 *
 * The Falcon Player (FPP) is free software, and is covered under
 * multiple Open Source licenses.  Please see the included 'LICENSES'
 * file for descriptions of what files are covered by each license.
 *
 * This source file is covered under the LGPL v2.1 as described in the
 * included LICENSE.LGPL file.
 */

#include <condition_variable>
#include <list>
#include <map>
#include <mutex>
#include <set>
#include <shared_mutex>
#include <thread>

#if __has_include(<jsoncpp/json/json.h>)
#include <jsoncpp/json/json.h>
#elif __has_include(<json/json.h>)
#include <json/json.h>
#endif

constexpr int UNKNOWN_WARNING_ID = 0;

class FPPWarning : public Json::Value {
public:
    FPPWarning(int i, const std::string& m, const std::string& p);

    std::string message() const;
    std::string plugin() const;
    int id() const;

    int timeout;
};

class WarningListener {
public:
    virtual void handleWarnings(const std::list<FPPWarning>& warnings) = 0;
};

class WarningHolder {
public:
    static void AddWarning(const std::string& w);
    static void AddWarningTimeout(const std::string& w, int seconds);
    static void RemoveWarning(const std::string& w);

    static void AddWarning(int id, const std::string& w, const std::map<std::string, std::string>& data = {});
    static void AddWarningTimeout(int seconds, int id, const std::string& w, const std::map<std::string, std::string>& data = {}, const std::string& plugin = "");
    static void RemoveWarning(int id, const std::string& w, const std::string& plugin = "");
    static void RemoveAllWarnings();

    static void AddWarningListener(WarningListener* l);
    static void RemoveWarningListener(WarningListener* l);

    static std::list<FPPWarning> GetWarnings();
    static void StartNotifyThread();
    static void StopNotifyThread();

    static void WriteWarningsFile();
    static void ClearWarningsFile();

private:
    static std::shared_mutex warningsLock;
    static std::list<FPPWarning> warnings;
    static uint64_t timeToRemove;

    static std::mutex notifyLock;
    static std::condition_variable notifyCV;
    static std::thread* notifyThread;

    static std::set<WarningListener*> listenerList;
    static std::mutex listenerListLock;
    static volatile bool runNotifyThread;

    static void NotifyListenersMain(); // main for notify thread
    static void writeWarningsFile(const std::list<FPPWarning>& warnings);
    static void UpdateWarningsAndNotify(bool notify);
};

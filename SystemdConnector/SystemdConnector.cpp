/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2020 Metrological
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
 */

#include "SystemdConnector.h"

// #include <systemd/sd-daemon.h>

namespace WPEFramework {
namespace Plugin {

    namespace {

        static Metadata<SystemdConnector> metadata(
            // Version
            1, 0, 0,
            // Preconditions
            {},
            // Terminations
            {},
            // Controls
            {}
        );
    }

    SystemdConnector::SystemdConnector()
    {
    }

    /* virtual */ SystemdConnector::~SystemdConnector()
    {
    }

    /* virtual */ const string SystemdConnector::Initialize(PluginHost::IShell* service)
    {
        TRACE(Trace::Information, (_T("Notify systemd that the Platform is up and running.\n")));

        // int rc = sd_notifyf(0,
        //     "READY=1\n"
        //     "STATUS=Platform Server is Ready (from WPE Framework Compositor Plugin)\n"
        //     "MAINPID=%lu",
        //     ::getpid());
        // if (rc) {
        //     TRACE(Trace::Error, (_T("Notify Nexus Server Ready to systemd: FAILED (%d)\n"), rc));
        // } else {
        //     TRACE(Trace::Information, (_T("Notify Nexus Server Ready to systemd: OK\n")));
        // }
        {
            string token;
            printf("WPEFramework::Plugin::SystemdConnector::Initialize()->PID<%d><%d> calling QueryInterfaceByCallsign\n", getpid(), gettid());
            // TODO: use interfaces and remove token
            auto security = service->QueryInterfaceByCallsign<PluginHost::IAuthenticate>("SecurityAgent");
            if (nullptr != security)
            {
                printf("WPEFramework::Plugin::SystemdConnector::Initialize()->PID<%d><%d> calling security->CreateToken()\n", getpid(), gettid());
                string payload = "http://localhost";
                if (security->CreateToken(static_cast<uint16_t>(payload.length()),
                                            reinterpret_cast<const uint8_t *>(payload.c_str()),
                                            token) == Core::ERROR_NONE)
                {
                    printf("WPEFramework::Plugin::SystemdConnector::Initialize()->PID<%d><%d> got security token<%s>\n", getpid(), gettid(), token.c_str());
                }
                else
                {
                    printf("WPEFramework::Plugin::SystemdConnector::Initialize()->PID<%d><%d> failed to get security token\n", getpid(), gettid());
                }
                printf("WPEFramework::Plugin::SystemdConnector::Initialize()->PID<%d><%d> calling security->Release()\n", getpid(), gettid());
                security->Release();
            }
            else
            {
                printf("WPEFramework::Plugin::SystemdConnector::Initialize()->PID<%d><%d> No security agent\n", getpid(), gettid());
                TRACE(Trace::Information, (_T("No security agent\n")));
            }

            string query = "token=" + token;
        }
        // On success return empty, to indicate there is no error text.
        return (string());
    }

    /* virtual */ void SystemdConnector::Deinitialize(PluginHost::IShell* /* servicei */)
    {
        //Strangely enough systemd has no notification for shutting down, so do nothing.
    }

    /* virtual */ string SystemdConnector::Information() const
    {
        // No additional info to report.
        return (string());
    }

} // namespace Plugin
} // namespace WPEFramework

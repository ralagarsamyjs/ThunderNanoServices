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
 
#include "VolumeControl.h"

namespace WPEFramework {
namespace Plugin {

    namespace {

        static Metadata<VolumeControl> metadata(
            // Version
            1, 0, 0,
            // Preconditions
            { subsystem::PLATFORM },
            // Terminations
            {},
            // Controls
            {}
        );
    }

    const string VolumeControl::Initialize(PluginHost::IShell* service)
    {
        string message;

        ASSERT (_service == nullptr);
        ASSERT (service != nullptr);
        ASSERT (_implementation == nullptr);
        ASSERT (_connectionId == 0);
        printf("WPEFramework::Plugin::VolumeControl::Initialize()->PID<%d><%d>Entered\n", getpid(), gettid());
        _service = service;
        printf("WPEFramework::Plugin::VolumeControl::Initialize()->PID<%d><%d>calling _service->AddRef()\n", getpid(), gettid());
        _service->AddRef();
        printf("WPEFramework::Plugin::VolumeControl::Initialize()->PID<%d><%d>calling _service->Register(&_connectionNotification)\n", getpid(), gettid());
        _service->Register(&_connectionNotification);

        printf("WPEFramework::Plugin::VolumeControl::Initialize()->PID<%d><%d>calling _service->Root<Exchange::IVolumeControl>()\n", getpid(), gettid());
        _implementation = _service->Root<Exchange::IVolumeControl>(_connectionId, 2000, _T("VolumeControlImplementation"));
        if (_implementation == nullptr) {
            message = _T("Couldn't create volume control instance");
        } else {
          printf("WPEFramework::Plugin::VolumeControl::Initialize()->PID<%d><%d>calling _implementation->Register(&_volumeNotification)\n", getpid(), gettid());
          _implementation->Register(&_volumeNotification);
          printf("WPEFramework::Plugin::VolumeControl::Initialize()->PID<%d><%d>calling Exchange::JVolumeControl::Register(*this, _implementation)\n", getpid(), gettid());
          Exchange::JVolumeControl::Register(*this, _implementation);
        }
        printf("WPEFramework::Plugin::VolumeControl::Initialize()->PID<%d><%d>Exit \n", getpid(), gettid());
        return (message);
    }

    void VolumeControl::Deinitialize(PluginHost::IShell* service)
    {
        printf("WPEFramework::Plugin::VolumeControl::Deinitialize()->PID<%d><%d>Entered\n", getpid(), gettid());
        if (_service != nullptr) {
            ASSERT(_service == service);
            printf("WPEFramework::Plugin::VolumeControl::Deinitialize()->PID<%d><%d>calling service->Unregister(&_connectionNotification)\n", getpid(), gettid());
            service->Unregister(&_connectionNotification);

            if (_implementation != nullptr) {

                printf("WPEFramework::Plugin::VolumeControl::Deinitialize()->PID<%d><%d>calling Exchange::JVolumeControl::Unregister(*this)\n", getpid(), gettid());
                Exchange::JVolumeControl::Unregister(*this);
                printf("WPEFramework::Plugin::VolumeControl::Deinitialize()->PID<%d><%d>calling _implementation->Unregister(&_volumeNotification)\n", getpid(), gettid());
                _implementation->Unregister(&_volumeNotification);

                printf("WPEFramework::Plugin::VolumeControl::Deinitialize()->PID<%d><%d>calling connection(_service->RemoteConnection(_connectionId))\n", getpid(), gettid());
                RPC::IRemoteConnection* connection(_service->RemoteConnection(_connectionId));
                printf("WPEFramework::Plugin::VolumeControl::Deinitialize()->PID<%d><%d>calling _implementation->Release()\n", getpid(), gettid());
                VARIABLE_IS_NOT_USED uint32_t result = _implementation->Release();
                _implementation = nullptr;
                // It should have been the last reference we are releasing,
                // so it should endup in a DESTRUCTION_SUCCEEDED, if not we
                // are leaking...
                ASSERT(result == Core::ERROR_DESTRUCTION_SUCCEEDED);
                // The process can disappear in the meantime...
                if (connection != nullptr) {
                    printf("WPEFramework::Plugin::VolumeControl::Deinitialize()->PID<%d><%d>calling connection->Terminate()\n", getpid(), gettid());
                    // But if it did not dissapear in the meantime, forcefully terminate it. Shoot to kill :-)
                    connection->Terminate();
                    printf("WPEFramework::Plugin::VolumeControl::Deinitialize()->PID<%d><%d>calling cconnection->Release()\n", getpid(), gettid());
                    connection->Release();
                }
            }
            printf("WPEFramework::Plugin::VolumeControl::Deinitialize()->PID<%d><%d>calling _service->Release()\n", getpid(), gettid());
            _service->Release();
            _service = nullptr;
            _connectionId = 0;
        }
    }

    string VolumeControl::Information() const
    {
        return string();
    }

    void VolumeControl::Deactivated(RPC::IRemoteConnection* connection)
    {
        printf("WPEFramework::Plugin::VolumeControl::Deactivated()->PID<%d><%d>\n", getpid(), gettid());
        if (connection->Id() == _connectionId) {
            ASSERT(_service != nullptr);
            Core::IWorkerPool::Instance().Submit(PluginHost::IShell::Job::Create(_service,
                PluginHost::IShell::DEACTIVATED,
                PluginHost::IShell::FAILURE));
        }
    }

} // namespace Plugin
} // namespace WPEFramework

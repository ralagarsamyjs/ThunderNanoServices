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
 
#include "VolumeControlImplementation.h"

#include "VolumeControlPlatform.h"

namespace WPEFramework {
namespace Plugin {

    SERVICE_REGISTRATION(VolumeControlImplementation, 1, 0);

    VolumeControlImplementation::VolumeControlImplementation()
        : _adminLock{}
        , _platform{std::move(VolumeControlPlatform::Create(
                                [this]() { NotifyVolumeChange(); },
                                [this]() { NotifyMutedChange(); }))}
    {
        printf("WPEFramework::Plugin::VolumeControlImplementation()->PID<%d><%d> Conxtructor\n", getpid(), gettid());
    }

    VolumeControlImplementation::~VolumeControlImplementation() = default;

    void VolumeControlImplementation::Register(Exchange::IVolumeControl::INotification* notification)
    {
        ASSERT(notification);
        _adminLock.Lock();
        printf("WPEFramework::Plugin::VolumeControlImplementation::Register()->PID<%d><%d> notification->AddRef()\n", getpid(), gettid());
        notification->AddRef();
        printf("WPEFramework::Plugin::VolumeControlImplementation::Register()->PID<%d><%d> _notifications.push_back(notification)\n", getpid(), gettid());
        _notifications.push_back(notification);
        _adminLock.Unlock();
    }

    void VolumeControlImplementation::Unregister(const Exchange::IVolumeControl::INotification* notification)
    {
        ASSERT(notification);
        _adminLock.Lock();
        printf("WPEFramework::Plugin::VolumeControlImplementation::Unregister()->PID<%d><%d>_notifications.erase(item)\n", getpid(), gettid());
        auto item = std::find(_notifications.begin(), _notifications.end(), notification);
        ASSERT(item != _notifications.end());
        _notifications.erase(item);
        printf("WPEFramework::Plugin::VolumeControlImplementation::Unregister()->PID<%d><%d>(*item)->Release()\n", getpid(), gettid());
        (*item)->Release();
        _adminLock.Unlock();
    }


    uint32_t VolumeControlImplementation::Muted(const bool muted)
    {
        TRACE(Trace::Information, (_T("Set Muted: %s"), muted ? _T("true") : _T("false")));
        printf("WPEFramework::Plugin::VolumeControlImplementation::Muted(muted)->PID<%d><%d>_platform->Muted(muted)\n", getpid(), gettid());
        return _platform->Muted(muted);
    }

    uint32_t VolumeControlImplementation::Muted(bool& muted) const
    {
        printf("WPEFramework::Plugin::VolumeControlImplementation::Muted(muted)->PID<%d><%d>_platform->Muted(muted)\n", getpid(), gettid());
        muted = _platform->Muted();
        TRACE(Trace::Information, (_T("Get Muted: %s"), muted ? _T("true") : _T("false")));
        return Core::ERROR_NONE;
    }

    uint32_t VolumeControlImplementation::Volume(const uint8_t volume)
    {
        TRACE(Trace::Information, (_T("Set Volume: %d"), volume));
        printf("WPEFramework::Plugin::VolumeControlImplementation::Volume(vol)->PID<%d><%d>_platform->Volume(volume)\n", getpid(), gettid());
        return _platform->Volume(volume);
    }

    uint32_t VolumeControlImplementation::Volume(uint8_t& vol) const
    {
        printf("WPEFramework::Plugin::VolumeControlImplementation::Volume(&vol)->PID<%d><%d>_platform->Volume()\n", getpid(), gettid());
        vol = _platform->Volume();
        TRACE(Trace::Information, (_T("Get Volume: %d"), vol));
        return Core::ERROR_NONE;
    }

    void VolumeControlImplementation::NotifyVolumeChange()
    {
        _adminLock.Lock();
        printf("WPEFramework::Plugin::VolumeControlImplementation::NotifyVolumeChange()->PID<%d><%d>notification->Volume(_platform->Volume())\n", getpid(), gettid());
        for (auto* notification : _notifications) {
            notification->Volume(_platform->Volume());
        }
        _adminLock.Unlock();
    }

    void VolumeControlImplementation::NotifyMutedChange()
    {
        _adminLock.Lock();
        printf("WPEFramework::Plugin::VolumeControlImplementation::NotifyMutedChange()->PID<%d><%d>notification->Muted(_platform->Muted())\n", getpid(), gettid());
        for (auto* notification : _notifications) {
            notification->Muted(_platform->Muted());
        }
        _adminLock.Unlock();
    }

}  // namespace Plugin
}  // namespace WPEFramework

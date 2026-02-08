#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>

namespace vtask
{
    using Clock = std::chrono::steady_clock;

    enum class ProfileCategory : uint8_t
    {
        eExecutor,
        eTaskGraph,
        eUser
    };

    struct ProfileEvent
    {
        std::string_view name;
        ProfileCategory  category {};
        uint64_t         timestampNs {};
        uint32_t         threadId {};
        bool             begin {};
    };

    class IProfiler
    {
    public:
        virtual ~IProfiler() = default;

        virtual void     onEvent(const ProfileEvent& e) noexcept = 0;
        virtual uint32_t threadId() noexcept                     = 0;
    };

    class ProfileScope
    {
    public:
        ProfileScope(IProfiler* p, std::string_view name, ProfileCategory cat) noexcept :
            m_Profiler(p), m_Name(name), m_Category(cat)
        {
            if (m_Profiler)
                emit(true);
        }

        ~ProfileScope() noexcept
        {
            if (m_Profiler)
                emit(false);
        }

        ProfileScope(const ProfileScope&)            = delete;
        ProfileScope& operator=(const ProfileScope&) = delete;

    private:
        void emit(bool begin) noexcept
        {
            const auto ns =
                std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now().time_since_epoch()).count();

            ProfileEvent e {
                .name        = m_Name,
                .category    = m_Category,
                .timestampNs = static_cast<uint64_t>(ns),
                .threadId    = m_Profiler->threadId(),
                .begin       = begin,
            };

            m_Profiler->onEvent(e);
        }

        IProfiler*       m_Profiler {};
        std::string_view m_Name;
        ProfileCategory  m_Category {};
    };
} // namespace vtask

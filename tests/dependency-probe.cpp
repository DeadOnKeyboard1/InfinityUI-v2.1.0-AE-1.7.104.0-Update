#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/null_sink.h>

static_assert(FMT_VERSION == 120200);
static_assert(SPDLOG_VER_MAJOR == 1 && SPDLOG_VER_MINOR == 17);

int main()
{
    auto sink=std::make_shared<spdlog::sinks::null_sink_mt>();
    spdlog::logger logger("dependency-probe",sink);
    logger.info("{}",fmt::format("compiled fmt {}",FMT_VERSION));
    logger.flush();
}

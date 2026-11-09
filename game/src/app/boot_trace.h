#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <string>

namespace simpsons {

/// Etapas del arranque, en el mismo orden que `game/re/boot-sequence.md`.
enum class BootStage {
    Entrypoint = 0,
    Crt,
    Heap,
    FileSystem,
    Render,
    Audio,
    Input,
    Frontend,
    Gameflow,
    Level,
    Count,
};

const char* BootStageName(BootStage stage);
const char* BootStageSlug(BootStage stage);

enum class BootStatus { Pending, Reached, Failed, Skipped };
const char* BootStatusName(BootStatus status);

struct BootStageRecord {
    BootStatus status = BootStatus::Pending;
    std::string detail;
    double elapsed_ms = 0.0;
};

/// Traza el avance del arranque real. Es la fuente de verdad de los criterios
/// de salida de las fases 2 y 3: ventana/primer frame y frontend alcanzado.
class BootTrace {
public:
    explicit BootTrace(int log_level = 2);

    void Reset();

    void Mark(BootStage stage, std::string detail = {});
    void Fail(BootStage stage, std::string reason);
    void Skip(BootStage stage, std::string reason);

    bool Reached(BootStage stage) const;
    /// Verdadero solo si todas las etapas hasta `stage` (incluida) se alcanzaron.
    bool ReachedThrough(BootStage stage) const;
    bool AnyReached() const;
    BootStage Furthest() const;

    const BootStageRecord& At(BootStage stage) const;
    double ElapsedMs() const;
    std::string Summary() const;

    /// Escribe un informe Markdown (para `game/re/logs/boot-trace.md`).
    bool WriteMarkdown(const std::filesystem::path& path) const;

private:
    static constexpr std::size_t kStageCount = static_cast<std::size_t>(BootStage::Count);

    std::array<BootStageRecord, kStageCount> records_{};
    std::chrono::steady_clock::time_point start_;
    int log_level_ = 2;
};

} // namespace simpsons

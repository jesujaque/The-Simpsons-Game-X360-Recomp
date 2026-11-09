#include "app/boot_trace.h"

#include <fstream>
#include <iomanip>
#include <iterator>
#include <sstream>

namespace simpsons {
namespace {

constexpr const char* kNames[] = {
    "Entrypoint",
    "CRT / constructores",
    "Heap y memoria",
    "Sistema de archivos",
    "Init RenderWare",
    "Init audio",
    "Init input",
    "Frontend / menú",
    "Gameflow / Lua",
    "Carga de nivel",
};

constexpr const char* kSlugs[] = {
    "entrypoint", "crt", "heap", "filesystem", "render",
    "audio",      "input", "frontend", "gameflow", "level",
};

std::size_t Index(BootStage stage) {
    return static_cast<std::size_t>(stage);
}

} // namespace

const char* BootStageName(BootStage stage) {
    const std::size_t index = Index(stage);
    if (index >= std::size(kNames)) {
        return "Desconocida";
    }
    return kNames[index];
}

const char* BootStageSlug(BootStage stage) {
    const std::size_t index = Index(stage);
    if (index >= std::size(kSlugs)) {
        return "unknown";
    }
    return kSlugs[index];
}

const char* BootStatusName(BootStatus status) {
    switch (status) {
    case BootStatus::Pending:
        return "pendiente";
    case BootStatus::Reached:
        return "alcanzada";
    case BootStatus::Failed:
        return "fallida";
    case BootStatus::Skipped:
        return "omitida";
    }
    return "desconocido";
}

BootTrace::BootTrace(int log_level) : start_(std::chrono::steady_clock::now()), log_level_(log_level) {
    Reset();
}

void BootTrace::Reset() {
    for (auto& record : records_) {
        record = BootStageRecord{};
    }
    start_ = std::chrono::steady_clock::now();
}

void BootTrace::Mark(BootStage stage, std::string detail) {
    const std::size_t index = Index(stage);
    if (index >= records_.size()) {
        return;
    }
    const auto elapsed = std::chrono::duration<double, std::milli>(
        std::chrono::steady_clock::now() - start_);
    records_[index].status = BootStatus::Reached;
    records_[index].detail = std::move(detail);
    records_[index].elapsed_ms = elapsed.count();
}

void BootTrace::Fail(BootStage stage, std::string reason) {
    const std::size_t index = Index(stage);
    if (index >= records_.size()) {
        return;
    }
    const auto elapsed = std::chrono::duration<double, std::milli>(
        std::chrono::steady_clock::now() - start_);
    records_[index].status = BootStatus::Failed;
    records_[index].detail = std::move(reason);
    records_[index].elapsed_ms = elapsed.count();
}

void BootTrace::Skip(BootStage stage, std::string reason) {
    const std::size_t index = Index(stage);
    if (index >= records_.size()) {
        return;
    }
    const auto elapsed = std::chrono::duration<double, std::milli>(
        std::chrono::steady_clock::now() - start_);
    records_[index].status = BootStatus::Skipped;
    records_[index].detail = std::move(reason);
    records_[index].elapsed_ms = elapsed.count();
}

bool BootTrace::Reached(BootStage stage) const {
    const std::size_t index = Index(stage);
    return index < records_.size() && records_[index].status == BootStatus::Reached;
}

bool BootTrace::ReachedThrough(BootStage stage) const {
    const std::size_t end = Index(stage);
    if (end >= records_.size()) {
        return false;
    }
    for (std::size_t index = 0; index <= end; ++index) {
        if (records_[index].status != BootStatus::Reached) {
            return false;
        }
    }
    return true;
}

BootStage BootTrace::Furthest() const {
    for (std::size_t index = records_.size(); index > 0; --index) {
        if (records_[index - 1].status == BootStatus::Reached) {
            return static_cast<BootStage>(index - 1);
        }
    }
    return BootStage::Entrypoint;
}

bool BootTrace::AnyReached() const {
    for (const auto& record : records_) {
        if (record.status == BootStatus::Reached) {
            return true;
        }
    }
    return false;
}

const BootStageRecord& BootTrace::At(BootStage stage) const {
    static const BootStageRecord empty;
    const std::size_t index = Index(stage);
    if (index >= records_.size()) {
        return empty;
    }
    return records_[index];
}

double BootTrace::ElapsedMs() const {
    return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start_)
        .count();
}

std::string BootTrace::Summary() const {
    std::ostringstream out;
    if (!AnyReached()) {
        out << "arranque: sin etapas alcanzadas (" << std::fixed << std::setprecision(1)
            << ElapsedMs() << " ms)";
        return out.str();
    }

    out << "arranque: ";
    for (std::size_t index = 0; index < records_.size(); ++index) {
        const auto stage = static_cast<BootStage>(index);
        if (records_[index].status == BootStatus::Reached) {
            out << BootStageSlug(stage);
            if (index + 1 < records_.size() &&
                records_[index + 1].status == BootStatus::Reached) {
                out << " > ";
            }
        }
    }
    out << " (" << std::fixed << std::setprecision(1) << ElapsedMs() << " ms)";
    return out.str();
}

bool BootTrace::WriteMarkdown(const std::filesystem::path& path) const {
    std::error_code ec;
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path(), ec);
    }

    std::ofstream out{path, std::ios::binary | std::ios::trunc};
    if (!out) {
        return false;
    }

    out << "# Boot trace\n\n";
    out << "Generado por `SimpsonsRecomp`. Este archivo no se versiona.\n\n";
    out << "| # | Etapa | Estado | Detalle | ms |\n";
    out << "| --- | --- | --- | --- | --- |\n";
    for (std::size_t index = 0; index < records_.size(); ++index) {
        const auto stage = static_cast<BootStage>(index);
        out << "| " << index + 1 << " | " << BootStageName(stage) << " | "
            << BootStatusName(records_[index].status) << " | ";
        out << (records_[index].detail.empty() ? "—" : records_[index].detail);
        out << " | " << std::fixed << std::setprecision(1) << records_[index].elapsed_ms << " |\n";
    }
    out << "\n## Resumen\n\n";
    if (AnyReached()) {
        out << "- Etapa más avanzada: **" << BootStageName(Furthest()) << "**\n";
    } else {
        out << "- Etapa más avanzada: **ninguna** (el arranque no llegó a la primera etapa)\n";
    }
    out << "- Frontend alcanzado: "
        << (Reached(BootStage::Frontend) ? "sí" : "no") << "\n";
    out << "- Tiempo total: " << std::fixed << std::setprecision(1) << ElapsedMs() << " ms\n";
    out << "- log_level: " << log_level_ << "\n";
    return out.good();
}

} // namespace simpsons

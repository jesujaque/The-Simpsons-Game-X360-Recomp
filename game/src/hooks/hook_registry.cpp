#include "hooks/hook_registry.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace simpsons {
namespace {

std::string AddressText(std::uint32_t address) {
    std::ostringstream out;
    out << "0x" << std::uppercase << std::hex << std::setw(8) << std::setfill('0') << address;
    return out.str();
}

std::string DefaultStubName(std::uint32_t address) {
    return "unresolved_" + AddressText(address);
}

} // namespace

bool HookRegistry::Register(std::uint32_t address, std::string name, HookFn fn,
                            std::string note) {
    if (!fn) {
        return false;
    }
    std::lock_guard lock{mutex_};
    if (entries_.contains(address)) {
        return false;
    }
    Entry entry;
    entry.info = HookInfo{address, std::move(name), std::move(note), false};
    entry.fn = std::move(fn);
    entries_.emplace(address, std::move(entry));
    return true;
}

bool HookRegistry::RegisterStub(std::uint32_t address, std::string name,
                                std::uint32_t return_value, std::string note) {
    HookFn fn = [return_value](HookContext& context) {
        context.return_value = return_value;
        context.handled = true;
    };
    const bool registered = Register(address, std::move(name), std::move(fn), std::move(note));
    if (registered) {
        std::lock_guard lock{mutex_};
        const auto it = entries_.find(address);
        if (it != entries_.end()) {
            it->second.info.stub = true;
        }
    }
    return registered;
}

bool HookRegistry::Dispatch(std::uint32_t address, HookContext& context) const {
    HookFn fn;
    {
        std::lock_guard lock{mutex_};
        const auto it = entries_.find(address);
        if (it != entries_.end()) {
            fn = it->second.fn;
            context.name = it->second.info.name;
        } else {
            auto& unresolved = unresolved_[address];
            if (unresolved.first.empty()) {
                unresolved.first = DefaultStubName(address);
                if (context.name.empty()) {
                    context.name = unresolved.first;
                }
                std::cerr << "[hooks] llamada no resuelta " << AddressText(address) << "\n";
            }
            ++unresolved.second;
            return false;
        }
    }

    context.address = address;
    context.handled = true;
    fn(context);
    return true;
}

bool HookRegistry::IsRegistered(std::uint32_t address) const {
    std::lock_guard lock{mutex_};
    return entries_.contains(address);
}

std::size_t HookRegistry::UnresolvedCount() const {
    std::lock_guard lock{mutex_};
    std::size_t total = 0;
    for (const auto& [address, info] : unresolved_) {
        (void)address;
        total += info.second;
    }
    return total;
}

std::vector<UnresolvedCall> HookRegistry::UnresolvedCalls() const {
    std::lock_guard lock{mutex_};
    std::vector<UnresolvedCall> calls;
    calls.reserve(unresolved_.size());
    for (const auto& [address, info] : unresolved_) {
        calls.push_back(UnresolvedCall{address, info.first, info.second});
    }
    return calls;
}

std::vector<HookInfo> HookRegistry::Hooks() const {
    std::lock_guard lock{mutex_};
    std::vector<HookInfo> hooks;
    hooks.reserve(entries_.size());
    for (const auto& [address, entry] : entries_) {
        (void)address;
        hooks.push_back(entry.info);
    }
    return hooks;
}

bool HookRegistry::WriteReport(const std::filesystem::path& path) const {
    std::error_code ec;
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path(), ec);
    }

    std::ofstream out{path, std::ios::binary | std::ios::trunc};
    if (!out) {
        return false;
    }

    out << "# Hook report\n\n";
    out << "Generado por `SimpsonsRecomp`. Este archivo no se versiona.\n\n";

    out << "## Hooks registrados\n\n";
    out << "| Dirección | Nombre | Tipo | Nota |\n";
    out << "| --- | --- | --- | --- |\n";
    const auto hooks = Hooks();
    if (hooks.empty()) {
        out << "| — | — | — | — |\n";
    } else {
        for (const auto& hook : hooks) {
            out << "| `" << AddressText(hook.address) << "` | " << hook.name << " | "
                << (hook.stub ? "stub" : "hook") << " | "
                << (hook.note.empty() ? "—" : hook.note) << " |\n";
        }
    }

    out << "\n## Llamadas no resueltas\n\n";
    out << "| Dirección | Nombre | Llamadas |\n";
    out << "| --- | --- | --- |\n";
    const auto unresolved = UnresolvedCalls();
    if (unresolved.empty()) {
        out << "| — | — | — |\n";
    } else {
        for (const auto& call : unresolved) {
            out << "| `" << AddressText(call.address) << "` | " << call.name << " | "
                << call.count << " |\n";
        }
    }
    out << "\nAñade las direcciones nuevas a `game/functions_patch.toml` con "
           "`python tools/triage.py <log> --write`.\n";
    return out.good();
}

} // namespace simpsons

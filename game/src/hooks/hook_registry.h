#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace simpsons {

/// Contexto mínimo de una llamada interceptada. El runtime rellenará los
/// registros cuando la integración real esté enlazada; los hooks que no los
/// necesiten pueden ignorarlos.
struct HookContext {
    std::uint32_t address = 0;
    std::string name;
    std::array<std::uint32_t, 8> gpr{};
    std::array<float, 8> fpr{};
    std::uint32_t return_value = 0;
    bool handled = false;
};

using HookFn = std::function<void(HookContext&)>;

struct HookInfo {
    std::uint32_t address = 0;
    std::string name;
    std::string note;
    bool stub = false;
};

struct UnresolvedCall {
    std::uint32_t address = 0;
    std::string name;
    std::size_t count = 0;
};

/// Registro de hooks y stubs de funciones del juego.
///
/// Es el punto de entrada que usa el runtime cuando encuentra una función
/// recompilada que debe sustituirse o una llamada indirecta que no pudo
/// resolver. Las llamadas no registradas se acumulan para que `tools/triage.py`
/// las convierta en entradas de `functions_patch.toml`.
class HookRegistry {
public:
    /// Registra un hook. Devuelve false si la dirección ya estaba registrada.
    bool Register(std::uint32_t address, std::string name, HookFn fn, std::string note = {});

    /// Registra un stub que devuelve `return_value` y cuenta las llamadas.
    bool RegisterStub(std::uint32_t address, std::string name,
                      std::uint32_t return_value = 0, std::string note = {});

    /// Ejecuta el hook registrado. Si no hay ninguno, incrementa el contador de
    /// llamadas no resueltas y devuelve false.
    bool Dispatch(std::uint32_t address, HookContext& context) const;

    bool IsRegistered(std::uint32_t address) const;

    /// Si está activo, el llamador debe abortar tras un Dispatch fallido. El
    /// registro no aborta por sí mismo para poder probarlo y reportar antes.
    void SetUnresolvedFatal(bool fatal) { unresolved_fatal_ = fatal; }
    bool UnresolvedFatal() const { return unresolved_fatal_; }

    std::size_t UnresolvedCount() const;
    std::vector<UnresolvedCall> UnresolvedCalls() const;
    std::vector<HookInfo> Hooks() const;

    /// Escribe un informe Markdown con hooks y llamadas no resueltas.
    bool WriteReport(const std::filesystem::path& path) const;

private:
    struct Entry {
        HookInfo info;
        HookFn fn;
    };

    mutable std::mutex mutex_;
    std::map<std::uint32_t, Entry> entries_;
    mutable std::map<std::uint32_t, std::pair<std::string, std::size_t>> unresolved_;
    bool unresolved_fatal_ = false;
};

} // namespace simpsons

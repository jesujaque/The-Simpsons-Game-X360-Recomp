#include "app/boot_trace.h"
#include "app/config.h"
#include "assets/asset_catalog.h"
#include "hooks/hook_registry.h"
#include "input/keymap.h"
#include "platform/vfs.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

int g_failures = 0;
int g_checks = 0;

#define CHECK(condition)                                                                       \
    do {                                                                                       \
        ++g_checks;                                                                            \
        if (!(condition)) {                                                                    \
            ++g_failures;                                                                       \
            std::cerr << "FAIL " << __FILE__ << ":" << __LINE__ << ": " << #condition << "\n"; \
        }                                                                                      \
    } while (false)

struct TempDir {
    fs::path path;

    TempDir() {
        static int counter = 0;
        const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
        path = fs::temp_directory_path() /
               ("simpsons-core-" + std::to_string(stamp) + "-" + std::to_string(counter++));
        fs::create_directories(path);
    }

    ~TempDir() {
        std::error_code ec;
        fs::remove_all(path, ec);
    }

    TempDir(const TempDir&) = delete;
    TempDir& operator=(const TempDir&) = delete;
};

void WriteFile(const fs::path& path, const std::string& contents) {
    fs::create_directories(path.parent_path());
    std::ofstream out{path, std::ios::binary | std::ios::trunc};
    out << contents;
}

std::string ReadFile(const fs::path& path) {
    std::ifstream in{path, std::ios::binary};
    return std::string{std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}

fs::path Normalized(const fs::path& path) {
    return path.lexically_normal();
}

void TestConfig() {
    const auto document = simpsons::ConfigDocument::Parse(R"(
# comentario
[general]
game_root = "game/extracted"  # al final
language = 'es'
log_level = 3

[window]
width = 1280
height = 720
vsync = false

[input]
deadzone = 0.25
[input.bindings]
confirm = "gamepad_a"
jump = "gamepad_a"

[patches]
widescreen = false

[nota]
texto = "con # almohadilla"
)");

    CHECK(document.Errors().empty());
    CHECK(document.GetString("general.game_root") == "game/extracted");
    CHECK(document.GetString("general.language") == "es");
    CHECK(document.GetInt("window.width") == 1280);
    CHECK(document.GetBool("window.vsync") == false);
    CHECK(document.GetFloat("input.deadzone") == 0.25f);
    CHECK(document.GetString("nota.texto") == "con # almohadilla");

    const auto config = simpsons::ApplyConfig(document);
    CHECK(config.game_root == "game/extracted");
    CHECK(config.language == "es");
    CHECK(config.window.width == 1280);
    CHECK(config.window.vsync == false);
    CHECK(config.patches.widescreen == false);
    CHECK(config.input.bindings.at("confirm") == "gamepad_a");
    CHECK(config.input.bindings.at("jump") == "gamepad_a");

    const auto broken = simpsons::ConfigDocument::Parse("[window]\nwidth 1280\n");
    CHECK(!broken.Errors().empty());

    const auto missing = simpsons::LoadConfig(fs::path{"no-existe-config.toml"});
    CHECK(!missing.warnings.empty());
}

void TestVirtualFileSystem() {
    TempDir temp;
    WriteFile(temp.path / "Frontend.str", "frontend");
    WriteFile(temp.path / "sub" / "nivel.lvl", "nivel");

    simpsons::platform::VirtualFileSystem vfs;
    CHECK(vfs.Mount("d", temp.path));
    CHECK(vfs.Mount("device", temp.path));
    vfs.SetDefaultMount(temp.path);

    const auto frontend = vfs.Resolve("d:\\Frontend.str");
    CHECK(frontend.has_value());
    if (frontend) {
        CHECK(Normalized(*frontend) == Normalized(temp.path / "Frontend.str"));
    }

    const auto with_parent = vfs.Resolve("d:/sub/../Frontend.str");
    CHECK(with_parent.has_value());
    if (with_parent) {
        CHECK(Normalized(*with_parent) == Normalized(temp.path / "Frontend.str"));
    }

    CHECK(!vfs.Resolve("d:\\..\\evil.txt").has_value());

    const auto case_insensitive = vfs.ResolveExisting("d:\\frontend.STR");
    CHECK(case_insensitive.has_value());

    const auto bare = vfs.ResolveExisting("\\Frontend.str");
    CHECK(bare.has_value());

    const auto unknown_device = vfs.Resolve("x:\\Frontend.str");
    CHECK(unknown_device.has_value());
    if (unknown_device) {
        CHECK(Normalized(*unknown_device) == Normalized(temp.path / "Frontend.str"));
    }

    CHECK(!vfs.ResolveExisting("d:\\no-existe.str").has_value());
}

void TestBootTrace() {
    TempDir temp;
    simpsons::BootTrace empty;
    CHECK(!empty.AnyReached());

    simpsons::BootTrace trace{2};
    trace.Mark(simpsons::BootStage::Entrypoint, "host");
    trace.Mark(simpsons::BootStage::Crt, "simulado");
    trace.Mark(simpsons::BootStage::Heap, "simulado");
    trace.Mark(simpsons::BootStage::FileSystem, "vfs");
    trace.Mark(simpsons::BootStage::Render, "headless");
    trace.Mark(simpsons::BootStage::Audio, "stub");
    trace.Mark(simpsons::BootStage::Input, "stub");

    CHECK(trace.Reached(simpsons::BootStage::Input));
    CHECK(trace.ReachedThrough(simpsons::BootStage::Input));
    CHECK(!trace.ReachedThrough(simpsons::BootStage::Frontend));
    CHECK(trace.Furthest() == simpsons::BootStage::Input);

    trace.Skip(simpsons::BootStage::Frontend, "falta el XEX");
    CHECK(!trace.Reached(simpsons::BootStage::Frontend));
    CHECK(std::string{simpsons::BootStageName(simpsons::BootStage::Frontend)} == "Frontend / menú");

    const auto report = temp.path / "boot-trace.md";
    CHECK(trace.WriteMarkdown(report));
    const auto text = ReadFile(report);
    CHECK(text.find("Frontend / menú") != std::string::npos);
    CHECK(text.find("omitida") != std::string::npos);
}

void TestHookRegistry() {
    TempDir temp;
    simpsons::HookRegistry registry;

    simpsons::HookContext context;
    CHECK(!registry.Dispatch(0x82A71810, context));
    CHECK(registry.UnresolvedCount() == 1);

    CHECK(registry.RegisterStub(0x82A71810, "boot_init_callback", 7));
    simpsons::HookContext stubbed;
    CHECK(registry.Dispatch(0x82A71810, stubbed));
    CHECK(stubbed.return_value == 7);
    CHECK(registry.IsRegistered(0x82A71810));

    CHECK(registry.Register(0x828385B8, "content_load_level",
                            [](simpsons::HookContext& call) { call.return_value = 42; }));
    simpsons::HookContext hooked;
    CHECK(registry.Dispatch(0x828385B8, hooked));
    CHECK(hooked.return_value == 42);
    CHECK(hooked.name == "content_load_level");

    const auto unresolved = registry.UnresolvedCalls();
    CHECK(unresolved.size() == 1);
    if (!unresolved.empty()) {
        CHECK(unresolved.front().count == 1);
    }

    const auto report = temp.path / "hooks.md";
    CHECK(registry.WriteReport(report));
    const auto text = ReadFile(report);
    CHECK(text.find("0x82A71810") != std::string::npos);
    CHECK(text.find("content_load_level") != std::string::npos);
}

void TestAssetCatalog() {
    TempDir temp;
    WriteFile(temp.path / "default.xex", "XEX2fake");
    WriteFile(temp.path / "Frontend.str", "frontend");
    WriteFile(temp.path / "cmd.txt", "flags");
    WriteFile(temp.path / "level.lvl", "level");
    WriteFile(temp.path / "script.lua", "print('hola')");
    WriteFile(temp.path / "audio.xma", "xma");

    CHECK(simpsons::assets::Classify(temp.path / "default.xex") ==
          simpsons::assets::AssetKind::Xex);
    CHECK(simpsons::assets::Classify(temp.path / "Frontend.str") ==
          simpsons::assets::AssetKind::Str);
    CHECK(simpsons::assets::Classify(temp.path / "cmd.txt") ==
          simpsons::assets::AssetKind::Cmd);

    simpsons::assets::AssetCatalog catalog;
    catalog.Scan(temp.path);
    const auto summary = catalog.Summarize();
    CHECK(summary.has_xex);
    CHECK(summary.has_frontend);
    CHECK(summary.has_cmd);
    CHECK(summary.has_lua);
    CHECK(summary.total_files == 6);

    const auto found = catalog.Find("frontend.STR");
    CHECK(found.has_value());
    CHECK(simpsons::assets::AssetCatalog::MissingRequired(temp.path).empty());

    const auto probe = simpsons::assets::AssetCatalog::Probe(temp.path / "default.xex");
    CHECK(probe.magic == "XEX2");
    CHECK(probe.header_size > 0);

    TempDir empty;
    const auto missing = simpsons::assets::AssetCatalog::MissingRequired(empty.path);
    CHECK(missing.size() == 2);
}

void TestActionMap() {
    auto map = simpsons::input::ActionMap::Defaults();
    const auto actions = map.ActionsFor("gamepad_a");
    CHECK(!actions.empty());
    CHECK(actions.front() == simpsons::input::Action::Confirm);

    CHECK(map.BindFromName("keyboard_q", "interact"));
    CHECK(!map.BindFromName("keyboard_q", "accion_inventada"));

    const std::vector<std::string> pressed_codes{"gamepad_a", "keyboard_space"};
    const auto pressed = map.Pressed(pressed_codes);
    CHECK(pressed.contains(simpsons::input::Action::Confirm));
    CHECK(pressed.contains(simpsons::input::Action::Jump));

    CHECK(simpsons::input::ParseAction("move_up") == simpsons::input::Action::MoveUp);
    CHECK(!simpsons::input::ParseAction("no_existe").has_value());
}

} // namespace

int main() {
    TestConfig();
    TestVirtualFileSystem();
    TestBootTrace();
    TestHookRegistry();
    TestAssetCatalog();
    TestActionMap();

    if (g_failures == 0) {
        std::cout << "OK: " << g_checks << " comprobaciones.\n";
        return 0;
    }
    std::cerr << "Fallos: " << g_failures << "/" << g_checks << "\n";
    return 1;
}

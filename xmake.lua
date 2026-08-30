local project_name = "rtsyn-comedi"
local project_xmake_repo = "rtsyn-xmake-repo"

set_license("GPL-3.0-or-later")

add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate")
set_defaultmode("release")
if is_mode("release") then
    set_optimize("fastest")
    set_strip("all")
    set_symbols("hidden")
end

option("valgrind", { default = false, description = "Run tests with Valgrind" })
option("tests", { default = true, description = "Build tests" })

set_languages("c23")
add_requires("rtsyn-abi", "rtsyn-module-loader")
if has_config("tests") then
    add_requires("gtest", "rtsyn-test-utils")
end

local workspace = os.getenv("RTSYN_WORKSPACE")
if workspace then
    local repository_dir = path.join(workspace, project_xmake_repo)
    add_repositories(project_xmake_repo .. " " .. repository_dir)
else
    add_repositories(project_xmake_repo .. " https://github.com/seregioo/" .. project_xmake_repo .. ".git")
end

target(project_name)
set_kind("shared")
add_files("src/*.c")
add_files("src/**/*.c")
add_includedirs("include", { public = true })
add_packages("rtsyn-abi", "rtsyn-module-loader")
add_defines("RTSYN_COMEDI_ENABLE_DRIVER")
add_links("comedi", "m")
add_includedirs("src")
add_headerfiles("include/(rtsyn/**.h)")

target("comedi-loopback")
set_kind("binary")
add_files("tools/comedi_loopback.c")
add_links("comedi", "m")

target("comedi-digital-loopback")
set_kind("binary")
add_files("tools/comedi_digital_loopback.c")
add_links("comedi")

if has_config("tests") then
local rtsyn_modules = {
    { path = "comedi", name = "comedi", link_plugin = true },
    { path = "module_loader", name = "module_loader", load_plugin = true },
}

for _, rtsyn_module in ipairs(rtsyn_modules) do
    local tests_name = "tests/" .. rtsyn_module.path .. "-tests"
    target(tests_name)
    set_kind("binary")
    if has_config("valgrind") then
        add_rules("@rtsyn-test-utils/valgrind")
    end
    add_deps(project_name, { links = rtsyn_module.link_plugin == true })
    add_packages("gtest")
    add_packages("rtsyn-abi", "rtsyn-module-loader")
    add_links("gtest_main")
    add_includedirs("src")
    add_files("tests/" .. rtsyn_module.path .. ".cpp")
    if rtsyn_module.link_plugin then
        if is_plat("macosx") then
            add_rpathdirs("@loader_path/..")
        elseif not is_plat("windows") then
            add_rpathdirs("$ORIGIN/..")
        else
            after_build(function(target)
                local plugin = target:dep(project_name)
                os.cp(plugin:targetfile(), target:targetdir())
            end)
        end
    end
    if rtsyn_module.load_plugin then
        add_rules("@rtsyn-test-utils/loadable_package", {
            target = project_name,
            define = "RTSYN_TEST_MODULE_PATH",
        })
    end
    add_tests(rtsyn_module.name)
end
end

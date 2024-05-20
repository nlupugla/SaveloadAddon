#!python

import os, sys, platform, json, subprocess

import SCons


# Minimum target platform versions.
if "ios_min_version" not in ARGUMENTS:
    ARGUMENTS["ios_min_version"] = "11.0"
if "macos_deployment_target" not in ARGUMENTS:
    ARGUMENTS["macos_deployment_target"] = "10.9"
if "android_api_level" not in ARGUMENTS:
    ARGUMENTS["android_api_level"] = "21"

env = SConscript("godot-cpp/SConstruct").Clone()

opts = Variables([], ARGUMENTS)

# Dependencies
# env.Tool("cmake", toolpath=["tools"])

opts.Update(env)

result_path = os.path.join("addons", "Saveload", "bin")

# Our includes and sources
env.Append(CPPDEFINES=["GDEXTENSION", "TOOLS_ENABLED"])  # Tells our sources we are building a GDExtension, not a module.
sources = [
    "src/register_types.cpp",
    "src/saveload_api.cpp",
    "src/saveload_spawner.cpp",
    "src/saveload_synchronizer.cpp",
    "src/scene_saveload.cpp",
    "src/scene_saveload_config.cpp",
    "src/editor/saveload_editor.cpp",
    "src/editor/saveload_editor_plugin.cpp",
    "src/editor/saveload_property_selector.cpp",
    "src/editor/saveload_scene_tree_dialogue.cpp",
]

# Make the shared library
result_name = "saveload{}{}".format(env["suffix"], env["SHLIBSUFFIX"])
library = env.SharedLibrary(target=os.path.join(result_path, result_name), source=sources)

Default(library)

# # GDNativeLibrary
# extfile = env.InstallAs(os.path.join(result_path, "saveload.gdextension"), "misc/saveload.gdextension")
# Default(extfile)

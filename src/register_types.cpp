/**************************************************************************/
/*  register_types.cpp                                                    */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "register_types.h"

#include "saveload_api.h"
#include "saveload_spawner.h"
#include "saveload_synchronizer.h"
#include "scene_saveload.h"

#ifdef GDEXTENSION
#define TOOLS_ENABLED
#include "editor/saveload_editor.h"
#include <godot_cpp/classes/engine.hpp>
#endif

#ifdef TOOLS_ENABLED
#include "editor/saveload_editor_plugin.h"
#endif

static SaveloadAPI *saveload_api = NULL;

void initialize_saveload_module(ModuleInitializationLevel p_level) {

#ifdef GDEXTENSION
    if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
        GDREGISTER_CLASS(SaveloadAPI);
        GDREGISTER_CLASS(SceneSaveload);
        saveload_api = memnew(SceneSaveload);
        Engine::get_singleton()->register_singleton("SaveloadAPI", saveload_api);
    }
#elif
    if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS) {
        saveload_api = memnew(SceneSaveload);
        GDREGISTER_CLASS(SaveloadAPI);
        Engine::get_singleton()->add_singleton(Engine::Singleton("SaveloadAPI", saveload_api));
    }
#endif
    if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
        GDREGISTER_CLASS(SceneSaveloadConfig);
        GDREGISTER_CLASS(SaveloadSpawner);
        GDREGISTER_CLASS(SaveloadSynchronizer);
    }
#ifdef TOOLS_ENABLED
    if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
#ifdef GDEXTENSION
        GDREGISTER_CLASS(SaveloadEditor);
        GDREGISTER_CLASS(SaveloadEditorPlugin);
#endif
        EditorPlugins::add_by_type<SaveloadEditorPlugin>();
    }
#endif
}

void uninitialize_saveload_module(ModuleInitializationLevel p_level) {
    if (saveload_api) {
#ifdef GDEXTENSION
        Engine::get_singleton()->unregister_singleton("SaveloadAPI");
#elif
        Engine::get_singleton()->remove_singleton("SaveloadAPI");
#endif
        memdelete(saveload_api);
        saveload_api = NULL;
    }
}

#ifdef GDEXTENSION

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/memory.hpp>

using namespace godot;

extern "C" {
GDExtensionBool GDE_EXPORT
saveload_init(GDExtensionInterfaceGetProcAddress p_interface, const GDExtensionClassLibraryPtr p_library,
              GDExtensionInitialization *r_initialization) {
    GDExtensionBinding::InitObject init_obj(p_interface, p_library, r_initialization);

    init_obj.register_initializer(initialize_saveload_module);
    init_obj.register_terminator(uninitialize_saveload_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
}
#endif

/**************************************************************************/
/*  saveload_editor_plugin.cpp                                            */
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

#include "saveload_editor_plugin.h"

#include "../saveload_synchronizer.h"
#include "saveload_editor.h"

#ifdef GDEXTENSION

#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/scene_tree.hpp>

using namespace godot;

#else

#include "editor/editor_interface.h"
#include "editor/editor_node.h"

#endif

// SaveloadEditorPlugin

void SaveloadEditorPlugin::hide_bottom_panel() {
#ifdef GDEXTENSION
    EditorPlugin::hide_bottom_panel();
#else
    EditorNode::get_singleton()->hide_bottom_panel();
#endif
}

void SaveloadEditorPlugin::make_bottom_panel_item_visible(Control *item) {
#ifdef GDEXTENSION
    EditorPlugin::make_bottom_panel_item_visible(item);
#else
    EditorNode::get_singleton()->make_bottom_panel_item_visible(item);
#endif
}

Button *SaveloadEditorPlugin::add_control_to_bottom_panel(Control *control, const String &title) {
#ifdef GDEXTENSION
    return EditorPlugin::add_control_to_bottom_panel(control, tr(title));
#else
    return EditorNode::get_singleton()->add_bottom_panel_item(TTR(title), control);
#endif
}

SaveloadEditorPlugin::SaveloadEditorPlugin() {
    saveload_editor = memnew(SaveloadEditor);
    button = add_control_to_bottom_panel(saveload_editor, "Save Load");
    //button = EditorNode::get_singleton()->add_bottom_panel_item(TTR("Save Load"), saveload_editor);
    button->hide();
#ifdef GDEXTENSION
    Callable pinned_callable = Callable(this, StringName("_pinned"));
    saveload_editor->get_pin()->connect(StringName("pressed"), pinned_callable);
#elif
    saveload_editor->get_pin()->connect(StringName("pressed"), callable_mp(this, &SaveloadEditorPlugin::_pinned));
#endif
}

void SaveloadEditorPlugin::_open_request(const String &p_path) {
#ifdef GDEXTENSION
    get_editor_interface()->open_scene_from_path(p_path);
#elif
    EditorInterface::get_singleton()->open_scene_from_path(p_path);
#endif
}

void SaveloadEditorPlugin::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_ENTER_TREE: {
#ifdef GDEXTENSION
            Callable node_removed_callable = Callable(this, StringName("_node_removed"));
            get_tree()->connect(StringName("node_removed"), node_removed_callable);
#elif
            get_tree()->connect(StringName("node_removed"), callable_mp(this, &SaveloadEditorPlugin::_node_removed));
#endif
        }
            break;
    }
}

void SaveloadEditorPlugin::_node_removed(Node *p_node) {
    if (p_node && p_node == saveload_editor->get_current()) {
        saveload_editor->edit(nullptr);
        if (saveload_editor->is_visible_in_tree()) {
            hide_bottom_panel();
            //EditorNode::get_singleton()->hide_bottom_panel();
        }
        button->hide();
        saveload_editor->get_pin()->set_pressed(false);
    }
}

void SaveloadEditorPlugin::_pinned() {
    if (!saveload_editor->get_pin()->is_pressed()) {
        if (saveload_editor->is_visible_in_tree()) {
            hide_bottom_panel();
            //EditorNode::get_singleton()->hide_bottom_panel();
        }
        button->hide();
    }
}

#ifdef GDEXTENSION
void SaveloadEditorPlugin::_edit(Object *p_object) {
#elif
void SaveloadEditorPlugin::edit(Object *p_object) {
#endif
    saveload_editor->edit(Object::cast_to<SaveloadSynchronizer>(p_object));
}

#ifdef GDEXTENSION
bool SaveloadEditorPlugin::_handles(Object *p_object) const {
#elif
bool SaveloadEditorPlugin::handles(Object *p_object) const {
#endif
    return p_object->is_class("SaveloadSynchronizer");
}

#ifdef GDEXTENSION
void SaveloadEditorPlugin::_make_visible(bool p_visible) {
#elif
void SaveloadEditorPlugin::make_visible(bool p_visible) {
#endif
    if (p_visible) {
        button->show();
        make_bottom_panel_item_visible(saveload_editor);
        //EditorNode::get_singleton()->make_bottom_panel_item_visible(saveload_editor);
    } else if (!saveload_editor->get_pin()->is_pressed()) {
        if (saveload_editor->is_visible_in_tree()) {
            hide_bottom_panel();
            //EditorNode::get_singleton()->hide_bottom_panel();
        }
        button->hide();
    }
}

/**************************************************************************/
/*  saveload_editor.h                                                     */
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

#ifndef SAVELOAD_EDITOR_H
#define SAVELOAD_EDITOR_H

#include "../scene_saveload_config.h"

#ifdef GDEXTENSION

#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/v_box_container.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/tree.hpp>
#include <godot_cpp/classes/accept_dialog.hpp>
#include <godot_cpp/classes/confirmation_dialog.hpp>
#include <godot_cpp/classes/line_edit.hpp>
// TODO: scene_tree_editor
// TODO: property_selector
// TODO: dialogs

using namespace godot;

#else

#include "editor/editor_plugin.h"
#include "scene/gui/box_container.h"
#include "scene/gui/dialogs.h"
#include "editor/gui/scene_tree_editor.h"
#include "scene/gui/tree.h"
#include "editor/property_selector.h"

class ConfirmationDialog;
class AcceptDialog;
class LineEdit;
class Tree;
class TreeItem;

#endif

class PropertySelector;
class SceneTreeDialog;

class SaveloadSynchronizer;

class SaveloadEditor : public VBoxContainer {
	GDCLASS(SaveloadEditor, VBoxContainer);

private:
	SaveloadSynchronizer *current = nullptr;

	AcceptDialog *error_dialog = nullptr;
	ConfirmationDialog *delete_dialog = nullptr;
	Button *add_pick_button = nullptr;
	Button *add_from_path_button = nullptr;
	LineEdit *np_line_edit = nullptr;
	Label *drop_label = nullptr;

	Ref<SceneSaveloadConfig> config;
	NodePath deleting;
	Tree *tree = nullptr;

	PropertySelector *prop_selector = nullptr;
	SceneTreeDialog *pick_node = nullptr;
	NodePath adding_node_path;

	Button *pin = nullptr;

	Ref<Texture2D> _get_class_icon(const Node *p_node);

	void _add_pressed();
	void _tree_item_edited();
	void _tree_button_pressed(Object *p_item, int p_column, int p_id, MouseButton p_button);
	void _update_checked(const NodePath &p_prop, int p_column, bool p_checked);
	void _update_config();
	void _dialog_closed(bool p_confirmed);
	void _add_property(const NodePath &p_property, bool p_sync = true);

	void _pick_node_filter_text_changed(const String &p_newtext);
	void _pick_node_select_recursive(TreeItem *p_item, const String &p_filter, Vector<Node *> &p_select_candidates);
	void _pick_node_filter_input(const Ref<InputEvent> &p_ie);
	void _pick_node_selected(const NodePath &p_path);

	void _pick_new_property();
	void _pick_node_property_selected(const NodePath &p_name);

	bool _can_drop_data_fw(const Point2 &p_point, const Variant &p_data, Control *p_from) const;
	void _drop_data_fw(const Point2 &p_point, const Variant &p_data, Control *p_from);

	void _add_sync_property(const NodePath &p_path);

protected:
	static void _bind_methods();

	void _notification(int p_what);

public:
	void edit(SaveloadSynchronizer *p_object);
	SaveloadSynchronizer *get_current() const { return current; }

	Button *get_pin() { return pin; }
	SaveloadEditor();
	~SaveloadEditor() {}
};

#endif // SAVELOAD_EDITOR_H

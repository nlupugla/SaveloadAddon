/**************************************************************************/
/*  saveload_property_selector.cpp                                        */
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

#ifdef GDEXTENSION

#include "saveload_property_selector.h"
#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/script.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/accept_dialog.hpp>
#include <godot_cpp/classes/v_box_container.hpp>

using namespace godot;

//#include "core/os/keyboard.h"
//#include "editor/doc_tools.h"
//#include "editor/editor_help.h"
//#include "editor/editor_node.h"
//#include "editor/editor_scale.h"
//#include "scene/gui/line_edit.h"
//#include "scene/gui/rich_text_label.h"
//#include "scene/gui/tree.h"

void SaveloadPropertySelector::_text_changed(const String &p_newtext) {
    _update_search();
}

void SaveloadPropertySelector::_sbox_input(const Ref<InputEvent> &p_ie) {
    Ref<InputEventKey> k = p_ie;

    if (k.is_valid()) {
        switch (k->get_keycode()) {
            case KEY_UP:
            case KEY_DOWN:
            case KEY_PAGEUP:
            case KEY_PAGEDOWN: {
                search_options->_gui_input(k);
                search_box->accept_event();

                TreeItem *root = search_options->get_root();
                if (!root->get_first_child()) {
                    break;
                }

                TreeItem *current = search_options->get_selected();

                TreeItem *item = search_options->get_next_selected(root);
                while (item) {
                    item->deselect(0);
                    item = search_options->get_next_selected(item);
                }

                current->select(0);

            }
                break;
            default:
                break;
        }
    }
}

void SaveloadPropertySelector::_update_search() {
    set_title(tr("Select Property"));

    search_options->clear();
    //help_bit->set_text("");

    TreeItem *root = search_options->create_item();

    // Allow using spaces in place of underscores in the search string (makes the search more fault-tolerant).
    const String search_text = search_box->get_text().replace(" ", "_");

    TypedArray<Dictionary> props;
    if (instance) {
        props = instance->get_property_list();
    }

    TreeItem *category = nullptr;

    bool found = false;

    Ref<Texture2D> type_icons[] = {
            search_options->get_theme_icon(StringName("Variant"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("bool"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("int"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("float"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("String"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("Vector2"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("Vector2i"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("Rect2"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("Rect2i"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("Vector3"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("Vector3i"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("Transform2D"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("Vector4"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("Vector4i"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("Plane"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("Quaternion"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("AABB"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("Basis"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("Transform3D"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("Projection"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("Color"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("StringName"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("NodePath"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("RID"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("MiniObject"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("Callable"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("Signal"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("Dictionary"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("Array"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("PackedByteArray"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("PackedInt32Array"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("PackedInt64Array"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("PackedFloat32Array"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("PackedFloat64Array"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("PackedStringArray"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("PackedVector2Array"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("PackedVector3Array"), StringName("EditorIcons")),
            search_options->get_theme_icon(StringName("PackedColorArray"), StringName("EditorIcons"))
    };
    static_assert((sizeof(type_icons) / sizeof(type_icons[0])) == Variant::VARIANT_MAX,
                  "Number of type icons doesn't match the number of Variant types.");

    Dictionary script_icons = ProjectSettings::get_singleton()->get(StringName("_global_script_class_icons"));
    for (int i = 0; i < props.size(); i++) {
        const PropertyInfo E = dict_to_property_info(props[i]);
        if (E.usage == PROPERTY_USAGE_CATEGORY) {
            if (category && category->get_first_child() == nullptr) {
                memdelete(category); //old category was unused
            }
            category = search_options->create_item(root);
            category->set_text(0, E.name);
            category->set_selectable(0, false);

            Ref<Texture2D> icon;
            if (E.name == StringName("Script Variables")) {
                icon = search_options->get_theme_icon(StringName("Script"), StringName("EditorIcons"));
            } else {
                icon = ResourceLoader::get_singleton()->load(script_icons[E.name]);
            }
            category->set_icon(0, icon);
            continue;
        }

        if (!(E.usage & PROPERTY_USAGE_EDITOR) && !(E.usage & PROPERTY_USAGE_SCRIPT_VARIABLE)) {
            continue;
        }

        if (!search_box->get_text().is_empty() && E.name.findn(search_text) == -1) {
            continue;
        }

        if (type_filter.size() && !type_filter.has(E.type)) {
            continue;
        }

        TreeItem *item = search_options->create_item(category ? category : root);
        item->set_text(0, E.name);
        item->set_metadata(0, E.name);
        item->set_icon(0, type_icons[E.type]);

        if (!found && !search_box->get_text().is_empty() && E.name.findn(search_text) != -1) {
            item->select(0);
            found = true;
        }

        item->set_selectable(0, true);
    }

    if (category && category->get_first_child() == nullptr) {
        memdelete(category); //old category was unused
    }
    get_ok_button()->set_disabled(root->get_first_child() == nullptr);
}

void SaveloadPropertySelector::_confirmed() {
    TreeItem *ti = search_options->get_selected();
    if (!ti) {
        return;
    }
    emit_signal(StringName("selected"), ti->get_metadata(0));
    hide();
}

void SaveloadPropertySelector::_item_selected() {

    TreeItem *item = search_options->get_selected();
    if (!item) {
        return;
    }
    String name = item->get_metadata(0);

    String class_type;
    if (type != Variant::NIL) {
        class_type = Variant::get_type_name(type);
    } else if (!base_type.is_empty()) {
        class_type = base_type;
    } else if (instance) {
        class_type = instance->get_class();
    }

//    DocTools *dd = EditorHelp::get_doc_data();
//    String text;
//    if (properties) {
//        while (!class_type.is_empty()) {
//            HashMap<String, DocData::ClassDoc>::Iterator E = dd->class_list.find(class_type);
//            if (E) {
//                for (int i = 0; i < E->value.properties.size(); i++) {
//                    if (E->value.properties[i].name == name) {
//                        text = DTR(E->value.properties[i].description);
//                        break;
//                    }
//                }
//            }
//
//            if (!text.is_empty()) {
//                break;
//            }
//
//            // The property may be from a parent class, keep looking.
//            class_type = ClassDB::get_parent_class(class_type);
//        }
//    } else {
//        while (!class_type.is_empty()) {
//            HashMap<String, DocData::ClassDoc>::Iterator E = dd->class_list.find(class_type);
//            if (E) {
//                for (int i = 0; i < E->value.methods.size(); i++) {
//                    if (E->value.methods[i].name == name) {
//                        text = DTR(E->value.methods[i].description);
//                        break;
//                    }
//                }
//            }

//            if (!text.is_empty()) {
//                break;
//            }

            // The method may be from a parent class, keep looking.
//            class_type = ClassDB::get_parent_class(class_type);
//        }
//    }

//    if (!text.is_empty()) {
//        // Display both property name and description, since the help bit may be displayed
//        // far away from the location (especially if the dialog was resized to be taller).
//        help_bit->set_text(vformat("[b]%s[/b]: %s", name, text));
//        help_bit->get_rich_text()->set_self_modulate(Color(1, 1, 1, 1));
//    } else {
//        // Use nested `vformat()` as translators shouldn't interfere with BBCode tags.
//        help_bit->set_text(vformat(TTR("No description available for %s."), vformat("[b]%s[/b]", name)));
//        help_bit->get_rich_text()->set_self_modulate(Color(1, 1, 1, 0.5));
//    }
}

void SaveloadPropertySelector::_hide_requested() {
    //_cancel_pressed(); // From AcceptDialog.
}

void SaveloadPropertySelector::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_ENTER_TREE: {
            connect("confirmed", callable_mp(this, &SaveloadPropertySelector::_confirmed));
        }
            break;

        case NOTIFICATION_EXIT_TREE: {
            disconnect("confirmed", callable_mp(this, &SaveloadPropertySelector::_confirmed));
        }
            break;
    }
}

void SaveloadPropertySelector::select_property_from_instance(Object *p_instance, const String &p_current) {
    base_type = "";
    selected = p_current;
    type = Variant::NIL;
    script = ObjectID();
    instance = p_instance;
    virtuals_only = false;

    popup_centered_ratio(0.6);
    search_box->set_text("");
    search_box->grab_focus();
    _update_search();
}

void SaveloadPropertySelector::set_type_filter(const Vector<Variant::Type> &p_type_filter) {
    type_filter = p_type_filter;
}

void SaveloadPropertySelector::_bind_methods() {
    ADD_SIGNAL(MethodInfo("selected", PropertyInfo(Variant::STRING, "name")));
}

SaveloadPropertySelector::SaveloadPropertySelector() {
    VBoxContainer *vbc = memnew(VBoxContainer);
    add_child(vbc);
    //set_child_rect(vbc);
    search_box = memnew(LineEdit);
//    vbc->add_margin_child(tr("Search:"), search_box);
//    search_box->connect("text_changed", callable_mp(this, &SaveloadPropertySelector::_text_changed));
//    search_box->connect("gui_input", callable_mp(this, &SaveloadPropertySelector::_sbox_input));
//    search_options = memnew(Tree);
//    vbc->add_margin_child(tr("Matches:"), search_options, true);
//    set_ok_button_text(tr("Open"));
//    get_ok_button()->set_disabled(true);
//    register_text_enter(search_box);
//    set_hide_on_ok(false);
//    search_options->connect("item_activated", callable_mp(this, &SaveloadPropertySelector::_confirmed));
//    search_options->connect("cell_selected", callable_mp(this, &SaveloadPropertySelector::_item_selected));
//    search_options->set_hide_root(true);
//    search_options->set_hide_folding(true);
//
//    help_bit = memnew(EditorHelpBit);
//    vbc->add_margin_child(TTR("Description:"), help_bit);
//    help_bit->connect("request_hide", callable_mp(this, &PropertySelector::_hide_requested));
}

#endif

/* SPDX-FileCopyrightText: 2001-2002 NaN Holding BV. All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

/** \file
 * \ingroup bke
 */

/* XXX temporary, until AssetHandle is designed properly and queries can return a pointer to it. */
#include "DNA_asset_types.h"

#include "BLI_string_ref.hh"
#include "BLI_utildefines.h"
#include "BLI_vector.hh"

#include "DNA_listBase.h"
#include "DNA_object_enums.h"
#include "RNA_types.hh"

struct ARegion;
struct Base;
struct bGPdata;
struct bGPDframe;
struct bGPDlayer;
struct bPoseChannel;
struct bScreen;
struct CacheFile;
struct Collection;
struct Depsgraph;
struct EditBone;
struct ID;
struct Image;
struct LayerCollection;
struct ListBase;
struct Main;
struct Mask;
struct MovieClip;
struct Object;
struct PointerRNA;
struct RegionView3D;
struct RenderEngineType;
struct ReportList;
struct Scene;
struct ScrArea;
struct SpaceAction;
struct SpaceClip;
struct SpaceClip;
struct SpaceConsole;
struct SpaceFile;
struct SpaceGraph;
struct SpaceImage;
struct SpaceInfo;
struct SpaceLink;
struct SpaceNla;
struct SpaceNode;
struct SpaceOutliner;
struct SpaceProperties;
struct SpaceSeq;
struct SpaceSpreadsheet;
struct SpaceText;
struct SpaceTopBar;
struct SpaceUserPref;
struct StructRNA;
struct Text;
struct ToolSettings;
struct View3D;
struct ViewLayer;
struct wmGizmoGroup;
struct wmMsgBus;
struct wmWindow;
struct wmWindowManager;
struct WorkSpace;

/* Structs */

struct bContext;

struct bContextDataResult;

/* Result of context lookups.
 * The specific values are important, and used implicitly in ctx_data_get(). Some functions also
 * still accept/return `int` instead, to ensure that the compiler uses the correct storage size
 * when mixing C/C++ code. */
enum eContextResult {
  /* The context member was found, and its data is available. */
  CTX_RESULT_OK = 1,

  /* The context member was not found. */
  CTX_RESULT_MEMBER_NOT_FOUND = 0,

  /* The context member was found, but its data is not available.
   * For example, "active_bone" is a valid context member, but has not data in Object mode. */
  CTX_RESULT_NO_DATA = -1,
};

/* Function mapping a context member name to its value. */
using bContextDataCallback = int /*eContextResult*/ (*)(const bContext *C,
                                                        const char *member,
                                                        bContextDataResult *result);

struct bContextStoreEntry {
  std::string name;
  PointerRNA ptr;
};

struct bContextStore {
  blender::Vector<bContextStoreEntry> entries;
  bool used = false;
};

namespace blender::asset_system {
class AssetRepresentation;
}

/* for the context's rna mode enum
 * keep aligned with data_mode_strings in context.cc */
enum eContextObjectMode {
  CTX_MODE_EDIT_MESH = 0,
  CTX_MODE_EDIT_CURVE,
  CTX_MODE_EDIT_SURFACE,
  CTX_MODE_EDIT_TEXT,
  CTX_MODE_EDIT_ARMATURE,
  CTX_MODE_EDIT_METABALL,
  CTX_MODE_EDIT_LATTICE,
  CTX_MODE_EDIT_CURVES,
  CTX_MODE_EDIT_GREASE_PENCIL,
  CTX_MODE_EDIT_POINT_CLOUD,
  CTX_MODE_POSE,
  CTX_MODE_SCULPT,
  CTX_MODE_PAINT_WEIGHT,
  CTX_MODE_PAINT_VERTEX,
  CTX_MODE_PAINT_TEXTURE,
  CTX_MODE_PARTICLE,
  CTX_MODE_OBJECT,
  CTX_MODE_PAINT_GPENCIL_LEGACY,
  CTX_MODE_EDIT_GPENCIL_LEGACY,
  CTX_MODE_SCULPT_GPENCIL_LEGACY,
  CTX_MODE_WEIGHT_GPENCIL_LEGACY,
  CTX_MODE_VERTEX_GPENCIL_LEGACY,
  CTX_MODE_SCULPT_CURVES,
  CTX_MODE_PAINT_GREASE_PENCIL,
};
#define CTX_MODE_NUM (CTX_MODE_PAINT_GREASE_PENCIL + 1)

/* Context */

bContext *CTX_create();
void CTX_free(bContext *C);

bContext *CTX_copy(const bContext *C);

/* Stored Context */

bContextStore *CTX_store_add(blender::Vector<std::unique_ptr<bContextStore>> &contexts,
                             blender::StringRefNull name,
                             const PointerRNA *ptr);
bContextStore *CTX_store_add_all(blender::Vector<std::unique_ptr<bContextStore>> &contexts,
                                 const bContextStore *context);
const bContextStore *CTX_store_get(const bContext *C);
void CTX_store_set(bContext *C, const bContextStore *store);
const PointerRNA *CTX_store_ptr_lookup(const bContextStore *store,
                                       blender::StringRefNull name,
                                       const StructRNA *type = nullptr);

/* need to store if python is initialized or not */
bool CTX_py_init_get(bContext *C);
void CTX_py_init_set(bContext *C, bool value);

void *CTX_py_dict_get(const bContext *C);
void *CTX_py_dict_get_orig(const bContext *C);

struct bContext_PyState {
  void *py_context;
  void *py_context_orig;
};
void CTX_py_state_push(bContext *C, bContext_PyState *pystate, void *value);
void CTX_py_state_pop(bContext *C, bContext_PyState *pystate);

/* Window Manager Context */

wmWindowManager *CTX_wm_manager(const bContext *C);
wmWindow *CTX_wm_window(const bContext *C);
WorkSpace *CTX_wm_workspace(const bContext *C);
bScreen *CTX_wm_screen(const bContext *C);
ScrArea *CTX_wm_area(const bContext *C);
SpaceLink *CTX_wm_space_data(const bContext *C);
ARegion *CTX_wm_region(const bContext *C);
void *CTX_wm_region_data(const bContext *C);
ARegion *CTX_wm_menu(const bContext *C);
wmGizmoGroup *CTX_wm_gizmo_group(const bContext *C);
wmMsgBus *CTX_wm_message_bus(const bContext *C);
ReportList *CTX_wm_reports(const bContext *C);

View3D *CTX_wm_view3d(const bContext *C);
RegionView3D *CTX_wm_region_view3d(const bContext *C);
SpaceText *CTX_wm_space_text(const bContext *C);
SpaceImage *CTX_wm_space_image(const bContext *C);
SpaceConsole *CTX_wm_space_console(const bContext *C);
SpaceProperties *CTX_wm_space_properties(const bContext *C);
SpaceFile *CTX_wm_space_file(const bContext *C);
SpaceSeq *CTX_wm_space_seq(const bContext *C);
SpaceOutliner *CTX_wm_space_outliner(const bContext *C);
SpaceNla *CTX_wm_space_nla(const bContext *C);
SpaceNode *CTX_wm_space_node(const bContext *C);
SpaceGraph *CTX_wm_space_graph(const bContext *C);
SpaceAction *CTX_wm_space_action(const bContext *C);
SpaceInfo *CTX_wm_space_info(const bContext *C);
SpaceUserPref *CTX_wm_space_userpref(const bContext *C);
SpaceClip *CTX_wm_space_clip(const bContext *C);
SpaceTopBar *CTX_wm_space_topbar(const bContext *C);
SpaceSpreadsheet *CTX_wm_space_spreadsheet(const bContext *C);

void CTX_wm_manager_set(bContext *C, wmWindowManager *wm);
void CTX_wm_window_set(bContext *C, wmWindow *win);
void CTX_wm_screen_set(bContext *C, bScreen *screen); /* to be removed */
void CTX_wm_area_set(bContext *C, ScrArea *area);
void CTX_wm_region_set(bContext *C, ARegion *region);
void CTX_wm_menu_set(bContext *C, ARegion *menu);
void CTX_wm_gizmo_group_set(bContext *C, wmGizmoGroup *gzgroup);

/**
 * Values to create the message that describes the reason poll failed.
 *
 * \note This must be called in the same context as the poll function that created it.
 */
struct bContextPollMsgDyn_Params {
  /** The result is allocated. */
  char *(*get_fn)(bContext *C, void *user_data);
  /** Optionally free the user-data. */
  void (*free_fn)(bContext *C, void *user_data);
  void *user_data;
};

const char *CTX_wm_operator_poll_msg_get(bContext *C, bool *r_free);
void CTX_wm_operator_poll_msg_set(bContext *C, const char *msg);
void CTX_wm_operator_poll_msg_set_dynamic(bContext *C, const bContextPollMsgDyn_Params *params);
void CTX_wm_operator_poll_msg_clear(bContext *C);

/* Data Context
 *
 * - #ListBase consists of #CollectionPointerLink items and must be
 *   freed with #BLI_freelistN!
 * - The dir #ListBase consists of #LinkData items.
 */

/* data type, needed so we can tell between a NULL pointer and an empty list */
enum {
  CTX_DATA_TYPE_POINTER = 0,
  CTX_DATA_TYPE_COLLECTION,
  CTX_DATA_TYPE_PROPERTY,
};

PointerRNA CTX_data_pointer_get(const bContext *C, const char *member);
PointerRNA CTX_data_pointer_get_type(const bContext *C, const char *member, StructRNA *type);
PointerRNA CTX_data_pointer_get_type_silent(const bContext *C,
                                            const char *member,
                                            StructRNA *type);
ListBase CTX_data_collection_get(const bContext *C, const char *member);

/**
 * For each pointer in collection_pointers, remap it to point to `ptr->propname`.
 *
 * Example:
 *
 *   lb = CTX_data_collection_get(C, "selected_pose_bones"); // lb contains pose bones.
 *   CTX_data_collection_remap_property(lb, "color");        // lb now contains bone colors.
 *
 * NOTE: this alters the items contained in the given listbase.
 * It does not change the listbase itself.
 */
void CTX_data_collection_remap_property(ListBase /*CollectionPointerLink*/ collection_pointers,
                                        const char *propname);

/**
 * \param C: Context.
 * \param use_store: Use 'C->wm.store'.
 * \param use_rna: Use Include the properties from 'RNA_Context'.
 * \param use_all: Don't skip values (currently only "scene").
 */
ListBase CTX_data_dir_get_ex(const bContext *C, bool use_store, bool use_rna, bool use_all);
ListBase CTX_data_dir_get(const bContext *C);
int /*eContextResult*/ CTX_data_get(const bContext *C,
                                    const char *member,
                                    PointerRNA *r_ptr,
                                    ListBase *r_lb,
                                    PropertyRNA **r_prop,
                                    int *r_index,
                                    short *r_type);

void CTX_data_id_pointer_set(bContextDataResult *result, ID *id);
void CTX_data_pointer_set_ptr(bContextDataResult *result, const PointerRNA *ptr);
void CTX_data_pointer_set(bContextDataResult *result, ID *id, StructRNA *type, void *data);

void CTX_data_id_list_add(bContextDataResult *result, ID *id);
void CTX_data_list_add_ptr(bContextDataResult *result, const PointerRNA *ptr);
void CTX_data_list_add(bContextDataResult *result, ID *id, StructRNA *type, void *data);

/**
 * Stores a property in a result. Make sure to also call
 * `CTX_data_type_set(result, CTX_DATA_TYPE_PROPERTY)`.
 * \param result: The result to store the property in.
 * \param prop: The property to store.
 * \param index: The particular index in the property to store.
 */
void CTX_data_prop_set(bContextDataResult *result, PropertyRNA *prop, int index);

void CTX_data_dir_set(bContextDataResult *result, const char **dir);

void CTX_data_type_set(bContextDataResult *result, short type);
short CTX_data_type_get(bContextDataResult *result);

bool CTX_data_equals(const char *member, const char *str);
bool CTX_data_dir(const char *member);

#define CTX_DATA_BEGIN(C, Type, instance, member) \
  { \
    ListBase ctx_data_list; \
    CollectionPointerLink *ctx_link; \
    CTX_data_##member(C, &ctx_data_list); \
    for (ctx_link = (CollectionPointerLink *)ctx_data_list.first; ctx_link; \
         ctx_link = ctx_link->next) \
    { \
      Type instance = (Type)ctx_link->ptr.data;

#define CTX_DATA_END \
  } \
  BLI_freelistN(&ctx_data_list); \
  } \
  (void)0

#define CTX_DATA_BEGIN_WITH_ID(C, Type, instance, member, Type_id, instance_id) \
  CTX_DATA_BEGIN (C, Type, instance, member) \
    Type_id instance_id = (Type_id)ctx_link->ptr.owner_id;

int ctx_data_list_count(const bContext *C, bool (*func)(const bContext *, ListBase *));

#define CTX_DATA_COUNT(C, member) ctx_data_list_count(C, CTX_data_##member)

/* Data Context Members */

Main *CTX_data_main(const bContext *C);
Scene *CTX_data_scene(const bContext *C);
/**
 * This is tricky. Sometimes the user overrides the render_layer
 * but not the scene_collection. In this case what to do?
 *
 * If the scene_collection is linked to the #ViewLayer we use it.
 * Otherwise we fallback to the active one of the #ViewLayer.
 */
LayerCollection *CTX_data_layer_collection(const bContext *C);
Collection *CTX_data_collection(const bContext *C);
ViewLayer *CTX_data_view_layer(const bContext *C);
RenderEngineType *CTX_data_engine_type(const bContext *C);
ToolSettings *CTX_data_tool_settings(const bContext *C);

const char *CTX_data_mode_string(const bContext *C);
enum eContextObjectMode CTX_data_mode_enum_ex(const Object *obedit,
                                              const Object *ob,
                                              eObjectMode object_mode);
enum eContextObjectMode CTX_data_mode_enum(const bContext *C);

void CTX_data_main_set(bContext *C, Main *bmain);
void CTX_data_scene_set(bContext *C, Scene *scene);

/* Only Outliner currently! */
bool CTX_data_selected_ids(const bContext *C, ListBase *list);

bool CTX_data_selected_editable_objects(const bContext *C, ListBase *list);
bool CTX_data_selected_editable_bases(const bContext *C, ListBase *list);

bool CTX_data_editable_objects(const bContext *C, ListBase *list);
bool CTX_data_editable_bases(const bContext *C, ListBase *list);

bool CTX_data_selected_objects(const bContext *C, ListBase *list);
bool CTX_data_selected_bases(const bContext *C, ListBase *list);

bool CTX_data_visible_objects(const bContext *C, ListBase *list);
bool CTX_data_visible_bases(const bContext *C, ListBase *list);

bool CTX_data_selectable_objects(const bContext *C, ListBase *list);
bool CTX_data_selectable_bases(const bContext *C, ListBase *list);

Object *CTX_data_active_object(const bContext *C);
Base *CTX_data_active_base(const bContext *C);
Object *CTX_data_edit_object(const bContext *C);

Image *CTX_data_edit_image(const bContext *C);

Text *CTX_data_edit_text(const bContext *C);
MovieClip *CTX_data_edit_movieclip(const bContext *C);
Mask *CTX_data_edit_mask(const bContext *C);

CacheFile *CTX_data_edit_cachefile(const bContext *C);

bool CTX_data_selected_nodes(const bContext *C, ListBase *list);

EditBone *CTX_data_active_bone(const bContext *C);
bool CTX_data_selected_bones(const bContext *C, ListBase *list);
bool CTX_data_selected_editable_bones(const bContext *C, ListBase *list);
bool CTX_data_visible_bones(const bContext *C, ListBase *list);
bool CTX_data_editable_bones(const bContext *C, ListBase *list);

bPoseChannel *CTX_data_active_pose_bone(const bContext *C);
bool CTX_data_selected_pose_bones(const bContext *C, ListBase *list);
bool CTX_data_selected_pose_bones_from_active_object(const bContext *C, ListBase *list);
bool CTX_data_visible_pose_bones(const bContext *C, ListBase *list);

bGPdata *CTX_data_gpencil_data(const bContext *C);
bGPDlayer *CTX_data_active_gpencil_layer(const bContext *C);
bGPDframe *CTX_data_active_gpencil_frame(const bContext *C);
bool CTX_data_visible_gpencil_layers(const bContext *C, ListBase *list);
bool CTX_data_editable_gpencil_layers(const bContext *C, ListBase *list);
bool CTX_data_editable_gpencil_strokes(const bContext *C, ListBase *list);

const AssetLibraryReference *CTX_wm_asset_library_ref(const bContext *C);
class blender::asset_system::AssetRepresentation *CTX_wm_asset(const bContext *C);

bool CTX_wm_interface_locked(const bContext *C);

/**
 * Gets pointer to the dependency graph.
 * If it doesn't exist yet, it will be allocated.
 *
 * The result dependency graph is NOT guaranteed to be up-to-date neither from relation nor from
 * evaluated data points of view.
 *
 * \note Can not be used if access to a fully evaluated data-block is needed.
 */
Depsgraph *CTX_data_depsgraph_pointer(const bContext *C);

/**
 * Get dependency graph which is expected to be fully evaluated.
 *
 * In the release builds it is the same as CTX_data_depsgraph_pointer(). In the debug builds extra
 * sanity checks are done. Additionally, this provides more semantic meaning to what is exactly
 * expected to happen.
 */
Depsgraph *CTX_data_expect_evaluated_depsgraph(const bContext *C);

/**
 * Gets fully updated and evaluated dependency graph.
 *
 * All the relations and evaluated objects are guaranteed to be up to date.
 *
 * \note Will be expensive if there are relations or objects tagged for update.
 * \note If there are pending updates depsgraph hooks will be invoked.
 */
Depsgraph *CTX_data_ensure_evaluated_depsgraph(const bContext *C);

/* Will Return NULL if depsgraph is not allocated yet.
 * Only used by handful of operators which are run on file load.
 */
Depsgraph *CTX_data_depsgraph_on_load(const bContext *C);
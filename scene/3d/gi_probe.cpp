/*************************************************************************/
/*  gi_probe.cpp                                                         */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2020 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2020 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "gi_probe.h"

#include "core/os/os.h"

#include "mesh_instance.h"
#include "voxelizer.h"

void GIProbeData::_set_data(const Dictionary &p_data) {
	ERR_FAIL_COND(!p_data.has("bounds"));
	ERR_FAIL_COND(!p_data.has("octree_size"));
	ERR_FAIL_COND(!p_data.has("octree_cells"));
	ERR_FAIL_COND(!p_data.has("octree_data"));
	ERR_FAIL_COND(!p_data.has("octree_df") && !p_data.has("octree_df_png"));
	ERR_FAIL_COND(!p_data.has("level_counts"));
	ERR_FAIL_COND(!p_data.has("to_cell_xform"));

	AABB bounds = p_data["bounds"];
	Vector3 octree_size = p_data["octree_size"];
	Vector<uint8_t> octree_cells = p_data["octree_cells"];
	Vector<uint8_t> octree_data = p_data["octree_data"];

	Vector<uint8_t> octree_df;
	if (p_data.has("octree_df")) {
		octree_df = p_data["octree_df"];
	} else if (p_data.has("octree_df_png")) {
		Vector<uint8_t> octree_df_png = p_data["octree_df_png"];
		Ref<Image> img;
		img.instance();
		Error err = img->load_png_from_buffer(octree_df_png);
		ERR_FAIL_COND(err != OK);
		ERR_FAIL_COND(img->get_format() != Image::FORMAT_L8);
		octree_df = img->get_data();
	}
	Vector<int> octree_levels = p_data["level_counts"];
	Transform to_cell_xform = p_data["to_cell_xform"];

	allocate(to_cell_xform, bounds, octree_size, octree_cells, octree_data, octree_df, octree_levels);
}

Dictionary GIProbeData::_get_data() const {
	Dictionary d;
	d["bounds"] = get_bounds();
	Vector3i otsize = get_octree_size();
	d["octree_size"] = Vector3(otsize);
	d["octree_cells"] = get_octree_cells();
	d["octree_data"] = get_data_cells();
	if (otsize != Vector3i()) {
		Ref<Image> img;
		img.instance();
		img->create(otsize.x * otsize.y, otsize.z, false, Image::FORMAT_L8, get_distance_field());
		Vector<uint8_t> df_png = img->save_png_to_buffer();
		ERR_FAIL_COND_V(df_png.size() == 0, Dictionary());
		d["octree_df_png"] = df_png;
	} else {
		d["octree_df"] = Vector<uint8_t>();
	}

void GIProbeData::set_bounds(const AABB &p_bounds) {

void GIProbeData::allocate(const Transform &p_to_cell_xform, const AABB &p_aabb, const Vector3 &p_octree_size, const Vector<uint8_t> &p_octree_cells, const Vector<uint8_t> &p_data_cells, const Vector<uint8_t> &p_distance_field, const Vector<int> &p_level_counts) {
	VS::get_singleton()->gi_probe_allocate(probe, p_to_cell_xform, p_aabb, p_octree_size, p_octree_cells, p_data_cells, p_distance_field, p_level_counts);
	bounds = p_aabb;
	to_cell_xform = p_to_cell_xform;
	octree_size = p_octree_size;
}

AABB GIProbeData::get_bounds() const {
	return bounds;
}
Vector3 GIProbeData::get_octree_size() const {
	return octree_size;
}
Vector<uint8_t> GIProbeData::get_octree_cells() const {
	return VS::get_singleton()->gi_probe_get_octree_cells(probe);
}
Vector<uint8_t> GIProbeData::get_data_cells() const {
	return VS::get_singleton()->gi_probe_get_data_cells(probe);
}
Vector<uint8_t> GIProbeData::get_distance_field() const {
	return VS::get_singleton()->gi_probe_get_distance_field(probe);
}

Vector<int> GIProbeData::get_level_counts() const {
	return VS::get_singleton()->gi_probe_get_level_counts(probe);
}

Transform GIProbeData::get_to_cell_xform() const {

	return VS::get_singleton()->gi_probe_get_to_cell_xform(probe);
}

void GIProbeData::set_dynamic_data(const PoolVector<int> &p_data) {

	VS::get_singleton()->gi_probe_set_dynamic_data(probe, p_data);
}
PoolVector<int> GIProbeData::get_dynamic_data() const {

	return VS::get_singleton()->gi_probe_get_dynamic_data(probe);
}

void GIProbeData::set_dynamic_range(int p_range) {

	VS::get_singleton()->gi_probe_set_dynamic_range(probe, p_range);
}

void GIProbeData::set_energy(float p_range) {

	VS::get_singleton()->gi_probe_set_energy(probe, p_range);
}

float GIProbeData::get_energy() const {

	return VS::get_singleton()->gi_probe_get_energy(probe);
}

void GIProbeData::set_bias(float p_range) {

	VS::get_singleton()->gi_probe_set_bias(probe, p_range);
}

float GIProbeData::get_bias() const {

	return VS::get_singleton()->gi_probe_get_bias(probe);
}

void GIProbeData::set_normal_bias(float p_range) {

	VS::get_singleton()->gi_probe_set_normal_bias(probe, p_range);
}

float GIProbeData::get_normal_bias() const {

	return VS::get_singleton()->gi_probe_get_normal_bias(probe);
}

void GIProbeData::set_propagation(float p_range) {

	VS::get_singleton()->gi_probe_set_propagation(probe, p_range);
}

float GIProbeData::get_propagation() const {

	return VS::get_singleton()->gi_probe_get_propagation(probe);
}

void GIProbeData::set_interior(bool p_enable) {

	VS::get_singleton()->gi_probe_set_interior(probe, p_enable);
}

bool GIProbeData::is_interior() const {

	return VS::get_singleton()->gi_probe_is_interior(probe);
}

bool GIProbeData::is_compressed() const {

	return VS::get_singleton()->gi_probe_is_compressed(probe);
}

void GIProbeData::set_compress(bool p_enable) {

	VS::get_singleton()->gi_probe_set_compress(probe, p_enable);
}

int GIProbeData::get_dynamic_range() const {

	return VS::get_singleton()->gi_probe_get_dynamic_range(probe);
}

RID GIProbeData::get_rid() const {

	return probe;
}

void GIProbeData::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_bounds", "bounds"), &GIProbeData::set_bounds);
	ClassDB::bind_method(D_METHOD("get_bounds"), &GIProbeData::get_bounds);

	ClassDB::bind_method(D_METHOD("set_cell_size", "cell_size"), &GIProbeData::set_cell_size);
	ClassDB::bind_method(D_METHOD("get_cell_size"), &GIProbeData::get_cell_size);

	ClassDB::bind_method(D_METHOD("set_to_cell_xform", "to_cell_xform"), &GIProbeData::set_to_cell_xform);
	ClassDB::bind_method(D_METHOD("get_to_cell_xform"), &GIProbeData::get_to_cell_xform);

	ClassDB::bind_method(D_METHOD("set_dynamic_data", "dynamic_data"), &GIProbeData::set_dynamic_data);
	ClassDB::bind_method(D_METHOD("get_dynamic_data"), &GIProbeData::get_dynamic_data);

	ClassDB::bind_method(D_METHOD("set_dynamic_range", "dynamic_range"), &GIProbeData::set_dynamic_range);
	ClassDB::bind_method(D_METHOD("get_dynamic_range"), &GIProbeData::get_dynamic_range);

	ClassDB::bind_method(D_METHOD("set_energy", "energy"), &GIProbeData::set_energy);
	ClassDB::bind_method(D_METHOD("get_energy"), &GIProbeData::get_energy);

	ClassDB::bind_method(D_METHOD("set_bias", "bias"), &GIProbeData::set_bias);
	ClassDB::bind_method(D_METHOD("get_bias"), &GIProbeData::get_bias);

	ClassDB::bind_method(D_METHOD("set_normal_bias", "bias"), &GIProbeData::set_normal_bias);
	ClassDB::bind_method(D_METHOD("get_normal_bias"), &GIProbeData::get_normal_bias);

	ClassDB::bind_method(D_METHOD("set_propagation", "propagation"), &GIProbeData::set_propagation);
	ClassDB::bind_method(D_METHOD("get_propagation"), &GIProbeData::get_propagation);

	ClassDB::bind_method(D_METHOD("set_interior", "interior"), &GIProbeData::set_interior);
	ClassDB::bind_method(D_METHOD("is_interior"), &GIProbeData::is_interior);

	ClassDB::bind_method(D_METHOD("set_compress", "compress"), &GIProbeData::set_compress);
	ClassDB::bind_method(D_METHOD("is_compressed"), &GIProbeData::is_compressed);

	ADD_PROPERTY(PropertyInfo(Variant::AABB, "bounds", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), "set_bounds", "get_bounds");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "cell_size", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), "set_cell_size", "get_cell_size");
	ADD_PROPERTY(PropertyInfo(Variant::TRANSFORM, "to_cell_xform", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), "set_to_cell_xform", "get_to_cell_xform");

	ADD_PROPERTY(PropertyInfo(Variant::POOL_INT_ARRAY, "dynamic_data", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), "set_dynamic_data", "get_dynamic_data");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "dynamic_range", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), "set_dynamic_range", "get_dynamic_range");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "energy", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), "set_energy", "get_energy");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "bias", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), "set_bias", "get_bias");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "normal_bias", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), "set_normal_bias", "get_normal_bias");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "propagation", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), "set_propagation", "get_propagation");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "interior", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), "set_interior", "is_interior");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "compress", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), "set_compress", "is_compressed");
}

GIProbeData::GIProbeData() {

	probe = VS::get_singleton()->gi_probe_create();
}

GIProbeData::~GIProbeData() {

	VS::get_singleton()->free(probe);
}

//////////////////////
//////////////////////

void GIProbe::set_probe_data(const Ref<GIProbeData> &p_data) {

	if (p_data.is_valid()) {
		VS::get_singleton()->instance_set_base(get_instance(), p_data->get_rid());
	} else {
		VS::get_singleton()->instance_set_base(get_instance(), RID());
	}

	probe_data = p_data;
}

Ref<GIProbeData> GIProbe::get_probe_data() const {

	return probe_data;
}

void GIProbe::set_subdiv(Subdiv p_subdiv) {

	ERR_FAIL_INDEX(p_subdiv, SUBDIV_MAX);
	subdiv = p_subdiv;
	update_gizmo();
}

GIProbe::Subdiv GIProbe::get_subdiv() const {

	return subdiv;
}

void GIProbe::set_extents(const Vector3 &p_extents) {

	extents = p_extents;
	update_gizmo();
	_change_notify("extents");
}

Vector3 GIProbe::get_extents() const {

	return extents;
}

void GIProbe::set_dynamic_range(int p_dynamic_range) {

	dynamic_range = p_dynamic_range;
}
int GIProbe::get_dynamic_range() const {

	return dynamic_range;
}

void GIProbe::set_energy(float p_energy) {

	energy = p_energy;
	if (probe_data.is_valid()) {
		probe_data->set_energy(energy);
	}
}
float GIProbe::get_energy() const {

	return energy;
}

void GIProbe::set_bias(float p_bias) {

	bias = p_bias;
	if (probe_data.is_valid()) {
		probe_data->set_bias(bias);
	}
}
float GIProbe::get_bias() const {

	return bias;
}

void GIProbe::set_normal_bias(float p_normal_bias) {

	normal_bias = p_normal_bias;
	if (probe_data.is_valid()) {
		probe_data->set_normal_bias(normal_bias);
	}
}
float GIProbe::get_normal_bias() const {

	return normal_bias;
}

void GIProbe::set_propagation(float p_propagation) {

	propagation = p_propagation;
	if (probe_data.is_valid()) {
		probe_data->set_propagation(propagation);
	}
}
float GIProbe::get_propagation() const {

	return propagation;
}

void GIProbe::set_interior(bool p_enable) {

	interior = p_enable;
	if (probe_data.is_valid()) {
		probe_data->set_interior(p_enable);
	}
}

bool GIProbe::is_interior() const {

	return interior;
}

void GIProbe::set_compress(bool p_enable) {

	compress = p_enable;
	if (probe_data.is_valid()) {
		probe_data->set_compress(p_enable);
	}
}

bool GIProbe::is_compressed() const {

	return compress;
}

void GIProbe::_find_meshes(Node *p_at_node, List<PlotMesh> &plot_meshes) {

	MeshInstance *mi = Object::cast_to<MeshInstance>(p_at_node);
	if (mi && mi->get_flag(GeometryInstance::FLAG_USE_BAKED_LIGHT) && mi->is_visible_in_tree()) {
		Ref<Mesh> mesh = mi->get_mesh();
		if (mesh.is_valid()) {

			AABB aabb = mesh->get_aabb();

			Transform xf = get_global_transform().affine_inverse() * mi->get_global_transform();

			if (AABB(-extents, extents * 2).intersects(xf.xform(aabb))) {
				PlotMesh pm;
				pm.local_xform = xf;
				pm.mesh = mesh;
				for (int i = 0; i < mesh->get_surface_count(); i++) {
					pm.instance_materials.push_back(mi->get_surface_material(i));
				}
				pm.override_material = mi->get_material_override();
				plot_meshes.push_back(pm);
			}
		}
	}

	Spatial *s = Object::cast_to<Spatial>(p_at_node);
	if (s) {

		if (s->is_visible_in_tree()) {

			Array meshes = p_at_node->call("get_meshes");
			for (int i = 0; i < meshes.size(); i += 2) {

				Transform mxf = meshes[i];
				Ref<Mesh> mesh = meshes[i + 1];
				if (!mesh.is_valid())
					continue;

				AABB aabb = mesh->get_aabb();

				Transform xf = get_global_transform().affine_inverse() * (s->get_global_transform() * mxf);

				if (AABB(-extents, extents * 2).intersects(xf.xform(aabb))) {
					PlotMesh pm;
					pm.local_xform = xf;
					pm.mesh = mesh;
					plot_meshes.push_back(pm);
				}
			}
		}
	}

	for (int i = 0; i < p_at_node->get_child_count(); i++) {
		Node *child = p_at_node->get_child(i);
		_find_meshes(child, plot_meshes);
	}
}

GIProbe::BakeBeginFunc GIProbe::bake_begin_function = NULL;
GIProbe::BakeStepFunc GIProbe::bake_step_function = NULL;
GIProbe::BakeEndFunc GIProbe::bake_end_function = NULL;

void GIProbe::bake(Node *p_from_node, bool p_create_visual_debug) {

	static const int subdiv_value[SUBDIV_MAX] = { 7, 8, 9, 10 };

	VoxelLightBaker baker;

	baker.begin_bake(subdiv_value[subdiv], AABB(-extents, extents * 2.0));

	List<PlotMesh> mesh_list;

	_find_meshes(p_from_node ? p_from_node : get_parent(), mesh_list);

	if (bake_begin_function) {
		bake_begin_function(mesh_list.size() + 1);
	}

	int pmc = 0;

	for (List<PlotMesh>::Element *E = mesh_list.front(); E; E = E->next()) {

		if (bake_step_function) {
			bake_step_function(pmc, RTR("Plotting Meshes") + " " + itos(pmc) + "/" + itos(mesh_list.size()));
		}

		pmc++;

		baker.plot_mesh(E->get().local_xform, E->get().mesh, E->get().instance_materials, E->get().override_material);
	}
	if (bake_step_function) {
		bake_step_function(pmc++, RTR("Finishing Plot"));
	}

	baker.end_bake();

	//create the data for visual server

	PoolVector<int> data = baker.create_gi_probe_data();

	if (p_create_visual_debug) {
		MultiMeshInstance *mmi = memnew(MultiMeshInstance);
		mmi->set_multimesh(baker.create_debug_multimesh());
		add_child(mmi);
#ifdef TOOLS_ENABLED
		if (get_tree()->get_edited_scene_root() == this) {
			mmi->set_owner(this);
		} else {
			mmi->set_owner(get_owner());
		}
#else
		mmi->set_owner(get_owner());
#endif

	} else {

		Ref<GIProbeData> probe_data = get_probe_data();

		if (probe_data.is_null())
			probe_data.instance();

		if (bake_step_function) {
			bake_step_function(pmc++, RTR("Generating Distance Field"));
		}

		Vector<uint8_t> df = baker.get_sdf_3d_image();

		probe_data->allocate(baker.get_to_cell_space_xform(), AABB(-extents, extents * 2.0), baker.get_giprobe_octree_size(), baker.get_giprobe_octree_cells(), baker.get_giprobe_data_cells(), df, baker.get_giprobe_level_cell_count());

		set_probe_data(probe_data);
	}

	if (bake_end_function) {
		bake_end_function();
	}
}

void GIProbe::_debug_bake() {

	bake(NULL, true);
}

AABB GIProbe::get_aabb() const {

	return AABB(-extents, extents * 2);
}

Vector<Face3> GIProbe::get_faces(uint32_t p_usage_flags) const {

	return Vector<Face3>();
}

String GIProbe::get_configuration_warning() const {

	if (OS::get_singleton()->get_current_video_driver() == OS::VIDEO_DRIVER_GLES2) {
		return TTR("GIProbes are not supported by the GLES2 video driver.\nUse a BakedLightmap instead.");
	}
	return String();
}

void GIProbe::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_probe_data", "data"), &GIProbe::set_probe_data);
	ClassDB::bind_method(D_METHOD("get_probe_data"), &GIProbe::get_probe_data);

	ClassDB::bind_method(D_METHOD("set_subdiv", "subdiv"), &GIProbe::set_subdiv);
	ClassDB::bind_method(D_METHOD("get_subdiv"), &GIProbe::get_subdiv);

	ClassDB::bind_method(D_METHOD("set_extents", "extents"), &GIProbe::set_extents);
	ClassDB::bind_method(D_METHOD("get_extents"), &GIProbe::get_extents);

	ClassDB::bind_method(D_METHOD("set_dynamic_range", "max"), &GIProbe::set_dynamic_range);
	ClassDB::bind_method(D_METHOD("get_dynamic_range"), &GIProbe::get_dynamic_range);

	ClassDB::bind_method(D_METHOD("set_energy", "max"), &GIProbe::set_energy);
	ClassDB::bind_method(D_METHOD("get_energy"), &GIProbe::get_energy);

	ClassDB::bind_method(D_METHOD("set_bias", "max"), &GIProbe::set_bias);
	ClassDB::bind_method(D_METHOD("get_bias"), &GIProbe::get_bias);

	ClassDB::bind_method(D_METHOD("set_normal_bias", "max"), &GIProbe::set_normal_bias);
	ClassDB::bind_method(D_METHOD("get_normal_bias"), &GIProbe::get_normal_bias);

	ClassDB::bind_method(D_METHOD("set_propagation", "max"), &GIProbe::set_propagation);
	ClassDB::bind_method(D_METHOD("get_propagation"), &GIProbe::get_propagation);

	ClassDB::bind_method(D_METHOD("set_interior", "enable"), &GIProbe::set_interior);
	ClassDB::bind_method(D_METHOD("is_interior"), &GIProbe::is_interior);

	ClassDB::bind_method(D_METHOD("set_compress", "enable"), &GIProbe::set_compress);
	ClassDB::bind_method(D_METHOD("is_compressed"), &GIProbe::is_compressed);

	ClassDB::bind_method(D_METHOD("bake", "from_node", "create_visual_debug"), &GIProbe::bake, DEFVAL(Variant()), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("debug_bake"), &GIProbe::_debug_bake);
	ClassDB::set_method_flags(get_class_static(), _scs_create("debug_bake"), METHOD_FLAGS_DEFAULT | METHOD_FLAG_EDITOR);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "subdiv", PROPERTY_HINT_ENUM, "64,128,256,512"), "set_subdiv", "get_subdiv");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "extents"), "set_extents", "get_extents");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "dynamic_range", PROPERTY_HINT_RANGE, "1,16,1"), "set_dynamic_range", "get_dynamic_range");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "energy", PROPERTY_HINT_RANGE, "0,16,0.01,or_greater"), "set_energy", "get_energy");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "propagation", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_propagation", "get_propagation");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "bias", PROPERTY_HINT_RANGE, "0,4,0.001"), "set_bias", "get_bias");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "normal_bias", PROPERTY_HINT_RANGE, "0,4,0.001"), "set_normal_bias", "get_normal_bias");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "interior"), "set_interior", "is_interior");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "compress"), "set_compress", "is_compressed");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "data", PROPERTY_HINT_RESOURCE_TYPE, "GIProbeData", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_DO_NOT_SHARE_ON_DUPLICATE), "set_probe_data", "get_probe_data");

	BIND_ENUM_CONSTANT(SUBDIV_64);
	BIND_ENUM_CONSTANT(SUBDIV_128);
	BIND_ENUM_CONSTANT(SUBDIV_256);
	BIND_ENUM_CONSTANT(SUBDIV_512);
	BIND_ENUM_CONSTANT(SUBDIV_MAX);
}

GIProbe::GIProbe() {

	subdiv = SUBDIV_128;
	dynamic_range = 4;
	energy = 1.0;
	bias = 1.5;
	normal_bias = 0.0;
	propagation = 0.7;
	extents = Vector3(10, 10, 10);
	interior = false;
	compress = false;

	gi_probe = VS::get_singleton()->gi_probe_create();
	set_disable_scale(true);
}

GIProbe::~GIProbe() {
	VS::get_singleton()->free(gi_probe);
}

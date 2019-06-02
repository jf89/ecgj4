#pragma once

#define MAX_SELECTED_ENTITIES 32

#define UI_STATES \
	/* start ui state list */ \
	UI_STATE(DEFAULT) \
	UI_STATE(DRAGGING_SELECTION) \
	UI_STATE(DRAGGING_MAP) \
	UI_STATE(MAN_SELECTED) \
	UI_STATE(FORT_SELECTED) \
	UI_STATE(MULTIPLE_SELECTED) \
	/* end ui state list */

enum ui_state_type {
	#define UI_STATE(name) UI_STATE_ ## name,
	UI_STATES
	#undef UI_STATE
};

char *ui_state_names[] = {
	#define UI_STATE(name) #name,
	UI_STATES
	#undef UI_STATE
};

struct ui_state {
	enum ui_state_type state;
	v2 map_center;
	f32 zoom;
	u32 mouse_over_entity;
	u32 num_selected_entities;
	u32 selected_entities[MAX_SELECTED_ENTITIES];
	union {
		struct {
			enum ui_state_type prev_state;
		} dragging_map;
		struct {
			v2 start_pos;
			v2 end_pos;
		} selecting;
	};
};

#define PROCESS_INPUT \
	bool process_input( \
		struct ui_state *ui_state, \
		struct game_state *game_state, \
		struct anim_state *anim_state, \
		struct dijkstra_maps *dijkstra_maps)
PROCESS_INPUT;

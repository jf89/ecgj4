#pragma once

struct dijkstra_map {
	enum {
		OBJECTIVE_PURSUE,
		OBJECTIVE_FLEE,
	} objective;
	v2_u8 goal;
	f32 weight[MAX_MAP_SIZE];
};

#define MAX_DIJKSTRA_MAPS 32

struct dijkstra_maps {
	u32 num_maps;
	u32 reference_counts[MAX_DIJKSTRA_MAPS];
	struct dijkstra_map maps[MAX_DIJKSTRA_MAPS];
};

#define PATH_CALCULATE \
	void path_calculate( \
		struct dijkstra_map *path_map, \
		struct game_state *game_state)
PATH_CALCULATE;

#define GET_FREE_DIJKSTRA_MAP u32 get_free_dijkstra_map(struct dijkstra_maps *maps)
GET_FREE_DIJKSTRA_MAP;

#include "common_physics.hpp"

bool keymap[512] = {};

void CommonPhysics::playerMovementHandler(foregroundMotion& player_motion, float elapsed_ms) {
	float step_seconds = elapsed_ms / 1000.f;
	for (int i = 0; i < sizeof(keymap) / sizeof(keymap[0]); i++) {
		if (keymap[i]) {
			switch (i) {
			case GLFW_KEY_W:
				player_motion.position.y += -275.f * step_seconds;
				break;
			case GLFW_KEY_S:
				player_motion.position.y += 275.f * step_seconds;
				break;
			case GLFW_KEY_A:
				player_motion.position.x += -275.f * step_seconds;
				break;
			case GLFW_KEY_D:
				player_motion.position.x += 275.f * step_seconds;
				break;
			}
		}
		// Bounds the player in the game screen
		player_motion.position.x = max(50.f, min((float)window_width_px - 50, player_motion.position.x));
		player_motion.position.y = max(50.f, min((float)window_height_px - 50, player_motion.position.y));
	}
}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You can
// surely implement a more accurate detection
bool CommonPhysics::checkCircleCollision(Entity& e1, Entity& e2)
{
	vec2 pos1 = get_position(e1);
	vec2 pos2 = get_position(e2);
	vec2 dp = pos1 - pos2;
	float dist_squared = dot(dp,dp);
	const vec2 other_bonding_box = get_bounding_box(e1) / 2.f;
	const float other_r_squared = dot(other_bonding_box, other_bonding_box);
	const vec2 my_bonding_box = get_bounding_box(e2) / 2.f;
	const float my_r_squared = dot(my_bonding_box, my_bonding_box);
	const float r_squared = max(other_r_squared, my_r_squared);
	if (dist_squared < r_squared)
		return true;
	return false;
}

// Helper to calculate the relative corners of a bounding box according to a motion.position
vec4 CommonPhysics::get_bbox_corners(vec2& bbox, vec2& position) {
	float x_min = position.x - bbox.x / 2.f;
    float x_max = position.x + bbox.x / 2.f;
    float y_min = position.y - bbox.y / 2.f;
    float y_max = position.y + bbox.y / 2.f;

	vec4 corners = { x_min, x_max, y_min, y_max };
	return corners;
}

// A slightly more precise collision check that uses rectangular bounding boxes and checks
// for horizontal and vertical overlap a.k.a. AABB check
bool CommonPhysics::checkBoxCollision(Entity& e1, Entity& e2)
{
	// Get bounding boxes and positions for both entities
    vec2 bbox1 = get_bounding_box(e1);
    vec2 bbox2 = get_bounding_box(e2);

	vec2 position1 = get_position(e1);
	vec2 position2 = get_position(e2);

    // Calculate the corners of the bounding box for motion1
	vec4 bbox1_corners = get_bbox_corners(bbox1, position1);
    float x1_min = bbox1_corners[0];
    float x1_max = bbox1_corners[1];
    float y1_min = bbox1_corners[2];
    float y1_max = bbox1_corners[3];

    // Calculate the corners of the bounding box for motion2
	vec4 bbox2_corners = get_bbox_corners(bbox2, position2);
    float x2_min = bbox2_corners[0];
    float x2_max = bbox2_corners[1];
    float y2_min = bbox2_corners[2];
    float y2_max = bbox2_corners[3];

    // Check for overlaps
    bool overlap_x = x1_min < x2_max && x2_min < x1_max;
    bool overlap_y = y1_min < y2_max && y2_min < y1_max;

    return overlap_x && overlap_y;
}

// Collision helper to check if a point (from a polygon mesh) is inside a rectangular bounding box
bool pointInsideAABB(vec3& point, vec2& bboxMin, vec2& bboxMax) {
    return (point.x >= bboxMin.x && point.x <= bboxMax.x) && (point.y >= bboxMin.y && point.y <= bboxMax.y);
}

// Helper to get transformed vertices of a mesh according to the entity's position
std::vector<ColoredVertex> transformVertices(Entity& meshEntity) {
	Mesh* mesh = registry.meshPtrs.get(meshEntity);
	// Assuming mesh collision entities are in the foreground 
	foregroundMotion& meshEntity_motion = registry.foregroundMotions.get(meshEntity); 

	std::vector<ColoredVertex> transformedVertices;

	for (const auto& vertex : mesh->vertices) {
		vec3 pos = { vertex.position.x, vertex.position.y, 1.0f };

		Transform transform;
		transform.translate(meshEntity_motion.position);
		transform.rotate(meshEntity_motion.angle);
		transform.scale(meshEntity_motion.scale);
		
		// Apply transformation matrix to original vertex position
		vec3 transformedPos = transform.mat * pos;

		// Preserve the original color of the vertex and convert to ColoredVertex
        ColoredVertex transformedVertex;
		transformedVertex.position = {transformedPos.x, transformedPos.y, transformedPos.z};
        transformedVertex.color = vertex.color; 

		transformedVertices.push_back(transformedVertex);
	}

	return transformedVertices;
}

// Helper function to calculate cross product
float cross(vec2 a, vec2 b) {
    return a.x * b.y - a.y * b.x;
}

// Reference: https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
// Helper function to check if two line segments intersect
// A line segment is defined by 2 end-points: [p1, p2] or [q1, q2]
bool lineIntersect(vec2 p1, vec2 p2, vec2 q1, vec2 q2) {
	// Compute direction vector of lines
    vec2 r = p2 - p1;
    vec2 s = q2 - q1;

	// Compute cross products 
    float rxs = cross(r, s);
    float qpxr = cross((q1 - p1), r);

	// Cross product (r x s) gives us the area of the parallelogram formed by r and s
	// If r x s = 0, the lines are parallel
	// Cross product (q1 - p1) x r tells us how q1 is oriented relative to line [p1, p2]
    // If both cross products are zero, the lines are collinear.
    if (rxs == 0 && qpxr == 0) {
        // Collinear lines can overlap or not.
        // The additional check for pointInsideAABB should resolve this
        return false; 
    }

    // If r x s = 0 and (q1 - p1) x r != 0, then the lines are parallel and non-intersecting.
    if (rxs == 0 && qpxr != 0) return false;

	// Any point of the line segments can be represented parametrically:
	// i.e. Any point on [p1, p2] = p1 + t(p2 - p1) where t varies from [0,1]
	//      Any point on [q1, q2] = q1 + u(q2 - q1) where q varies from [0,1]
	// Solving the parametric equation for t/u, an intersection occurs when both 
    // t and u values are in between [0, 1]
    float t = cross((q1 - p1), s) / rxs;
    float u = cross((q1 - p1), r) / rxs;

    // If r x s != 0 (not parallel) and 0 <= t <= 1 and 0 <= u <= 1, 
	// the line segments meet at the point p + t r = q + u s.
    return (t >= 0 && t <= 1 && u >= 0 && u <= 1);
}

// Helper function to check if edges of the mesh and AABB intersect
bool mesh_AABB_edge_intersection(const std::vector<ColoredVertex>& meshVertices, const std::vector<uint16_t>& vertexIndices, vec2& boxMin, vec2& boxMax) {
    // Define AABB edges; each edge is a pair of vec2
    std::vector<std::pair<vec2, vec2>> aabbEdges = {
        {{boxMin.x, boxMin.y}, {boxMax.x, boxMin.y}}, // Bottom edge
        {{boxMax.x, boxMin.y}, {boxMax.x, boxMax.y}}, // Right edge
        {{boxMax.x, boxMax.y}, {boxMin.x, boxMax.y}}, // Top edge
        {{boxMin.x, boxMax.y}, {boxMin.x, boxMin.y}}  // Left edge
    };

    // Loop through all edges of the mesh
	// ASSUMPTION: edges connections correspond to 'faces' of the polygon, accessible through vertexIndices
    for (size_t i = 0; i < vertexIndices.size(); i+=3) {
        std::vector<std::pair<vec2, vec2>> meshEdges = {
            {    meshVertices[vertexIndices[i]].position,	meshVertices[vertexIndices[i + 1]].position},
            {meshVertices[vertexIndices[i + 1]].position, 	meshVertices[vertexIndices[i + 2]].position},
            {meshVertices[vertexIndices[i + 2]].position, 	meshVertices[vertexIndices[i]].position}
        };
        
        for (const auto& edge : meshEdges) {
            for (const auto& aabbEdge : aabbEdges) {
                if (lineIntersect(edge.first, edge.second, aabbEdge.first, aabbEdge.second)) {
                    return true; // Intersection found
                }
            }
        }
    }

    return false; 
}

// Checks if there is a collision between an entity with a polygon mesh and a regular rectangular entity
// ASSUMPTION: all entities that can collide are in the foreground
bool CommonPhysics::checkMeshCollision(Entity& meshEntity, Entity& otherEntity) {
	// Transform mesh vertices to game coordinate plane
	std::vector<ColoredVertex> transformedVertices = transformVertices(meshEntity);

	// Get bounding box and max/min corners for other entity
	foregroundMotion& otherEntity_motion = registry.foregroundMotions.get(otherEntity);
	vec2 otherEntity_bbox = get_bounding_box(otherEntity);
	vec4 otherEntity_bbox_corners = get_bbox_corners(otherEntity_bbox, otherEntity_motion.position);

	float x_min = otherEntity_bbox_corners[0];
    float x_max = otherEntity_bbox_corners[1];
    float y_min = otherEntity_bbox_corners[2];
    float y_max = otherEntity_bbox_corners[3];
	
	// scaling down the bbox slightly
	vec2 boxMin = vec2{ x_min, y_min } * BBOX_SCALE;
	vec2 boxMax = vec2{ x_max, y_max } / BBOX_SCALE;

	// Narrow phase collision check
	// A collision would pass the first or both conditions 

	// 1. Are any vertices inside an AABB? 
	for (ColoredVertex& vertex : transformedVertices) {
		if (pointInsideAABB(vertex.position, boxMin, boxMax)) {
			return true;
		}
	}
	
	// 2. If not, are any edges intersection with an edge of an AABB?
	Mesh* mesh = registry.meshPtrs.get(meshEntity);
	std::vector<uint16_t>& vertexIndices = mesh->vertex_indices;
	if (mesh_AABB_edge_intersection(transformedVertices, vertexIndices, boxMin, boxMax)) {
		return true;
	}
	
	return false;
}

// General collision detection function that incorporates both broad and narrow phases
// ASSUMPTION: First parameter is entity with a mesh. This may change in future iterations. 
bool CommonPhysics::isColliding(Entity& meshEntity, Entity& e2) {
	// Broad phase AABB collision check
	if (!checkBoxCollision(meshEntity, e2)) {
		return false;
	}

	// Narrow phase collision check
	if (registry.meshPtrs.has(meshEntity)) {
		return checkMeshCollision(meshEntity, e2);
	}

	// If AABB overlaps but there is no mesh, then we have a simple AABB collision
	return true;
}

float CommonPhysics::lerp(float property, float end_x, float t)
{
	return round(property * (1 - t) + end_x * t);
}

// https://en.wikipedia.org/wiki/B%C3%A9zier_curve#:~:text=A%20B%C3%A9zier%20curve%20(%2F%CB%88b,by%20means%20of%20a%20formula.
vec2 CommonPhysics::getBezierPosition(BezierCurve& b) {
	vec2 result = vec2(0.0f, 0.0f);
	int n = b.points.size() - 1;

	for (int i = 0; i <= n; i++) {
		float var1 = choose(n, i);
		float var2 = pow((1 - b.t), n - i);
		float var3 = pow(b.t, i);
		result += var1 * var2 * var3 * b.points[i];
	}

	return result;
}

// Returns the local bounding coordinates scaled by the current size of the entity
vec2 CommonPhysics::get_bounding_box(Entity& e)
{
	vec2 scale = get_scale(e);
	// abs is to avoid negative scale due to the facing direction.
	return { abs(scale.x), abs(scale.y) };
}

void CommonPhysics::setAngleToPlayer(foregroundMotion& player_motion, foregroundMotion& enemy_motion) {
	enemy_motion.angle = atan2((player_motion.position.y - enemy_motion.position.y), (enemy_motion.position.x - player_motion.position.x));

	// for debugging
	//float distance = sqrtf(pow((player_motion.position.y - enemy_motion.position.y), 2) + pow((enemy_motion.position.x - player_motion.position.x), 2));
	//printf("angle: %f, distance: %f\n", enemy_motion.angle, distance);
}

// probably want a component that removes the following two functions.
vec2 CommonPhysics::get_position(Entity& e) {
	if (registry.foregroundMotions.has(e)) {
		foregroundMotion& motion = registry.foregroundMotions.get(e);
		return motion.position;
	}
	else if (registry.backgroundMotions.has(e)) {
		backgroundMotions& staticObject = registry.backgroundMotions.get(e);
		return staticObject.position;
	}
	else {
		assert(false && "Entity does not have a position.");
		return vec2(0, 0);
	}
}

vec2 CommonPhysics::get_scale(Entity& e) {
	if (registry.foregroundMotions.has(e)) {
		foregroundMotion& motion = registry.foregroundMotions.get(e);
		return motion.scale;
	}
	else if (registry.backgroundMotions.has(e)) {
		backgroundMotions& staticObject = registry.backgroundMotions.get(e);
		return staticObject.scale;
	}
	else {
		assert(false && "Entity does not have a scale.");
		return vec2(0, 0);
	}
	
}

BBox CommonPhysics::getBBoxBounds(Entity& e) {
	vec2 pos = get_position(e);
	vec2 scale = get_scale(e);
	scale.y *= -1;

	BBox box;
	box.top = pos.y - (scale.y / 2);
	box.bottom = pos.y + (scale.y / 2);
	box.left = pos.x - (scale.x / 2);
	box.right = pos.x + (scale.x / 2);

	return box;
}


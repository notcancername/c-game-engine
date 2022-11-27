#include "engine.h"
/*
typedef struct GraphEdge {
	int vertexIDs[2];
	float weight;
}

typedef struct GraphVertex {
	size_t edgeCount;
	GraphEdge* edges; //dynamically allocated using the DynArray util
}

typedef struct Graph {
	size_t vertexCount;
	GraphVertex* vertices;
}
*/

//vertexID 0 is reserved for a "NULL node", a node that "doesnt exist", to allow connections from vertices to nothing
//this is more a convenience feature, to be used later for the maze generation algorithm these graph functions will use




Graph* createGraph(GraphVertex* vertices, size_t vertexCount) {
	if (vertices == NULL) {
		//vertex count is assumed to be 0
		vertexCount = 0;
	}
	Graph* intGraph = gmalloc(sizeof(*intGraph));
	intGraph->vertexCount = vertexCount;
	intGraph->vertices = createDynArray(sizeof(GraphVertex));
	
	if (vertexCount != 0) {
		for (size_t i = 0; i < vertexCount; i++) {
			//copy vertices from GraphVertex* into the array
			appendElement(intGraph->vertices, &vertices[i]);
		}
	}
	return intGraph;
} 

int addVertex(Graph* intGraph, GraphEdge* edges, size_t edgeCount) {
	if (intGraph == NULL) {
		return -1;
	}

	if (edges == NULL) {
		appendElement(intGraph->vertices, &(GraphVertex){0, NULL});
		return 0;
	}

	dynArray* intEdges = createDynArray(sizeof(GraphEdge));

	intGraph->vertexCount++;
	for (size_t i = 0; i < edgeCount; i++) {
		appendElement(intEdges, &edges[i]);
	}

	appendElement(intGraph->vertices, &(GraphVertex){edgeCount, intEdges});	
	return 0;
}

//hm. dynArray's ids will shift after the vertex is removed, completely destroying the graph.
//it would be better here to insert a "null" vertex, so that the graph could write over this vertex if needed
//idk
//this is a bug to fix when i need to

int removeVertex(Graph* intGraph, int vertexID) {
	logtofile("removeVertex has a bug in it that i dont wanna fix right now, use this later!", ERR, "Graph");
	return -1;
	if (intGraph == NULL || vertexID == 0) {
		return -1;
	}

	GraphVertex* intVertex = (GraphVertex*)getElement(intGraph->vertices, vertexID - 1);
	if (intVertex == NULL) {
		return -1;
	}

	if (intVertex->edges != NULL) {
		deleteArray(intVertex->edges);
	}	
	intGraph->vertexCount--;
	removeElement(intGraph->vertices, vertexID - 1);

	return 0;
}

int addEdge(Graph* intGraph, GraphEdge edge, int vertexID) {
	if (intGraph == NULL || vertexID == 0) {
		return -1;
	}

	GraphVertex* intVertex = (GraphVertex*)getElement(intGraph->vertices, vertexID - 1);
	if (intVertex == NULL) {
		return -1;
	}

	intVertex->edgeCount++;
	//prevent "edge fragmentation" memory usage bug

	for (size_t i = 0; i < intVertex->edgeCount; i++) {
		GraphEdge* intEdge = (GraphEdge*)getElement(intVertex->edges, i);
		if (intEdge->vertexIDs[0] == 0 && intEdge->vertexIDs[1] == 0) {
			updateElement(intVertex->edges, i, &edge);
			return 0;
		}
	}

	appendElement(intVertex->edges, &edge);
	return 0;
}

int removeEdge(Graph* intGraph, int vertexID, int edgeID) {
	if (intGraph == NULL || vertexID == 0) {
		return -1;
	}

	GraphVertex* intVertex = (GraphVertex*)getElement(intGraph->vertices, vertexID - 1);
	if (intVertex == NULL) {
		return -1;
	}

	intVertex->edgeCount--;
	updateElement(intVertex->edges, edgeID, &(GraphEdge){.vertexIDs = {0,0}, 0.0}); //makes the edge a loop at 0,0 with a weight of 0. this can be used as a null edge detection system
	return 0;	
}

int deleteGraph(Graph* intGraph) {
	if (intGraph == NULL) {
		return -1;
	}

	logtofile("This relies on removeVertex, and so also doesnt work yet. Come back later!", ERR, "Graph");
	return -1;
	for (size_t i = 1; i < intGraph->vertexCount + 1; i++) {
		removeVertex(intGraph, i);
	}

	gfree(intGraph);

	return 0;
}

int drawGraph(Graph* intGraph, int x, int y) {
	UNUSED(intGraph);
	if (intGraph == NULL) {
		return -1;
	}
	if (intGraph->vertexCount > (size_t)x*y) {
		//special case the last y value

	}
	for (int i = 0; i < y; i++) {
		for (int j = 0; j < x; j++) {
			drawCircle(VECCNT(j*96 - 256, i*96 - 256), 16, (RGBA){.rgba=0xFF543893});
			//draw the lines we need for this
			GraphVertex* intVertex = (GraphVertex*)getElement(intGraph->vertices, i*y + j);
			for (size_t k = 0; k < intVertex->edgeCount; k++) {
				GraphEdge* intEdge = (GraphEdge*)getElement(intVertex->edges, k);
				if (intEdge->vertexIDs[0] == 0 && intEdge->vertexIDs[1] == 0) {
					continue;
				}

				int idX = intEdge->vertexIDs[1] % y;
				int idY = (intEdge->vertexIDs[1] - idX)/y;
				drawLine(VECCNT(j*96 - 256, i*96 - 256), VECCNT(idX*96 - 256, idY*96 - 256), (RGBA){.rgba=0xFF854389}, 1);
			}
		}
	}
	return 0;
}

Graph* nearestNeighbour(Graph* intGraph) { //generates a tree from the graph, starting from a random vertex
	if (intGraph == NULL) {
		return -1;
	}
	Graph* stGraph = createGraph(NULL, 0);

	int currentNode = rand() % intGraph->vertexCount;
	for (size_t i = 0; i < intGraph->vertexCount - 1; i++) {
		int currentEdge = 0;

	}
}

//horrible bug found!
/*

the entire graph data structure does not allow for backwards traversal through all nodes
because im fucking stupid and wrote addEdge wrong.
the entire graph system should be rewritten but for now my brain is VERY tired
*/
#ifndef PHYS_CELL_H
#define PHYS_CELL_H
#include <vector>

#include "physics/softbody/CellVertex.h"
namespace aly {
	namespace softbody {

		class LatticeLocation;

		// Fundamentally, a cell is a rendering concept. It means a volume of space mapped from
		//  object space to world space. These are the cubes in the lattice.
		// One kind of cell has particles at the vertices; this is one way of mapping the space.
		//  Another kind has a particle at the center of each cell, and projects out from that
		//  particle based on its rotation. This is the kind we use.
		// Cells must keep track of other cells sharing their vertices, and if they are attached
		//  to those cells, they must agree with them on where the vertices are, so that everything
		//  is nice and smooth. This is handled by the CellVertex class.
		class Cell
		{
		public:
			LatticeLocation *center;
			CellVertex vertices[8];

			std::vector<Cell*> connectedCells;

			Cell();
			void initialize();
			void initialize2();
			void handleVertexSharerFracture(); // Update in response to a fracture occuring in one of the cells that share this vertex
			void updateVertexPositions();
		};
		typedef std::shared_ptr<Cell> CellPtr;
	}
}
#endif

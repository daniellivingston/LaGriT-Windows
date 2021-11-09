#pragma once

#include <iostream>
#include <exception>

namespace LaGriT {
    struct LGException : public std::exception {
        const char * what () const throw() {
            return "LaGriT fatal error";
        }
    };


    // Core
    int initialize();

    // Mesh
    int selectMesh();
    int createMesh();
    int getMeshName();
    int setMeshName();
    int getMeshProps();
    int getMeshQuality();

    /// Mesh nodes
    int getNumNodes();
    int setNumNodes();

    int getMeshX();
    int setMeshX();
    int getMeshY();
    int setMeshY();
    int getMeshZ();
    int setMeshZ();

    /// Mesh cells
    int getNumCells();
    int setNumCells();

    /// Mesh attributes
}
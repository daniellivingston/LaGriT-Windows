#ifndef __LAGRIT_INTERFACE_HPP__
#define __LAGRIT_INTERFACE_HPP__

#include <string>
#include <iostream>
#include <vector>

#define MAX_STR_LEN 100

extern "C" int INITLAGRIT_C(const char[], const char[], const char[], unsigned int, unsigned int, unsigned int);
extern "C" int DOTASK_C(const char [], unsigned int);

extern "C" int CMO_CREATE_C(const char[], unsigned int);
extern "C" int CMO_GET_NAME_C(char[], unsigned int);
extern "C" int CMO_GET_INFO_C(char[], char[], void*, int*, int*, unsigned int, unsigned int);
extern "C" int CMO_GET_INTINFO_C(char[], char[], int*, int*, int*, unsigned int, unsigned int);
extern "C" int CMO_SET_INFO_C(char[], char[], int*, unsigned int, unsigned int);
extern "C" int CMO_GET_ATTRIBUTE_NAME_C(const char[], int*, char[], unsigned int);
extern "C" int CMO_NEWLEN_C(const char[], unsigned int);
extern "C" int CMO_RELEASE_C(const char[], unsigned int);
extern "C" int CMO_GET_LENGTH_C(const char[], const char[], int*, int*, unsigned int, unsigned int);
// cmo_get_attinfo
// cmo_set_attinfo
// cmo_get_attparam

namespace Lagrit
{
    namespace MeshOptions
    {
        namespace GetInfoOpts
        {
            static const std::string psets = "isetwd";
            static const std::string ialias = "ialias";
            static const std::string nodeMaterials = "imt1";
            static const std::string nodeType = "itp1";
            static const std::string constraintNums = "icr1";
            static const std::string childParent = "isn1";
            static const std::string iGenNums = "ign1";
            static const std::string xVector = "xic";
            static const std::string yVector = "yic";
            static const std::string zVector = "zic";
            static const std::string cellMaterials = "itetclr";
            static const std::string cellGeometry = "itettyp";
            static const std::string eltsets = "xtetwd";
            static const std::string itetOffset = "itetoff";
            static const std::string jtetOffset = "jtetoff";
            static const std::string cellVertices = "itet";
            static const std::string cellConnectivity = "jtet";
        };

        namespace GetIntInfoOpts
        {
            static const std::string numAttributes = "number_of_attributes";
            static const std::string numNodes = "nnodes";
            static const std::string numCells = "nelements";
            static const std::string numFaces = "nfaces";
            static const std::string numEdges = "nedges";
            static const std::string mBoundary = "mbndry";
            static const std::string dimsTopo = "ndimensions_topo";
            static const std::string dimsGeom = "ndimensions_geom";
            static const std::string nodesPerCell = "nodes_per_element";
            static const std::string edgesPerCell = "edges_per_element";
            static const std::string facesPerCell = "faces_per_element";
        }

        enum IType
        {
            integer = 1,
            real = 2,
            character = 3,
            pointer = 4
        };
    }

    struct Nodes
    {
        int size;
        double *x;
        double *y;
        double *z;

        Nodes(int size_, double *x_, double *y_, double *z_)
            : size(size_), x(x_), y(y_), z(z_) {}
    };

    struct Cells {
        int size;
        int cell_rank;
        int* connectivity;
        int* type;

        Cells(int size_, int cell_rank_, int* connectivity_, int* type_) 
            : size(size_), cell_rank(cell_rank_), connectivity(connectivity_), type(type_) {}
        
        ~Cells() {}
    };

    struct Attribute {
        std::string name;
        int length;
        int rank;
        void* data;
        MeshOptions::IType data_type;

        Attribute(std::string n, int l, int r, void* d, MeshOptions::IType dt)
            : name(n), length(l), rank(r), data(d), data_type(dt) {}
        
        ~Attribute() {}
    };

    class Mesh
    {
    public:
        std::string getName();
        Nodes getNodes();
        Cells getCells();
        Attribute getAttributeAtIndex(int index);
        std::vector<Attribute> getAttributes();
        void print();

        int numNodes();
        int numCells();
        int numAttributes();
        void select();

        Mesh();
        Mesh(std::string name_) : name(name_) {}
        ~Mesh();

    private:
        std::string name;
        double* nodesX();
        double* nodesY();
        double* nodesZ();
    };

    int initialize();                                                         // initlagrit
    int initialize(bool noisy);                                               // initlagrit
    int initialize(bool noisy, std::string log_file, std::string batch_file); // initlagrit

    int sendCommand(std::string cmd); // sendline

    Mesh getActiveMesh(); // cmo_get_name
    void* GetInfo(Mesh * mesh, const std::string ioption);  // cmo_get_info
    int GetIntInfo(Mesh * mesh, const std::string ioption); // cmo_get_int_info

    Mesh MeshCreate(const std::string name); // cmo_create
    int SetInfo();                           // cmo_set_info
}

#endif
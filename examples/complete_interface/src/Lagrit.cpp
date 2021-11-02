#include "Lagrit.hpp"

namespace Lagrit {

/*
    Nodes::Nodes(int size_, double* x_, double* y_, double* z_) {
        size = size_;
        x = x_;
        y = y_;
        z = z_;
    }

    Mesh::Mesh() {}
    Mesh::Mesh(std::string name_) {
        name = name_;
        // TODO check if mesh exists
    }
    */

    std::string Mesh::getName() {
        return this->name;
    }

    void Mesh::select() {
        sendCommand("cmo/select/" + this->name);
    }

    void Mesh::print() {
        std::cout << this->name << std::endl;
    }

    int Mesh::numNodes() {
        return GetIntInfo(this, MeshOptions::GetIntInfoOpts::numNodes);
    }

    int Mesh::numCells() {
        return GetIntInfo(this, MeshOptions::GetIntInfoOpts::numCells);
    }

    int Mesh::numAttributes() {
        return GetIntInfo(this, MeshOptions::GetIntInfoOpts::numAttributes);
    }

    double* Mesh::nodesX() {
        void* data_ptr = GetInfo(this, MeshOptions::GetInfoOpts::xVector);
        return (double *)data_ptr;
    }

    double* Mesh::nodesY() {
        void* data_ptr = GetInfo(this, MeshOptions::GetInfoOpts::yVector);
        return (double *)data_ptr;
    }

    double* Mesh::nodesZ() {
        void* data_ptr = GetInfo(this, MeshOptions::GetInfoOpts::zVector);
        return (double *)data_ptr;
    }

    Nodes Mesh::getNodes() {
        int num_nodes = this->numNodes();
        double* x_ptr = this->nodesX();
        double* y_ptr = this->nodesY();
        double* z_ptr = this->nodesZ();

        return Nodes(num_nodes, x_ptr, y_ptr, z_ptr);
    }

    Cells Mesh::getCells() {
        void* data_ptr;
        int num_cells = this->numCells();
        int cell_length = GetIntInfo(this, MeshOptions::GetIntInfoOpts::nodesPerCell);

        data_ptr = GetInfo(this, MeshOptions::GetInfoOpts::cellGeometry);
        int* cell_types = (int *)data_ptr;

        data_ptr = GetInfo(this, MeshOptions::GetInfoOpts::cellConnectivity);
        int* cell_connectivity = (int *)data_ptr;

        return Cells(num_cells, cell_length, cell_connectivity, cell_types);
    }

    Attribute Mesh::getAttributeAtIndex(int index) {
        int length, rank;
        void* data_ptr;
        MeshOptions::IType data_type;
        char* att_name_c;

        int ierr = CMO_GET_ATTRIBUTE_NAME_C(
            this->getName().c_str(),
            &index,
            att_name_c,
            this->getName().size()
        );

        std::string att_name(att_name_c);

        return Attribute(att_name, length, rank, data_ptr, data_type);
    }

    std::vector<Attribute> Mesh::getAttributes() {
        std::vector<Attribute> attributes;
        std::string cmo_name = this->getName();
        unsigned int len_cmo_name = this->getName().size();
        int ierr;
        int num_attrs = this->numAttributes();

        for (int i = 0; i < num_attrs; ++i) {
            attributes.push_back(this->getAttributeAtIndex(i));

            void* data_ptr;
            int itype, length, rank;
            std::string att_name = ;
            unsigned int len_att_name = ;
            ierr = CMO_GET_LENGTH_C(att_name.c_str(), cmo_name.c_str(),
                &length, &rank, cmo_name.size(), att_name.size());
            
            ierr = CMO_GET_ATTINFO_C(att_name.c_str(), cmo_name.c_str(), &data_ptr, &itype);

            MeshOptions::IType data_type;

            switch (itype) {
                case 1:
                    data_type = MeshOptions::IType::integer;
                    break;
                case 2:
                    data_type = MeshOptions::IType::real;
                    break;
                case 3:
                    data_type = MeshOptions::IType::character;
                    break;
                case 4:
                    data_type = MeshOptions::IType::pointer;
                    break;
                default:
                    break;
            }
        }

        return attributes;
    }

    // https://docs.oracle.com/cd/E19205-01/819-5262/6n7bvdr18/
    int GetIntInfo(Mesh* mesh, const std::string ioption) {
        char cmo_c[MAX_STR_LEN];
        char ioption_c[MAX_STR_LEN];

        int iout = -1;
        int lout = -1;
        int itype = -1;

        strcpy(cmo_c, (mesh->getName()).c_str());
        strcpy(ioption_c, ioption.c_str());

        int ierr = CMO_GET_INTINFO_C(
            ioption_c, cmo_c,
            &iout, &lout, &itype,
            strlen(ioption_c), strlen(cmo_c)
        );

        if (ierr != 0) {
            std::cerr << "ERROR: " << cmo_c << "; " << ioption_c << std::endl;
        }

        return iout;
    }

    void* GetInfo(Mesh *mesh, const std::string ioption) {
        char cmo_c[MAX_STR_LEN];
        char ioption_c[MAX_STR_LEN];

        int lout = -1;
        int itype = -1;

        strcpy(cmo_c, (mesh->getName()).c_str());
        strcpy(ioption_c, ioption.c_str());

        void* data_ptr = NULL;

        int ierr = CMO_GET_INFO_C(
            ioption_c, cmo_c,
            &data_ptr, &lout, &itype,
            strlen(ioption_c), strlen(cmo_c)
        );

        if (ierr != 0 || NULL == data_ptr) {
            std::cerr << "ERROR: " << cmo_c << "; " << ioption_c << std::endl;
        }

        return data_ptr;
        //return std::make_pair<void*, MeshOptions::IType>(data_ptr, itype)
    }


    /*
        Initializes LaGriT.
    */
    int initialize(bool noisy, std::string log_file, std::string batch_file) {
        std::string mode;

        if (noisy) {
            mode = "noisy";
        } else {
            mode = "silent";
        }

        int ierr = INITLAGRIT_C(
            mode.c_str(),
            log_file.c_str(),
            batch_file.c_str(),
            mode.size(),
            log_file.size(),
            batch_file.size()
        );

        return ierr;
    }

    int initialize(bool noisy) {
        return initialize(noisy, " ", " ");
    }

    int initialize() {
        return initialize(true);
    }

    /*
        Sends a standard string-based command to LaGriT.
    */
    int sendCommand(std::string cmd) {
        int ierr = -1;
        const char finish[] = "; finish";

        char *cmd_c = new char[cmd.length() + strlen(finish) + 1];
        strcpy(cmd_c, cmd.c_str());
        strcat(cmd_c, finish);

        ierr = DOTASK_C(cmd_c, strlen(cmd_c));
        delete[] cmd_c;
        return ierr;
    }

    /*
        Returns the CMO (current mesh object.)
    */
    Mesh getActiveMesh() {
        char cmo_c[MAX_STR_LEN];
        int ierr = CMO_GET_NAME_C(cmo_c, MAX_STR_LEN);
        std::string cmo(cmo_c);
        return Mesh(cmo);
    }

}
#include "Lagrit.hpp"

namespace Lagrit {

    Mesh::Mesh() {}
    Mesh::Mesh(std::string name_) {
        name = name_;
        // TODO check if mesh exists
    }

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
        return GetIntInfo(this, MeshIntOptions::numNodes);
    }

    int Mesh::numCells() {
        return GetIntInfo(this, MeshIntOptions::numCells);
    }

    double* Mesh::getX() {
        double xxx = 1;
        int xic = GetInfo(this, MeshOptions::xVector);
        return &xxx;
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

    int GetInfo(Mesh *mesh, const std::string ioption) {
        char cmo_c[MAX_STR_LEN];
        char ioption_c[MAX_STR_LEN];

        int lout = -1;
        int itype = -1;

        strcpy(cmo_c, (mesh->getName()).c_str());
        strcpy(ioption_c, ioption.c_str());

        //double xvec[mesh->numNodes()];
        //double *xvec = (double *) malloc(mesh->numNodes() * sizeof(double));

        //void* ptr = NULL;
        int ptr = NULL;

        int ierr = CMO_GET_INFO_C(
            ioption_c, cmo_c,
            &ptr, &lout, &itype,
            strlen(ioption_c), strlen(cmo_c)
        );

        std::cout << "DONE WITH GET INFO\n";
        std::cout << "itype = " << itype << "; lout = " << lout << std::endl;

        if (ierr != 0) {
            std::cerr << "ERROR: " << cmo_c << "; " << ioption_c << std::endl;
        }

        //free(xvec);

        return ierr;
    }


    int initialize(bool noisy, std::string log_file, std::string batch_file) {
        std::string mode;

        if (noisy) {
            mode = "noisy ";
        } else {
            mode = "silent ";
        }

        int ierr = INITLAGRIT_C(
            mode.c_str(),
            log_file.c_str(),
            batch_file.c_str(),
            mode.size(),
            log_file.size(),
            batch_file.size()
            //strlen(mode),
            //strlen(log_file),
            //strlen(batch_file)
        );

        return 0;
    }

    int initialize(bool noisy) {
        return initialize(noisy, " ", " ");
    }

    int initialize() {
        return initialize(true);
    }

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

    Mesh getActiveMesh() {
        char cmo_c[MAX_STR_LEN];
        int ierr = CMO_GET_NAME_C(cmo_c, MAX_STR_LEN);
        std::string cmo(cmo_c);
        return Mesh(cmo);
    }

}
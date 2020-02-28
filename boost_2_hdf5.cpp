#include "highfive/H5File.hpp"
#include "highfive/H5DataSet.hpp"


#include<iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <string>


#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>



int main(int argc, char** argv)
{
    if(argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <file.ser> <file.h5>" << std::endl;
        return EXIT_FAILURE;
    }

    using EdgeCount = uint64_t;

    std::string inFile(argv[1]);
    std::string outFile(argv[2]);


    std::ifstream ifs;
    ifs.exceptions(~ifs.goodbit);
    ifs.open(inFile, std::ios_base::in | std::ios_base::binary);

    boost::archive::binary_iarchive bi(ifs);

    std::vector<EdgeCount> src, dst;
    std::cout << "Reading binary input file: " << inFile << std::endl;
    auto t0 = std::chrono::high_resolution_clock::now();
    bi >> src >> dst;
    auto ts = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - t0);
    ifs.close();
    std::cout << "Number of edges: " << src.size() << std::endl;
    std::cout << "Reading input file took time: " << ts.count() << " sec" << std::endl;


    // we create a new hdf5 file
    HighFive::File file(outFile, HighFive::File::ReadWrite | HighFive::File::Create | HighFive::File::Truncate);

    // let's create a dataset of native integer with the size of the vector 'data'
    HighFive::DataSet dataset_vec1 = file.createDataSet<EdgeCount>("/src",  HighFive::DataSpace::From(src));
    HighFive::DataSet dataset_vec2 = file.createDataSet<EdgeCount>("/dst",  HighFive::DataSpace::From(dst));


    t0 = std::chrono::high_resolution_clock::now();
    //now write them to an hdf5 file
    dataset_vec1.write(src);
    dataset_vec2.write(dst);
    ts = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - t0);
    std::cout << "Writing output file took time: " << ts.count() << " sec" << std::endl;


    return EXIT_SUCCESS;
}

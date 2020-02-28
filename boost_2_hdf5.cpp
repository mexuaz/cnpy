#include "highfive/H5File.hpp"
#include "highfive/H5DataSet.hpp"


#include<iostream>
#include <fstream>
#include <vector>

// Standard filesystem support
#if __cplusplus >= 201703L // C++17 support is available
#include <filesystem>
namespace fs = std::filesystem;
#else
static_assert(__cplusplus >= 201402L, "C++14 at least is required.") // C++14 support
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>



int main(int argc, char** argv)
{
    if(argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file.ser>" << std::endl;
        return EXIT_FAILURE;
    }

    using EdgeCount = uint64_t;

    std::filesystem::path inFile(argv[1]);
    std::filesystem::path outFile(argv[1]);
    outFile.replace_extension(".h5");


    std::ifstream ifs;
    ifs.exceptions(~ifs.goodbit);
    ifs.open(inFile, std::ios_base::in | std::ios_base::binary);

    boost::archive::binary_iarchive bi(ifs);

    std::vector<EdgeCount> src, dst;
    std::cout << "Reading binary input file: " << inFile.filename().string() << std::endl;
    auto t0 = std::chrono::high_resolution_clock::now();
    bi >> src >> dst;
    auto ts = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - t0);
    ifs.close();
    std::cout << "Number of edges: " << src.size() << std::endl;
    std::cout << "Reading input file took time: " << ts.count() << " sec" << std::endl;


    // we create a new hdf5 file
    HighFive::File file(outFile, HighFive::File::ReadWrite | HighFive::File::Create | HighFive::File::Truncate);

    // let's create a dataset of native integer with the size of the vector 'data'
    HighFive::DataSet dataset_vec1 = file.createDataSet<unsigned>("/src",  HighFive::DataSpace::From(src));
    HighFive::DataSet dataset_vec2 = file.createDataSet<unsigned>("/dst",  HighFive::DataSpace::From(dst));


    t0 = std::chrono::high_resolution_clock::now();
    //now write them to an hdf5 file
    dataset_vec1.write(src);
    dataset_vec2.write(dst);
    ts = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - t0);
    std::cout << "Writing output file took time: " << ts.count() << " sec" << std::endl;


    return EXIT_SUCCESS;
}

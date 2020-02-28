#include"cnpy.h"
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
    outFile.replace_extension(".npz");


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

#if(0)
    cnpy::npy_save("src.npy", src); // save single vector to file
    cnpy::npy_save("src.npy", &src[0], {src.size()}); // save vector with shape

    //load it into a new array
    cnpy::NpyArray arr = cnpy::npy_load("arr1.npy");
    EdgeCount* loaded_data = arr.data<EdgeCount>();
    
    //append the same data to file
    //npy array on file now has shape (Nz+Nz,Ny,Nx)
    cnpy::npy_save("arr1.npy", &src[0],{src.size()}, "a");
#endif

    t0 = std::chrono::high_resolution_clock::now();
    //now write to an npz file
    cnpy::npz_save(outFile.string(), "src", src, "w"); // "w" overwrites any existing file
    cnpy::npz_save(outFile.string(), "dst", dst, "a"); // "a" appends to the file we created above
    ts = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - t0);
    std::cout << "Writing output file took time: " << ts.count() << " sec" << std::endl;

#if(0)
    cnpy::NpyArray src2 = cnpy::npz_load(outFile.string(), "src");

    //load the entire npz file
    cnpy::npz_t fl_npz = cnpy::npz_load(outFile.string());
#endif

    return EXIT_SUCCESS;
}

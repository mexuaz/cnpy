#include "highfive/H5File.hpp"
#include "highfive/H5DataSet.hpp"

#include"cnpy.h"

#include<iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <string>


#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

using namespace std;

template <typename T>
void deserialize_text(const string& filename,
                      vector<T> &src,
                      vector<T> &dst,
                      const char comment = '#',
                      function<void(size_t)> size_read_callback = nullptr,
                      size_t callback_freq = 10485750 // call callback every 10MB
                      ){

    ifstream f;

    // set exceptions to be thrown on failure
    f.exceptions(ifstream::failbit | ifstream::badbit);

    f.open(filename);

    string strLine;
    while(true) {
        try {
            getline(f, strLine);
        } catch (ios_base::failure& e) {
            if (!f.eof())
                throw e;
            break;
        }

        // notify callback about the amount read so far
        if (size_read_callback) {
            static size_t sz_read = 0;
            static size_t sz_report = 0;
            sz_read += strLine.size(); // size in bytes read so far
            if((sz_read - sz_report)>callback_freq) { // report every 1MB
                size_read_callback(sz_read);
                sz_report = sz_read;
            }
        }

        // left trim
        strLine.erase(strLine.begin(), find_if(strLine.begin(), strLine.end(), [](int ch) {return !isspace(ch);}));


        // right trim
        strLine.erase(find_if(strLine.rbegin(), strLine.rend(), [](int ch) {return !isspace(ch);}).base(), strLine.end());

        // skip empty lines or comment lines
        if (!strLine.size() || strLine[0] == comment) {
            continue;
        }

        // Tokenize string by space
        istringstream iss(strLine);
        vector<string> tokens{ istream_iterator<string>{iss}, istream_iterator<string>{} };

        if (tokens.size() != 2) {
            throw runtime_error("Unsupported text file.");
        }

        T u, v;
        istringstream issu(tokens[0]), issv(tokens[1]);
        issu >> u; issv >> v;

        src.push_back(u);
        dst.push_back(v);

    } // end of while loop eof

    f.close();
}

template <typename T>
void deserialize_boost(const string& filename,
               vector<T>& src,
               vector<T>& dst) {
    std::ifstream ifs;
    ifs.exceptions(~ifs.goodbit);
    ifs.open(filename, std::ios_base::in | std::ios_base::binary);
    boost::archive::binary_iarchive bi(ifs);
    bi >> src >> dst;
    ifs.close();
}

template <typename T>
void serialize_boost(const string& filename,
               const vector<T>& src,
               const vector<T>& dst) {
    std::ofstream ofs;
    ofs.exceptions(~ofs.goodbit);
    ofs.open(filename, std::ios_base::out | std::ios_base::binary);
    boost::archive::binary_oarchive bo(ofs);
    bo << src << dst;
    ofs.close();
}

template <typename T>
void serialize_hdf5(const string& filename,
               const vector<T>& src,
               const vector<T>& dst) {
    HighFive::File file(filename, HighFive::File::ReadWrite | HighFive::File::Create | HighFive::File::Truncate);

    // let's create a dataset of native integer with the size of the vector 'data'
    HighFive::DataSet dataset_vec1 = file.createDataSet<T>("/src",  HighFive::DataSpace::From(src));
    HighFive::DataSet dataset_vec2 = file.createDataSet<T>("/dst",  HighFive::DataSpace::From(dst));


    //now write them to an hdf5 file
    dataset_vec1.write(src);
    dataset_vec2.write(dst);
}


template <typename T>
void serialize_npz(const string& filename,
               const vector<T>& src,
               const vector<T>& dst) {
    //now write to an npz file
    cnpy::npz_save(filename, "src", src, "w"); // "w" overwrites any existing file
    cnpy::npz_save(filename, "dst", dst, "a"); // "a" appends to the file we created above
}


int main(int argc, char** argv)
{
    if(argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file.ser/file.txt>" << std::endl;
        return EXIT_FAILURE;
    }

    using EdgeCount = uint64_t;

    std::string inFile(argv[1]);

    // Get the file extenstion
    auto ext (inFile.substr(inFile.find_last_of(".")));


    std::cout << "Reading input file: " << inFile << std::endl;
    std::vector<EdgeCount> src, dst;
    auto t0 = std::chrono::high_resolution_clock::now();
    if(ext==".txt") {
            deserialize_text<EdgeCount>(inFile, src, dst);
    } else if(ext==".ser") {
            deserialize_boost<EdgeCount>(inFile, src, dst);
    } else {
        throw runtime_error("Unknown extenstion for input file.");
    }

    auto ts = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - t0);
    std::cout << "Reading input file took time: " << ts.count() << " sec" << std::endl;
    std::cout << "Number of edges: " << src.size() << std::endl;

    if(ext!=".ser") {
        t0 = std::chrono::high_resolution_clock::now();
        serialize_boost<EdgeCount>(inFile.substr(0, inFile.find_last_of("."))+".ser", src, dst);
        ts = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - t0);
        std::cout << "Writing boost file took time: " << ts.count() << " sec" << std::endl;
    }

    t0 = std::chrono::high_resolution_clock::now();
    serialize_hdf5<EdgeCount>(inFile.substr(0, inFile.find_last_of("."))+".h5", src, dst);
    ts = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - t0);
    std::cout << "Writing hdf5 file took time: " << ts.count() << " sec" << std::endl;



    t0 = std::chrono::high_resolution_clock::now();
    serialize_npz<EdgeCount>(inFile.substr(0, inFile.find_last_of("."))+".npz", src, dst);
    ts = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - t0);
    std::cout << "Writing npz file took time: " << ts.count() << " sec" << std::endl;


    return EXIT_SUCCESS;
}

//  ********************************************************************
//  This file is part of KAT - the K-mer Analysis Toolkit.
//
//  KAT is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  KAT is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with KAT.  If not, see <http://www.gnu.org/licenses/>.
//  *******************************************************************
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/timer/timer.hpp>
namespace bfs = boost::filesystem;
using bfs::path;
using boost::timer::auto_cpu_timer;

#include "jellyfish_helper.hpp"
using kat::JellyfishHelper;

#include "input_handler.hpp"

void kat::InputHandler::validateInput() {
    
    bool start = true;
    // Check input file(s) exists
    for(path& p : input) {
        if (!bfs::exists(p) && !bfs::symbolic_link_exists(p)) {
            BOOST_THROW_EXCEPTION(JellyfishException() << JellyfishErrorInfo(string(
                    "Could not find input file ") + lexical_cast<string>(index) + " at: " + p.string() + "; please check the path and try again."));
        }
        
        InputMode m = JellyfishHelper::isSequenceFile(p) ? InputMode::COUNT : InputMode::LOAD;
        
        if (start) {
            mode = m;
        }
        else {
            if (m != mode) {
                BOOST_THROW_EXCEPTION(JellyfishException() << JellyfishErrorInfo(string(
                    "Cannot mix sequence files and jellyfish hashes.  Input: ") + p.string()));
            }
        }        
        
    }
    
    
}

void kat::InputHandler::loadHeader() {
    if (mode == InputMode::LOAD) {
        header = JellyfishHelper::loadHashHeader(input[0]);
    }    
}

void kat::InputHandler::validateMerLen(uint16_t merLen) {    
    
    if (mode == InputMode::LOAD) {
        if (header->key_len() != merLen * 2) {

            BOOST_THROW_EXCEPTION(JellyfishException() << JellyfishErrorInfo(string(
                "Cannot process hashes that were created with different K-mer lengths.  Expected: ") +
                lexical_cast<string>(merLen) + 
                ".  Key length was " + 
                lexical_cast<string>(header->key_len() / 2) + 
                " for : " + input[0].string()));
        }
    }
}

string kat::InputHandler::pathString() {
    
    string s;
    for(path& p : input) {
        s += p.string() + " ";
    }
    return s;
}

void kat::InputHandler::count(uint16_t merLen, uint16_t threads) {
    
    auto_cpu_timer timer(1, "  Time taken: %ws\n\n");      
    
    hashCounter = make_shared<HashCounter>(hashSize, merLen * 2, 7, threads);
        
    cout << "Input is a sequence file.  Counting kmers for " << pathString() << "...";
    cout.flush();

    hash = JellyfishHelper::countSeqFile(input, *hashCounter, canonical, threads);
    
    // Create header for newly counted hash
    header = make_shared<file_header>();
    header->fill_standard();
    header->update_from_ary(*hash);
    header->counter_len(4);  // Hard code for now.
    header->canonical(canonical);
    header->format(binary_dumper::format);    
    
    cout << " done.";
    cout.flush();    
}

void kat::InputHandler::loadHash(bool verbose) {
    
    if (verbose) {
        auto_cpu_timer timer(1, "  Time taken: %ws\n\n");        
    
        cout << "Loading hashes into memory...";
        cout.flush();  
    }
    
    hashLoader = make_shared<HashLoader>();
    hashLoader->loadHash(input[0], false); 
    hash = hashLoader->getHash();
    canonical = hashLoader->getCanonical();
    
    if (verbose) {
        cout << " done.";
        cout.flush();
    }
}

void kat::InputHandler::dump(const path& outputPath, uint16_t threads, bool verbose) {
    
    // Remove anything that exists at the target location
    if (bfs::is_symlink(outputPath) || bfs::exists(outputPath)) {
        bfs::remove(outputPath.c_str());
    }

    // Either dump or symlink as appropriate
    if (mode == InputHandler::InputHandler::InputMode::COUNT) {
    
        auto_cpu_timer timer(1, "  Time taken: %ws\n\n"); 
        cout << "Dumping hash to " << outputPath.string() << " ...";
        cout.flush();

        JellyfishHelper::dumpHash(hash, *header, threads, outputPath);

        cout << " done.";
        cout.flush();
    }
    else {
        bfs::create_symlink(getSingleInput(), outputPath);
    }
    
    
}
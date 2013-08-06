// pykaldi/pykaldi-util.h

// Copyright 2012 Cisco Systems (author: Matthias Paulik)

//   Modifications to the original contribution by Cisco Systems made by:
//   Vassil Panayotov
//   Ondrej Platek

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED
// WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE,
// MERCHANTABLITY OR NON-INFRINGEMENT.
// See the Apache 2 License for the specific language governing permissions and
// limitations under the License.

#ifndef KALDI_PYKALDI_PYKALDIBIN_UTIL_H_
#define KALDI_PYKALDI_PYKALDIBIN_UTIL_H_


/*****************
 *  C interface  *
 *****************/
#ifdef __cplusplus
extern "C" {
#endif


void pykaldi_version(int *out_major, int * out_minor, int *patch);

const char* pykaldi_git_revision();

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
/*******************
 *  C++ interface  *
 *******************/

#include "base/kaldi-common.h"
#include "fstext/fstext-lib.h"

namespace kaldi {

// Reads a decoding graph from a file
fst::Fst<fst::StdArc> *ReadDecodeGraph(std::string filename);

// Prints a string corresponding to (a possibly partial) decode result as
// and adds a "new line" character if "line_break" argument is true
void PrintPartialResult(const std::vector<int32>& words,
                        const fst::SymbolTable *word_syms,
                        bool line_break);

} // namespace kaldi

#endif // __cplusplus
#endif // KALDI_PYKALDI_PYKALDIBIN_UTIL_H_

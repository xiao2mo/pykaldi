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
#ifndef KALDI_PYKALDI_UTILS_H_
#define KALDI_PYKALDI_UTILS_H_
#include <string>
#include "base/kaldi-common.h"
#include "fstext/fstext-lib.h"
#include "lat/kaldi-lattice.h"

#ifdef DEBUG
#include <fstream>
#endif // DEBUG

namespace kaldi {

void pykaldi_version(int *out_major, int * out_minor, int *patch);

void build_git_revision(std::string & pykaldi_git_revision);

// Reads a decoding graph from a file
fst::Fst<fst::StdArc> *ReadDecodeGraph(std::string filename);

// Prints a string corresponding to (a possibly partial) decode result as
// and adds a "new line" character if "line_break" argument is true
void PrintPartialResult(const std::vector<int32>& words,
                        const fst::SymbolTable *word_syms,
                        bool line_break);

// Extract n-best lists from lattice in Tropical semiring.
// Tropical semiring is needed for shortest-path algorithm ?TODO?
double LatticeToNbest(const fst::VectorFst<fst::StdArc> &lat,
                     std::vector<std::vector<int> > &nbest,
                     std::vector<BaseFloat> &prob_out,
                     int n);


std::vector<int32> phones_to_vector(const std::string & s);


// FIXME Copied from lat/lattice-functions.cc
// FIXME does it work with multiple final states? With negated costs?..
// There is no no declaration in lat/lattice-functions.h!
// Computes (normal or Viterbi) alphas and betas; returns (total-prob, or
// best-path negated cost) 
// Note: in either case, the alphas and betas are negated costs.
// Requires that lat be topologically sorted.  This code
// will work for either CompactLattice or Latice.
template<typename LatticeType>
static double ComputeLatticeAlphasAndBetas(const LatticeType &lat,
                                           vector<double> *alpha,
                                           vector<double> *beta) {
  typedef typename LatticeType::Arc Arc;
  typedef typename Arc::Weight Weight;
  typedef typename Arc::StateId StateId;

  StateId num_states = lat.NumStates();
  KALDI_ASSERT(lat.Properties(fst::kTopSorted, true) == fst::kTopSorted);
  KALDI_ASSERT(lat.Start() == 0);
  alpha->resize(num_states, kLogZeroDouble);
  beta->resize(num_states, kLogZeroDouble);

  double tot_forward_prob = kLogZeroDouble;
  (*alpha)[0] = 0.0;
  // Propagate alphas forward.
  for (StateId s = 0; s < num_states; s++) {
    double this_alpha = (*alpha)[s];
    for (fst::ArcIterator<LatticeType> aiter(lat, s); !aiter.Done();
         aiter.Next()) {
      const Arc &arc = aiter.Value();
      double arc_like = -ConvertToCost(arc.weight);
      (*alpha)[arc.nextstate] = LogAdd((*alpha)[arc.nextstate], 
                                        this_alpha + arc_like);
    }
    Weight f = lat.Final(s);
    if (f != Weight::Zero()) {
      double final_like = this_alpha - ConvertToCost(f);
      tot_forward_prob = LogAdd(tot_forward_prob, final_like);
    }
  }
  for (StateId s = num_states-1; s >= 0; s--) { // it's guaranteed signed.
    double this_beta = -ConvertToCost(lat.Final(s));
    for (fst::ArcIterator<LatticeType> aiter(lat, s); !aiter.Done();
         aiter.Next()) {
      const Arc &arc = aiter.Value();
      double arc_like = -ConvertToCost(arc.weight),
          arc_beta = (*beta)[arc.nextstate] + arc_like;
      this_beta = LogAdd(this_beta, arc_beta);
    }
    (*beta)[s] = this_beta;
  }
  double tot_backward_prob = (*beta)[lat.Start()];
  if (!ApproxEqual(tot_forward_prob, tot_backward_prob, 1e-8)) {
    KALDI_WARN << "Total forward probability over lattice = " << tot_forward_prob
               << ", while total backward probability = " << tot_backward_prob;
  }
  // Split the difference when returning... they should be the same.
  return 0.5 * (tot_backward_prob + tot_forward_prob);
}


// Lattice lat has to have loglikelihoods on weights
void MovePostToArcs(fst::VectorFst<fst::LogArc> * lat,
                          const std::vector<double> &alpha,
                          const std::vector<double> &beta);


// the input lattice has to have log-likelihood weights
double LatticeToWordsPost(Lattice &lat, fst::VectorFst<fst::LogArc> *pst);

// the input lattice has to have log-likelihood weights
double CompactLatticeToWordsPost(CompactLattice &lat, fst::VectorFst<fst::LogArc> *pst);


} // namespace kaldi

#endif // KALDI_PYKALDI_UTILS_H_